/*
The MIT License (MIT)

Copyright (c) 2015 Kevin Milczewski <kmilczewski@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */


#ifndef UTILS_C11_MACROS_H_
#define UTILS_C11_MACROS_H_

#include "min_funcs.h"

#define MIN(X, Y) (_Generic((X) + (Y),   \
    int:                min,             \
    unsigned:           minu,            \
    long:               minl,            \
    unsigned long:      minul,           \
    long long:          minll,           \
    unsigned long long: minull,          \
    float:              minf,            \
    double:             mind,            \
    long double:        minld)((X), (Y)))

#define MAX(X, Y) (_Generic((X) + (Y),   \
    int:                max,             \
    unsigned:           maxu,            \
    long:               maxl,            \
    unsigned long:      maxul,           \
    long long:          maxll,           \
    unsigned long long: maxull,          \
    float:              maxf,            \
    double:             maxd,            \
    long double:        maxld)((X), (Y)))

#define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))

#endif /* UTILS_C11_MACROS_H_ */
