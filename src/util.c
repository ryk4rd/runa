#include "util.h"
#include <stdio.h>

void *safe_malloc(size_t size) {
    void *ptr = malloc(size);
    if (ptr == NULL) {
        perror("memory allocation failed");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

void* safe_realloc(void* ptr, size_t new_size) {
    if (new_size == 0) {
        perror("Error: attempt to reallocate with size 0\n");
        return NULL;
    }

    void* new_ptr = realloc(ptr, new_size);

    if (new_ptr == NULL) {
        perror("Error: Memory reallocation failed\n");
        return NULL;
    }

    return new_ptr;
}
