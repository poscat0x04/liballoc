//
// Created by poscat on 11/30/2022.
//
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>

void *malloc_(size_t size) {
    void* p = malloc(size);
    if (p == NULL) {
        fprintf(stderr, "Failed to allocate %zu bytes, wat!?", size);
        abort();
    }

    return p;
};