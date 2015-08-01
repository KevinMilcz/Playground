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


#ifndef MEM_TEST_UTILS_H_
#define MEM_TEST_UTILS_H_

typedef int (*memTestFunc)(size_t bytes, void*your_data);
typedef void* (*memcpy_func_t)(void * restrict dest, const void * restrict src, size_t len);
typedef void* (*memset_func_t) (void *d, int c, size_t sz);

typedef enum {
	TEST_VALIDITY 	= 0x01,
	TEST_SPEED		= 0x02,

	TEST_ALL		= 0x03
}test_types_t;

typedef enum {
	KB             = (1   << 10),
	MB             = (1Ul << 20),
	GB             = (1UL << 30),
	NO_LIMIT        = -1
} test_sizes;

typedef struct {
	const memcpy_func_t 	func;
	const char const *		name;
	test_types_t			tests;
	int64_t					max_size;
}memcpy_container;

typedef struct {
	const memset_func_t 	func;
	const char const *		name;
	test_types_t			tests;
}memset_test_container;

typedef struct {
	memTestFunc			func;
	void*				data;
	const char*			name;
} mem_test_container;


#endif /* MEM_TEST_UTILS_H_ */
