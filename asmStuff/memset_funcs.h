/*
 * asm_memset.h
 *
 *  Created on: Jun 30, 2015
 *      Author: adminuser
 */

#ifndef MEMSET_FUNCS_H_
#define MEMSET_FUNCS_H_

#include <stddef.h>

void* stos_memset(void* dst, int c, size_t sz);
void* c_memset(void* dst, int c, size_t sz);
void* sse2_memset(void*dst, int c, size_t sz);

#endif /* MEMSET_FUNCS_H_ */
