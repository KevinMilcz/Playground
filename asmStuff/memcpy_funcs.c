/*
 * asmMemcpy.c
 *
 *  Created on: Jan 20, 2015
 *      Author: Kevin Milczewski
 */

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "memcpy_funcs.h"

void* sse2_memcpy(void * restrict dest, const void * restrict src, size_t len)
{
	if(len < 64) {
		return small_copy(dest, src, len);
	}
	if( len > 512 * 1024) {
		return movs_memcpy(dest, src, len);
	}

	void * original_dest = dest;
	register int64_t loop_end = len & ~(64-1);
	register int64_t byte_count = 0;
	int64_t remainder = len & (64-1);

	asm volatile(
			"1: \n"
			"movdqu 	(%[src_reg],   %[count]), %%xmm0 \n"
			"movdqu 	16(%[src_reg], %[count]), %%xmm1 \n"
			"movdqu 	32(%[src_reg], %[count]), %%xmm2 \n"
			"movdqu 	48(%[src_reg], %[count]), %%xmm3 \n"

			"movdqu 	%%xmm0, (%[dst_reg],   %[count]) \n"
			"movdqu 	%%xmm1, 16(%[dst_reg], %[count]) \n"
			"movdqu 	%%xmm2, 32(%[dst_reg], %[count]) \n"
			"movdqu 	%%xmm3, 48(%[dst_reg], %[count]) \n"
			"add		$64, %[count]	 \n"
			"cmp		%[count], %[loop_end] \n"
			"ja			1b \n"
			: [count] "+r" (byte_count)
			: [src_reg] "r"(src), [dst_reg] "r" (dest), [loop_end] "r"(loop_end)
			: "xmm0", "xmm1", "xmm2", "xmm3", "memory");
	if(remainder != 0)
	{
		small_copy(dest + byte_count, src + byte_count, remainder);
	}
	return original_dest;
}

void* small_copy(void * restrict dst, const void * restrict src, size_t len)
{
	void* origin = dst;

	assert(len <64);

	/* Go backwards so indexing is easier */
	/* Fall through from big to small, 32-16-8-4-2-1 */
	/* Have to use local labels because the code gets inlined during optimization
	 * 	causing duplicate labels :(  */
	asm volatile(
			"add		%[count], %[src_reg] \n"
			"add		%[count], %[dst_reg] \n"
			"neg		%[count] \n"
			"cmp        $-0x20, %[count] \n"
			"jg         1f \n"

			/* copy 32 bytes */
			"movups 	(%[src_reg], %[count]), %%xmm0 \n"
			"movups 	0x10(%[src_reg], %[count]), %%xmm1 \n"
			"movups 	%%xmm0, (%[dst_reg], %[count]) \n"
			"movups 	%%xmm1, 0x10(%[dst_reg], %[count]) \n"
			"add        $0x20, %[count] \n"

			"1: \n" /* copy 16 bytes */
			"cmp		$-0x10, %[count] \n"
			"jg         2f \n"
			"movups 	(%[src_reg], %[count]), %%xmm0 \n"
			"movups 	%%xmm0, (%[dst_reg], %[count]) \n"
			"add        $0x10, %[count] \n"

			"2: \n" /* copy 8 bytes */
			"cmp		$-0x8, %[count] \n"
			"jg         3f \n"
			"movq       (%[src_reg], %[count]), %%rax \n"
			"movq       %%rax, (%[dst_reg], %[count]) \n"
			"add        $0x8, %[count] \n"

			"3: \n" /* copy 4 bytes */
			"cmp		$-0x4, %[count] \n"
			"jg        	4f \n"
			"mov       	(%[src_reg], %[count]), %%eax \n"
			"mov       	%%eax, (%[dst_reg], %[count]) \n"
			"add       	$0x4, %[count] \n"
			"jz         6f \n" /*Early exit if divisible by 4 */

			"4: \n" /* copy 2 bytes */
			"cmp		$-0x2, %[count] \n"
			"jg         5f \n"
			"mov       	(%[src_reg], %[count]), %%ax \n"
			"mov       	%%ax, (%[dst_reg], %[count]) \n"
			"add        $0x2, %[count] \n"

			"5: \n" /* copy 1 byte */
			"cmp		$-0x1, %[count] \n"
			"jg         6f \n"
			"mov       	(%[src_reg], %[count]), %%al \n"
			"mov       	%%al, (%[dst_reg], %[count]) \n"
			"add       	$0x1, %[count] \n"

			"6: \n" /* Done */

			: [count] "+r"(len), [src_reg] "+r"(src), [dst_reg] "+r" (dst)
			:
			: "xmm0","xmm1", "rax", "memory");
	return origin;
}

void* byte_copy(void * restrict dest, const void * restrict source, size_t len)
{
	uint32_t i;
	uint8_t *restrict dst = dest;
	const uint8_t * restrict src = source;

	for(i = 0; i < len; i++)
	{
		dst[i] = src[i];
	}
	return dest;
}

void* c_memcpy(void * restrict dest, const void * restrict src, size_t len)
{
	int64_t count 	= len / 8;
	int64_t rem 	= len % 8;

	uint64_t *dst64 = dest;
	const uint64_t *src64 = src;
	for(int i = 0; i < count; ++i) {
		dst64[i] = src64[i];
	}

	uint8_t *dst8 = (uint8_t*)&dst64[count];
	const uint8_t *src8 = (uint8_t*)&src64[count];
	for(int i = 0; i < rem; ++i)
	{
		dst8[i] = src8[i];
	}
	return dest;
}

void* movs_memcpy(void * restrict dest, const void * restrict src, size_t len)
{
	uint32_t count, rem;
	void * original_dest = dest;
	if(len < 64)
	{
		return small_copy(dest, src, len);
	}
	else
	{
		count 	= len / 8;
		rem 	= len % 8;
		asm volatile (
			"CLD \n"
			"REP MOVSQ \n"
			: "+c"(count), "+S"(src), "+D"(dest)
			:
			: "memory" );
		if(rem > 0)
		{
			really_small_copy(dest, src, rem);
		}
	}
	return original_dest;
}

void* really_small_copy(void * restrict dst, const void * restrict src, size_t len)
{
	int64_t target = ((len & 7) << 2);

	/* Jump table: index into a table of jumps by the last 3 bits,
	 * each jump points to an unrolled group of moves
	 */
	asm volatile (
			"mov	$start%=, %%rax \n"
			"add    %[target], %%rax \n"
			"jmp	*%%rax \n"
			"start%=: \n"

			/* Hopefully these all get assembled as 2 byte jumps or i'm screwed... */
			/* 0 */
			"jmp dummy%= \n"
			/* 1 */
			"jmp dummy%= \n"
			/* 2  1 byte lands here*/
			"jmp 1f \n"
			/* 3  */
			"jmp dummy%= \n"
			/* 4  2 bytes lands here*/
			"jmp 2f \n"
			/* 5 */
			"jmp dummy%= \n"
			/* 6 3 bytes land here */
			"jmp 3f \n"
			/* 7  */
			"jmp dummy%= \n"
			/* 8  4 bytes land here*/
			"jmp 4f \n"
			/* 9  */
			"jmp dummy%= \n"
			/* 10 5 bytes lands here  */
			"jmp 5f \n"
			/* 11  */
			"jmp dummy%= \n"
			/* 12 6 bytes lands here  */
			"jmp 6f \n"
			/* 13  */
			"jmp dummy%= \n"
			/* 14 7 bytes lands here  */
			"jmp 7f \n"
			/* 15  */
			"dummy%=: \n" /* Dummy target for other jumps so they dont try to jump more than 127 bytes */
			"jmp done%= \n"	  /* Because it's the last entry in the table we can jump farther than 127 bytes */

			".align 4\n"
			"7: \n"	/* 7 bytes */
			"movl	(%[src], %[cnt]), %%eax \n"
			"movl	%%eax, (%[dst], %[cnt]) \n"
			"add	$4, %[cnt] \n"
			"3: \n" /* 3 bytes */
			"movw	(%[src], %[cnt]), %%cx \n"
			"movw	%%cx, (%[dst], %[cnt]) \n"
			"add	$2, %[cnt] \n"
			"1: \n" /* 1 byte */
			"movb	(%[src], %[cnt]), %%dl \n"
			"movb	%%dl, (%[dst], %[cnt]) \n"
			"jmp	done%= \n"

			".align 4\n"
			"6: \n" /* 6 bytes */
			"movl	(%[src], %[cnt]), %%eax \n"
			"movl	%%eax, (%[dst], %[cnt]) \n"
			"add	$4, %[cnt] \n"
			"2: \n" /* 2 bytes */
			"movw	(%[src], %[cnt]), %%cx \n"
			"movw	%%cx, (%[dst], %[cnt]) \n"
			"jmp	done%= \n"

			".align 4\n"
			"5: \n" /*5 bytes */
			"movl	(%[src], %[cnt]), %%eax \n"
			"movl	%%eax, (%[dst], %[cnt]) \n"
			"movb	4(%[src], %[cnt]), %%cl \n"
			"movb	%%cl, 4(%[dst], %[cnt]) \n"
			"jmp	done%= \n"

			".align 4\n"
			"4: \n" /* 4 bytes */
			"movl	(%[src], %[cnt]), %%eax \n"
			"movl	%%eax, (%[dst], %[cnt]) \n"
			"jmp	done%= \n"

			".align 4\n"
			"2: \n" /* 2 bytes */
			"movw	(%[src], %[cnt]), %%ax \n"
			"movw	%%ax, (%[dst], %[cnt]) \n"

			"done%=: \n"
			: [target] "+r" (target)
			: [src] "r"(src), [dst] "r"(dst), [cnt] "r"(0ULL)
			: "rax", "rcx", "rdx"
	);
	return dst;
}
