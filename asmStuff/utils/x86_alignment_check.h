/*
 * x86_alignment_check.h
 *
 *  Created on: Feb 28, 2015
 *      Author: adminuser
 */

#ifndef X86_ALIGNMENT_CHECK_H_
#define X86_ALIGNMENT_CHECK_H_

/* Turn on x86 alignment check (OS needs to have AM set in CR0 to work */
static inline void set_ac(void)
{
	asm volatile (
		"pushfq \n "
	    "pop %%rax \n"
		"or $(1<<18), %%rax \n"
		"push %%rax \n"
	    "popfq \n"
		:::"rax");
}
/* Turn off x86 alignment check */
static inline void clear_ac(void)
{
	asm volatile (
		"pushfq \n "
	    "pop %%rax \n"
		"and $(~(1<<18)), %%rax \n"
		"push %%rax \n"
	    "popfq \n"
		:::"rax");
}



#endif /* X86_ALIGNMENT_CHECK_H_ */
