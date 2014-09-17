//
//  main.c
//  hashtable
//
//  Created by Victor J. Roemer on 3/4/12.
//  Copyright (c) 2012 Victor J. Roemer. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "hashtable.h"

struct key {
    unsigned jA;
    unsigned jB;
    unsigned jC;
    unsigned jD;
};

struct value {
    unsigned junk;
};

int main()
{
    Hash *table = hash_create(1024*1024);
    assert(table != NULL);
    assert(table->buckets == (1024*1024));

    for(int i = 0; i < ((1024*1024)*0.7); i++) {
        struct key key = { i, i+1, i*2, i % 100 };
        struct value *val = malloc(sizeof *val);
        assert(val != NULL);

        int result = hash_insert(table, val, &key, sizeof(key));
        assert(result != -1);
    }

    unsigned it = 0, *p_it = &it;
    struct key *key = NULL;

    for(void *data = hash_first(table, p_it, (void *)&key);
        data; data = hash_next(table, p_it, (void *)&key)) {
        data = hash_remove(table, key, sizeof(*key));
        assert(data != NULL);
        free(data);
    }
    assert(hash_size(table) == 0);

    void *data = hash_get(table, key, sizeof(*key));
    assert(data == NULL);

    hash_destroy(table);

    return 0;
}
