#pragma once

/*
 * arena.hh - v1.0.0 - Opinionated Queer License v1.2 - https://github.com/cathyprime/arena.hh
 *
 * Author: Magdalena Jaroszyńska
 * Last Edited: 10-11-2024
 */

namespace cat {

// TODO: allow for custom allocator

#ifndef THROWING
#define NOEXCEPT noexcept
#define NULL_THROW(alloc) alloc
#else
#include <new>
#define NOEXCEPT
#define NULL_THROW(alloc) alloc; if (alloc == nullptr) throw new std::bad_alloc()
#endif // endif THROWING

#ifndef MEM_ARENA_DEFAULT
#define MEM_ARENA_DEFAULT 64
#endif

#ifndef size_t
#include <stddef.h>
#endif

template<size_t arena_size = MEM_ARENA_DEFAULT>
class Mem_Arena_Stack {
    char arr[arena_size];
    char *m_current = arr;
    size_t m_remaining = arena_size;

    void *alloc(size_t requested)
    {
        if (requested > m_remaining) return nullptr;
        m_remaining -= requested;
        char *ptr = m_current;
        m_current += requested;
        return ptr;
    }
    template<size_t sz>
    friend void *operator new(size_t, Mem_Arena_Stack<sz>*) NOEXCEPT;

  public:
    Mem_Arena_Stack() = default;

    size_t size()
    {
        return arena_size;
    }

    size_t remaining()
    {
        return m_remaining;
    }
};

class Mem_Arena {
    size_t m_remaining = MEM_ARENA_DEFAULT;
    char *m_data;
    char *m_current;

    void *alloc(size_t size)
    {
        if (size > m_remaining) return nullptr;
        m_remaining -= size;
        char *ptr = m_current;
        m_current += size;
        return ptr;
    }
    friend void *operator new(size_t, Mem_Arena*) NOEXCEPT;

  public:
    Mem_Arena() :
      m_data((char*)malloc(m_remaining)),
      m_current(m_data)
    {};

    Mem_Arena(size_t size) :
      m_remaining(size),
      m_data((char*)malloc(m_remaining)),
      m_current(m_data)
    {};

    size_t size()
    {
        return (m_current - m_data) + m_remaining;
    }

    size_t remaining()
    {
        return m_remaining;
    }

    ~Mem_Arena()
    {
        free(m_data);
    }
};

template<size_t arena_size>
void *operator new(size_t size, Mem_Arena_Stack<arena_size> *arena) NOEXCEPT
{
    void *ptr = NULL_THROW(arena->alloc(size));
    return ptr;
}

inline void *operator new(size_t size, Mem_Arena *arena) NOEXCEPT
{
    void *ptr = NULL_THROW(arena->alloc(size));
    return ptr;
}

}
