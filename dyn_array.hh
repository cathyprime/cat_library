#pragma once

/*
 * dyn_array.hh - v1.0.0 - Opinionated Queer License v1.2 - https://github.com/cathyprime/cat_library
 *
 * Author: Magdalena Jaroszyńska
 * Last Edited: 23-11-2024
 */

// TODO: fix unnecessary copies

#include <stddef.h>

namespace cat {

template<typename Item>
class Dyn_Array {
    size_t capacity;
    size_t len;
    Item *data;
    void resize();

  public:
    Dyn_Array();
    Dyn_Array(size_t cap);
    ~Dyn_Array();
    void push(Item it);
    template<typename... Args> void emplace(Args...);
    size_t size() const;
    size_t cap() const;
    void clear();
    Item &get(size_t idx) const;
    Item &operator[](size_t idx);
    void operator+=(Item it);
};
} // namespace cat

#ifndef _DYN_ARRAY_IMPL
#define _DYN_ARRAY_IMPL
#include <stdlib.h>

#define DARR template<typename Item> cat::Dyn_Array<Item>
#define DA(T) template<typename Item> T cat::Dyn_Array<Item>::

DARR::Dyn_Array(size_t cap) :
  capacity(cap),
  len(0),
  data(new Item[cap])
{ }

DARR::Dyn_Array() :
  Dyn_Array(5)
{ }

DARR::~Dyn_Array()
{
    delete[] this->data;
}

DA(void) resize() {
  this->capacity *= 2;
  this->data = (Item *)realloc(this->data, this->capacity);
}

DA(void) push(Item it)
{
    if (this->len == this->capacity) resize();
    this->data[len++] = it;
}

DA(void) clear()
{
    this->len = 0;
    memset(this->data, 0, sizeof(Item*) * this->capacity);
}

DA(Item&) get(size_t idx) const
{
    return this->data[idx];
}

DA(Item&) operator[](size_t idx)
{
    return this->data[idx];
}

DA(size_t) size() const
{
    return this->len;
}

DA(size_t) cap() const
{
    return this->capacity;
}

DA(void) operator+=(Item it)
{
    this->push(it);
}

template<typename Item>
template<typename... Args>
void cat::Dyn_Array<Item>::emplace(Args... args)
{
    if (this->len == this->capacity) resize();
    data[len++] = Item{args...};
}

#endif // _DYN_ARRAY_IMPL
