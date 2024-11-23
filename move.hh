#pragma once

/*
 * move.hh - v1.0.0 - Opinionated Queer License v1.2 - https://github.com/cathyprime/cat_library
 *
 * Author: Magdalena Jaroszyńska
 * Last Edited: 11-11-2024
 */

namespace cat {

template<typename T>
struct remove_reference { typedef T type; };

template<typename T>
struct remove_reference<T&> { typedef T type; };

template<typename T>
struct remove_reference<T&&> { typedef T type; };

template<typename T>
constexpr typename remove_reference<T>::type&& move(T&& arg)
{
    return static_cast<typename remove_reference<T>::type&&>(arg);
}

} // namespace cat
