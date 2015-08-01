/*
 * min_funcs.h
 *
 *  Created on: Jul 5, 2015
 *      Author: adminuser
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
