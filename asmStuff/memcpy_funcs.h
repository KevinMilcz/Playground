/*
 * asmMemcpy.h
 *
 *  Created on: Feb 28, 2015
 *      Author: adminuser
 */

#ifndef MEMCPY_FUNCS_H_
#define MEMCPY_FUNCS_H_

#include <stdint.h>


extern void* sse2_memcpy(void * restrict dest, const void * restrict src, size_t len);
extern void* movs_memcpy(void * restrict dest, const void * restrict src, size_t len);
extern void* c_memcpy(void * restrict dest, const void * restrict src, size_t len);
extern void* byte_copy(void * restrict dest, const void * restrict source, size_t len);
extern void* small_copy(void * restrict dest, const void * restrict source, size_t len);
extern void* really_small_copy(void * restrict dst, const void * restrict src, size_t len);

#endif /* MEMCPY_FUNCS_H_ */
