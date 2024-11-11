#pragma once

/*
 * string.hh - v1.0.0 - Opinionated Queer License v1.2 - https://github.com/cathyprime/cat_library
 *
 * Author: Magdalena Jaroszyńska
 * Last Edited: 10-11-2024
 */

typedef unsigned long int size_t;

void* realloc(void* ptr, size_t size);
#include <string.h>

// TODO: implement strcpy
// TODO: implement strlen

class String;

template<typename T>
concept is_concatable = etl::is_one_of<T, String, const String&, String&&, const char*, char>::value;

class String
{
    size_t m_len;
    size_t m_cap;
    char* m_data;

    void ensure_cap(size_t wanted)
    {
        if (m_cap >= wanted)
            return;

        m_cap = m_cap == 0 ? wanted : m_cap * 2;
        m_data = (char*)realloc(m_data, m_cap);
    }

public:
    String() noexcept :
        m_len(0),
        m_cap(0),
        m_data(nullptr){}

    String(size_t cap) noexcept :
        m_len(0),
        m_cap(cap),
        m_data(new char[cap]){}

    String(const char* c_str) noexcept :
        m_len(strlen(c_str)),
        m_cap(m_len + 1)
    {
        m_data = new char[m_cap];
        strcpy(m_data, c_str);
    }

    String(const String& other) noexcept :
        m_len(other.m_len),
        m_cap(other.m_cap)
    {
        m_data = new char[m_cap];
        strcpy(m_data, other.m_data);
    }

    String(String&& other) noexcept
    {
        m_len = other.m_len;
        m_cap = other.m_cap;
        m_data = other.m_data;
        other.m_data = nullptr;
        other.m_len = 0;
        other.m_cap = 0;
    }

    void operator=(const char* rhs) noexcept
    {
        size_t len = strlen(rhs);
        m_len = len;
        m_cap = len + 1;
        m_data = new char[m_cap];
        strcpy(m_data, rhs);
    }

    void operator=(const String& rhs) noexcept
    {
        if (this == &rhs)
            return;

        if (m_data == nullptr)
            m_data = new char[rhs.m_cap];

        m_cap = rhs.m_cap;
        m_len = rhs.m_len;
        strcpy(m_data, rhs.m_data);
    }

    void operator=(String&& rhs) noexcept
    {
        if (this == &rhs)
            return;

        delete[] m_data;
        m_len = rhs.m_len;
        m_cap = rhs.m_cap;
        m_data = rhs.m_data;
        rhs.m_data = nullptr;
        rhs.m_len = 0;
        rhs.m_cap = 0;

        return;
    }

    ~String()
    {
        delete[] m_data;
    }

    bool is_empty()
    {
        return m_len == 0;
    }

    String& reserve(size_t additional_cap)
    {
        auto empty_space = m_cap - m_len;
        if (empty_space >= additional_cap)
            return *this;

        m_cap += additional_cap;
        m_data = (char*)realloc(m_data, m_cap);
        return *this;
    }

    String& push(char ch)
    {
        ensure_cap(m_len + 1);
        m_data[m_len++] = ch;
        return *this;
    }

    String& push(const String& str)
    {
        if (m_data == nullptr) {
            *this = str;
            return *this;
        }

        ensure_cap(m_len + str.m_len);
        m_len += str.m_len;
        strcat(m_data, str.m_data);
        return *this;
    }

    String& push(const char* c_str)
    {
        if (m_data == nullptr) {
            *this = c_str;
            return *this;
        }

        size_t len = strlen(c_str);
        ensure_cap(len + m_len);
        strcat(m_data, c_str);
        m_len += len;
        return *this;
    }

    String& push(String&& str)
    {
        if (this == &str) return *this;

        if (m_data == nullptr) {
            *this = etl::move(str);
            return *this;
        }

        push(str.c_str());
        delete[] str.m_data;
        str.m_data = nullptr;
        str.m_cap = 0;
        str.m_len = 0;
        return *this;
    }

    size_t len() const
    {
        return m_len;
    }

    size_t cap() const
    {
        return m_cap;
    }

    const char* c_str() const
    {
        return m_data;
    }

    template<typename T>
        requires is_concatable<T>
    void operator+=(T rhs)
    {
        push(rhs);
    }

    char operator[](size_t x) const
    {
        if (x >= m_len)
            *(volatile int*)0 = 0; // segfault if out of bounds :3
        return m_data[x];
    }
};

template<typename T, typename U>
    requires is_concatable<T> && is_concatable<U>
inline String operator+(T lhs, U rhs)
{
    String new_str(lhs);
    new_str.push(rhs);
    return new_str;
}
