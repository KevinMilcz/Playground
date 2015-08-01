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


#ifndef UTILS_MIN_FUNCS_H_
#define UTILS_MIN_FUNCS_H_

static inline
int min(int const x, int const y)
{
    return y < x ? y : x;
}

static inline
unsigned minu(unsigned const x, unsigned const y)
{
    return y < x ? y : x;
}

static inline
long minl(long const x, long const y)
{
    return y < x ? y : x;
}

static inline
unsigned long minul(unsigned long const x, unsigned long const y)
{
    return y < x ? y : x;
}

static inline
long long minll(long long const x, long long const y)
{
    return y < x ? y : x;
}

static inline
unsigned long long minull(unsigned long long const x, unsigned long long const y)
{
    return y < x ? y : x;
}

static inline
float minf(float const x, float const y)
{
    return y < x ? y : x;
}

static inline
double mind(double const x, double const y)
{
    return y < x ? y : x;
}

static inline
long double minld(long double const x, long double const y)
{
    return y < x ? y : x;
}

static inline
int max(int const x, int const y)
{
    return y > x ? y : x;
}

static inline
unsigned maxu(unsigned const x, unsigned const y)
{
    return y > x ? y : x;
}

static inline
long maxl(long const x, long const y)
{
    return y > x ? y : x;
}

static inline
unsigned long maxul(unsigned long const x, unsigned long const y)
{
    return y > x ? y : x;
}

static inline
long long maxll(long long const x, long long const y)
{
    return y > x ? y : x;
}

static inline
unsigned long long maxull(unsigned long long const x, unsigned long long const y)
{
    return y > x ? y : x;
}

static inline
float maxf(float const x, float const y)
{
    return y > x ? y : x;
}

static inline
double maxd(double const x, double const y)
{
    return y > x ? y : x;
}

static inline
long double maxld(long double const x, long double const y)
{
    return y > x ? y : x;
}



#endif /* UTILS_MIN_FUNCS_H_ */
