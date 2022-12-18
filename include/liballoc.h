#pragma once
#if defined _WIN32 || defined __CYGWIN__
  #ifdef BUILDING_LIBALLOC
    #define LIBALLOC_PUBLIC __declspec(dllexport)
  #else
    #define LIBALLOC_PUBLIC __declspec(dllimport)
  #endif
#else
  #ifdef BUILDING_LIBALLOC
      #define LIBALLOC_PUBLIC __attribute__ ((visibility ("default")))
  #else
      #define LIBALLOC_PUBLIC
  #endif
#endif

#define Size size_t

struct Arena_t;

typedef struct Arena_t* Arena;

Arena LIBALLOC_PUBLIC arena_new(Arena parent, Size initial_size, Size max_chunk_size);

Arena LIBALLOC_PUBLIC arena_new_named(Arena parent, Size initial_size, Size max_chunk_size , char* name);

void LIBALLOC_PUBLIC arena_link(Arena parent, Arena child);

void LIBALLOC_PUBLIC *arena_alloc(Arena ar, Size size);

void LIBALLOC_PUBLIC *arena_aligned_alloc(Arena, Size size, Size align);

void LIBALLOC_PUBLIC arena_destroy(Arena);
