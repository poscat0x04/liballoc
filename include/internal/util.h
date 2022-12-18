//
// Created by poscat on 11/30/2022.
//

#ifndef LIBALLOC_UTIL_H
#define LIBALLOC_UTIL_H

#endif //LIBALLOC_UTIL_H

#include <limits.h>

void *malloc_(size_t size);

inline size_t find_size(size_t target, size_t base, size_t max_size) {
    while (base <= SIZE_MAX/2) {
        base = base << 1;
        if (base > max_size) {
            return max_size;
        } else if (target < base) {
            return base;
        }
    }
    return max_size;
}
