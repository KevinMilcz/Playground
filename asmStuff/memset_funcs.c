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


#include <stdint.h>
#include <assert.h>
#include "memset_funcs.h"

/*
 * Background of the 0x0101 pattern to fill a register :
 * Each 1 adds the byte into the output, putting a 0 between each 1
 * means that the bytes don't interfere. The same thing works in
 * base ten, try BaseTen(101) x BaseTen(55) = BaseTen(5555)
 *
 * 			  A5
 * 		  x 0101
 * 		  ______
 *			  A5
 *           000 indent 1 zero
 *          A500 indent 2 zeros
 *         00000 indent 3 zeros
 *          A5A5 sum
 */

static void* memset_unaligned_upto_63(void* dst, int c, size_t sz)
{
	int64_t pat = (uint8_t)c * 0x01010101; /* Fills pat with c */

	/* use negative indexes from the end of dest + sz */
	asm volatile (
			"add		%[count], %[dst] \n"
			"neg		%[count] \n"
			"cmp        $-32, %[count] \n"
			"jg         1f \n"

			".p2align 4 \n"
			/* move 32 bytes */
			"movq	%[pattern], (%[dst], %[count]) \n"
			"movq	%[pattern], 8(%[dst], %[count]) \n"
			"movq	%[pattern], 16(%[dst], %[count]) \n"
			"movq	%[pattern], 24(%[dst], %[count]) \n"
			"add	$32, %[count] \n"

			/* move 16 bytes */
			"1: \n"
			"cmp	$-16, %[count] \n"
			"jg		2f \n"
			"movq	%[pattern], (%[dst], %[count]) \n"
			"movq	%[pattern], 8(%[dst], %[count]) \n"
			"add	$16, %[count] \n"

			/* Move 8 bytes */
			"2: \n"
			"cmp	$-8, %[count] \n"
			"jg		3f \n"
			"movq	%[pattern], (%[dst], %[count]) \n"
			"add    $8, %[count] \n"
			"jz     5f \n" /* assume most memsets will be 8 byte multiples, so try to leave early */

			/* move 4 bytes */
			"3: \n"
			"cmp	$-4, %[count] \n"
			"jg		4f \n"
			"movl	%%edx, (%[dst], %[count]) \n"
			"add    $4, %[count] \n"

			/* move 2 bytes */
			"3: \n"
			"cmp	$-2, %[count] \n"
			"jg		4f \n"
			"movw	%%dx, (%[dst], %[count]) \n"
			"add    $2, %[count] \n"

			/* move 1 byte */
			"4: \n"
			"cmp	$-1, %[count] \n"
			"jg		5f \n"
			"movb	%%dl, (%[dst], %[count]) \n"

			"5: \n"
		: 	[count] "+r" (sz)
		:	[dst] "a"(dst), [pattern] "d"(pat)
		:   "memory" );
	return dst;
}

void* sse2_memset(void*dst, int c, size_t sz)
{
	if(sz <= 63)
		return memset_unaligned_upto_63(dst, c, sz);
	else if(sz >= 512*1024)
		return stos_memset(dst, c, sz); /* STOS performs better on very large data */

	void* orig = dst;
	int32_t pat = (uint8_t)c * 0x01010101;

	/* [   v          v   ] */
	/* S   A1         A2  E ;  S=start, A1= first aligned store, A2 = last aligned store, E = end*/
asm volatile (
		"movd %[pattern], %%xmm0 \n"
		"pshufd  $0, %%xmm0, %%xmm0 \n" /* Propagate the data through xmm0 */

		/* Set the first 64 bytes, always, to get aligned. Should be faster than branching
		 * At worst we we write the data twice for sz == 64 bytes*/
		"movdqu    %%xmm0, (%[dst]) \n"
		"movdqu    %%xmm0, 16(%[dst]) \n"
		"movdqu    %%xmm0, 32(%[dst]) \n"
		"movdqu    %%xmm0, 48(%[dst]) \n"

		/* Turn count into a pointer to the last aligned store we can do */
		"lea	-1(%[dst], %[count]), %[count] \n"
		"and	$-64, %[count] \n"

		/* Align dest to the first 64 byte aligned address */
		"add	$64, %[dst] \n"
		"and	$-64, %[dst] \n"

		/* Turn dst into a negative index from count. All this setup was so that we can
		 * Add to dest and check the zero flag without having to do a compare instruction in the loop */
		"sub	%[count], %[dst] \n"
		"jnb    2f \n"
		".p2align 4 \n"

		"1: \n"
		"movdqa  %%xmm0, (%[dst], %[count]) \n"
		"movdqa  %%xmm0, 16(%[dst], %[count]) \n"
		"movdqa  %%xmm0, 32(%[dst], %[count]) \n"
		"movdqa  %%xmm0, 48(%[dst], %[count]) \n"
		"add	$64, %[dst] \n"
		"jnz	1b \n"

		/* Cleanup the last 1-64 bytes. similar to the entry code, we always write out
		 * 64 bytes to avoid branching*/
		"2: \n"
		"movdqu	%%xmm0, -48(%[orig_dst], %[orig_count]) \n"
		"movdqu	%%xmm0, -32(%[orig_dst], %[orig_count]) \n"
		"movdqu	%%xmm0, -16(%[orig_dst], %[orig_count]) \n"
		"movdqu	%%xmm0, -8(%[orig_dst], %[orig_count]) \n"
	: [count] "+r"(sz), [dst] "+r"(dst)
	: [pattern] "r"(pat), [orig_dst] "a" (orig), [orig_count] "r" (sz)
	: "xmm0", "memory" );

	return orig;
}

void* stos_memset(void* dst, int c, size_t sz)
{
	int64_t count 	= sz / 8;
	int64_t rem 	= sz % 8;
	register const int64_t pat = (uint8_t)c * 0x0101010101010101ULL;

	void* orig = dst;

	asm volatile (
		"CLD \n"
		"REP STOSQ \n"
		: "+c"(count), "+D"(dst)
		: "a"(pat)
		: "memory" );

	if(rem > 0)
	{
		asm volatile(
			"REP STOSB \n"
			: "+c"(rem), "+D"(dst)
			: "a"(pat)
			: "memory");
	}
	return orig;
}

void* c_memset(void* dst, int c, size_t sz)
{
	void* orig = dst;

	const int64_t count 	= sz / 8;
	const int64_t rem 	= sz % 8;
	const uint8_t byte = (uint8_t)c;
	register const int64_t pat = byte * 0x0101010101010101ULL;

	int64_t *data = dst;
	for(int i = 0; i < count; ++i) {
		data[i] = pat;
	}

	uint8_t *data8 = (uint8_t*)&data[count];
	for(int i = 0; i < rem; ++i)
	{
		data8[i] = byte;
	}
	return orig;
}
