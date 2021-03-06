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
