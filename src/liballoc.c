#include <stddef.h>
#include <malloc.h>
#include <assert.h>

#include "liballoc.h"
#include "internal/util.h"

/* This function will not be exported and is not
 * directly callable by users of this library.
 */
#define chunk_size sizeof(struct Chunk_t)
#define arena_size sizeof(struct Arena_t)
#define al_size sizeof(struct ArenaList_t)

const Size DEFAULT_ALIGN = 8;

typedef struct Chunk_t* Chunk;
typedef struct ArenaList_t* ArenaList;

struct ArenaList_t {
    Arena arena;
    ArenaList next;
};

struct Chunk_t {
    Size size;
    Chunk prev;
    void* current;
    void* limit;
};

struct Arena_t {
    char* name;
    Size size;
    Size max_chunk_size;
    Arena parent;
    ArenaList children;
    Chunk chunks;
};

static ArenaList arena_list_cons(Arena ar, ArenaList al) {
    ArenaList al_new = malloc_(al_size);
    al_new->next = al;
    al_new->arena = ar;
    return al_new;
}

static ArenaList arena_list_remove(Arena ar, ArenaList al) {
    if (al == NULL)
        return NULL;
    else if (al->arena == ar) {
        return arena_list_remove(ar, al->next);
    } else {
        ArenaList al_tail = arena_list_remove(ar, al->next);
        al->next = arena_list_remove(ar, al_tail);
        return al;
    }
}

static void arena_list_free(ArenaList al) {
    while (al != NULL) {
        arena_destroy(al->arena);
        ArenaList tmp = al->next;
        free(al);
        al = tmp;
    }
}

static void chunk_free(Chunk chunk) {
    Chunk tmp;
    do {
        tmp = chunk;
        chunk = chunk->prev;
        free(tmp);
    } while (chunk != NULL);
}

static Chunk chunk_new(Size size) {
    void* p = malloc_(size + chunk_size);
    Chunk c = p;
    c->size = size;
    c->prev = NULL;
    c->current = (char*)p + chunk_size;
    c->limit = (char*)c->current + size;
    return c;
}

Arena arena_new(Arena parent, Size initial_size, Size max_chunk_size) {
    return arena_new_named(parent, initial_size, max_chunk_size, NULL);
}

Arena arena_new_named(Arena parent, Size initial_size, Size max_chunk_size, char* name) {
    if (max_chunk_size <= initial_size || initial_size == 0)
        return NULL;

    Arena ar = malloc_(arena_size);

    ar->name = name;
    ar->parent = parent;
    ar->chunks = chunk_new(initial_size);
    ar->size = initial_size + arena_size + chunk_size;
    ar->max_chunk_size = max_chunk_size;

    if (parent != NULL)
        parent->children = arena_list_cons(ar, parent->children);

    return ar;
}

void arena_link(Arena parent, Arena child) {
    child->parent = parent;
    parent->children = arena_list_cons(child, parent->children);
}

void *arena_alloc(Arena ar, Size size) {
    return arena_aligned_alloc(ar, size, DEFAULT_ALIGN);
}

void *arena_aligned_alloc(Arena ar, Size size, Size align) {
    if (size > ar->max_chunk_size)
        return NULL;

    // the chunk to allocate from
    Chunk chunk = ar->chunks;

    Size align_offset = (align - ((uintptr_t)chunk->current % align)) % align;

    ptrdiff_t free_space = (char*)chunk->limit - (char*)chunk->current;

    assert(free_space >= 0);
    if ((size_t)free_space < (size + align_offset)) {
        // allocate (size + align) bytes to ensure that we can always find an aligned spot
        Size new_chunk_size = find_size(size + align, chunk->size, ar->max_chunk_size);
        Chunk new_chunk = chunk_new(new_chunk_size);

        // prepends the new chunk to the chunk list
        new_chunk->prev = chunk;

        // update the arena size
        ar->size += chunk_size + new_chunk_size;

        // update variable chunk
        chunk = new_chunk;
    }
    // finds the first address that satisfies the alignment constraint
    align_offset = (align - ((uintptr_t)chunk->current % align)) % align;

    // prepares the pointers
    void *tmp = (char*)chunk->current + align_offset;
    chunk->current = (char*)tmp + size;

    // point the arena to the newly allocated chunk
    ar->chunks = chunk;

    return tmp;
}

void arena_destroy(Arena ar) {
    if (ar->parent != NULL)
        ar->parent->children = arena_list_remove(ar, ar->parent->children);
    chunk_free(ar->chunks);
    arena_list_free(ar->children);
    free(ar);
}
