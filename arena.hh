#pragma once

/*
 * arena.hh - v2.0.0 - Opinionated Queer License v1.2 - https://github.com/cathyprime/arena.hh
 *
 * Author: Magdalena Jaroszyńska
 * Last Edited: 10-11-2024
 */

namespace cat {

// TODO: allow for custom allocator

#ifndef CAT_MEM_ARENA_DEFAULT
#define CAT_MEM_ARENA_DEFAULT 256
#endif

#include <stddef.h>

// TODO: Implement cat::Mem_Arena_Stack

class Mem_Arena {
    size_t m_capacity = CAT_MEM_ARENA_DEFAULT;
    char *m_data;
    char *m_current;

    using Dtor_Ptr = void(*)(const void*);

    struct metadata_t {
        size_t size;
        Dtor_Ptr dtor;
    };

  public:
    template<typename T> void *alloc()
    {
        constexpr size_t size = sizeof(T);
        constexpr size_t metadata_size = sizeof(metadata_t);

        if (size > m_capacity)
            return nullptr;

        metadata_t *meta = (metadata_t*)m_current;
        *meta = {
            .size = size,
            .dtor = [](const void *ptr) {
                static_cast<const T*>(ptr)->~T();
            }
        };
        m_current += metadata_size;

        void *ptr = m_current;
        m_current += size;
        return ptr;
    }

    Mem_Arena()
      : m_data((char*)malloc(m_capacity))
      , m_current(m_data)
    {};

    Mem_Arena(size_t size)
      : m_capacity(size)
      , m_data((char*)malloc(m_capacity))
      , m_current(m_data)
    {};

    size_t size()
    {
        return (size_t)m_capacity;
    }

    ~Mem_Arena()
    {
        constexpr size_t metadata_size = sizeof(metadata_t);
        char *pos = m_data;
        while (pos < m_current) {
            metadata_t *meta = (metadata_t*)pos;
            meta->dtor(pos + metadata_size);
            pos += meta->size + metadata_size;
        }
        free(m_data);
    }
};

}
