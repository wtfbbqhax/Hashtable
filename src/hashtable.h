//
//  hashtable.h
//  hashtable
//
//  Lightweight hashtable implementation in C.
//
//  Created by Victor J. Roemer on 3/4/12.
//  Copyright (c) 2012 Victor J. Roemer. All rights reserved.
//

#ifndef hashtable_h
#define hashtable_h

#include <stdlib.h>
#include <stdbool.h>

typedef struct
{
    bool filled;
    void *value;
    size_t keysize;
    void *key;
} Bucket;

typedef struct
{
    size_t buckets;
    size_t size;
    Bucket **table;
} Hash;

#define hash_size(table) ((table)->size)

Hash *hash_create(size_t buckets);

void hash_destroy(Hash *this);

int hash_insert(Hash *table, void *data, void *key, size_t keysize);

void *hash_remove(Hash *this, void *key, size_t keysize);

void *hash_first(Hash *this, unsigned *it, const void **key);

void *hash_next(Hash *this, unsigned *it, const void **key);

void *hash_get(Hash *this, void *key, size_t keysize);

void hash_dump(Hash *table);

#endif /* hashtable_h */
