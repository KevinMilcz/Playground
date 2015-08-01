/*
 * c11_macros.h
 *
 *  Created on: Jul 4, 2015
 *      Author: adminuser
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
