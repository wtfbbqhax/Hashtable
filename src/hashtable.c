//
//  hashtable.c
//  hashtable
//
//  Created by Victor J. Roemer on 3/4/12.
//  Copyright (c) 2012 Victor J. Roemer. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>

#include <sys/types.h>

#include "hashtable.h"
#include "digest.h"

/*
 * hash_create
 *
 * Allocate space for a new table and initialize all the table elements.
 */
Hash *hash_create(size_t buckets)
{
    Hash *this = calloc(1, sizeof(*this));
    if (this == NULL) {
        return NULL;
    }

    this->table = (Bucket **)calloc(buckets, sizeof(*(this->table)));
    if (this->table == NULL) {
        free(this);
        return NULL;
    }

    this->buckets = buckets;
    this->size = 0;

    for (size_t i = 0; i < this->buckets; ++i) {
        this->table[i] = NULL;
    }

    return this;
}

/*
 * hash_destroy
 *
 * Will not remove entries in the table so make sure you take care of that.
 */
void hash_destroy(Hash *this)
{
    assert(this != NULL);
    assert(this->table != NULL);
    assert(this->size == 0);

    for (size_t i = 0; i < this->buckets; ++i)
        free(this->table[i]);

    free(this->table);
    free(this);
}

/*
 * bucket_create
 *
 * Create a new bucket.
 */
static inline Bucket *bucket_create(void *value, void *key, size_t keysize)
{
    Bucket *bucket = NULL;
    if ((bucket = malloc(keysize + sizeof(*bucket))) == NULL) {
        return NULL;
    }

    bucket->keysize = keysize;
    bucket->value = value;
    bucket->filled = true;

    /* store key with bucket, bucket->key will point into itself */
    bucket->key = &bucket->key+1;
    memcpy(bucket->key, key, keysize);

    return bucket;
}

/* 
 * hash_insert
 *
 * Insert a new key/value pair.
 */
int hash_insert(Hash *this, void *value, void *key, size_t keysize)
{
    /** XXX Check for loadfactor >= 0.7 and resize table here */

    unsigned long idx = siphash % this->buckets;
    
    for (size_t i = 1; i < this->buckets; ++i) {
        if (this->table[idx] == NULL) {
            this->table[idx] = bucket_create(value, key, keysize);
            this->size++;
            return 0;
        }
        else if (this->table[idx]->filled == false) {
#ifndef KEYSIZE_OPTIMIZATION
            /* if the keysize in the bucket does not match the keysize
             * then we need to recreate the bucket. */
            if (this->table[idx]->keysize != keysize)  {
                free(this->table[idx]);
                this->table[idx] = bucket_create(value, key, keysize);
                return 0;
            }
#endif
            this->table[idx]->filled = true;
            memcpy(this->table[idx]->key, key, keysize);
            this->table[idx]->keysize = keysize;
            this->table[idx]->value = value;
            this->size++;
            return 0;
        }

        /* bucket was already filled, find a new bucket */
        idx = (idx + i*i) % this->buckets;
    }
    
    return -1;
}

/*
 * hash_remove
 *
 * Remove key/value pair from table and return the value.
 */
void *hash_remove(Hash *this, void *key, size_t keysize)
{
    unsigned long idx = fnv1a_digest(key, keysize, 0x811c9dc5) % this->buckets;

    for (size_t i = 0; i < this->buckets; i++) {
        if (this->table[idx] == NULL)
            return NULL;

        if (this->table[idx]->filled == false) {
            idx = (idx + i*i) % this->buckets;
            continue;
        }

        if (memcmp(key, this->table[idx]->key,
            this->table[idx]->keysize) == 0) {
            this->table[idx]->filled = false;
            this->size--;
            return this->table[idx]->value;
        }

        idx = (idx + i*i) % this->buckets;
    }
    
    return NULL;
}

/*
 * hash_get
 *
 * Return value for the key in the table.
 */
void *hash_get(Hash *this, void *key, size_t keysize)
{
    unsigned long idx = fnv1a_digest(key, keysize, 0x811c9dc5) %
        this->buckets;

    for (size_t i = 0; i < this->buckets; i++) {
        if (this->table[idx] == NULL)
            return NULL;

        if (this->table[idx]->filled == false)
            continue;

        else
        if (memcmp(key, this->table[idx]->key,
            this->table[idx]->keysize) == 0)
            return this->table[idx]->value;

        idx = (idx + i*i) % this->buckets;
    }

    return NULL;
}

/*
 * hash_first
 *
 * return the first element in the hash table.
 */
void *hash_first(Hash *this, unsigned *it, const void **key)
{
    for (*it = 0; (*it) < this->buckets; (*it)++) {
        if (this->table[*it] == NULL)
            continue;
        if (this->table[*it]->filled) {
            *key = this->table[*it]->key;
            return this->table[*it]->value;
        }
    }

    return NULL;
}

/*
 * hash_next
 *
 * return the next element in the hash table.
 */
void *hash_next(Hash *this, unsigned *it, const void **key)
{
    for ((*it)++; (*it) < this->buckets; (*it)++) {
        if (this->table[*it] == NULL)
            continue;
        if (this->table[*it]->filled) {
            *key = this->table[*it]->key;
            return this->table[*it]->value;
        }
    }

    return NULL;
}

/*
 * hash_dump
 *
 * display the contents of the hash table.
 */
void hash_dump(Hash *this)
{
    unsigned long memuse = sizeof(*this) + ((sizeof(Bucket)+16)*this->buckets);
    printf("Fixed memory usage = %lu\n", memuse);
    for (size_t i = 1; i < this->buckets; ++i) {
        if (this->table[i] != NULL && this->table[i]->filled)
            printf("[%u][ full ]\n", (unsigned)i);
    }
}
