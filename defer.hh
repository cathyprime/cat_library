#pragma once

/*
 * defer.hh - v1.0.0 - Opinionated Queer License v1.2 - https://github.com/cathyprime/cat_library
 *
 * Author: Magdalena Jaroszyńska
 * Last Edited: 11-11-2024
 */

template<typename F>
class Defer_Class {
    F f;
  public:
    template<typename T>
    Defer_Class(T &&f_) : f(f_){}

    ~Defer_Class() {
        f();
    }
};

inline struct {
    template<typename F>
    Defer_Class<F> operator<<(F && f) {
        return Defer_Class<F>(f);
    }
} deferrer;

#define __TOKENPASTE(x, y) x ## y
#define __TOKENPASTE2(x, y) __TOKENPASTE(x, y)
#define defer Defer_Class __TOKENPASTE2(__deferred_lambda_call, __COUNTER__) = deferrer << [&]()
