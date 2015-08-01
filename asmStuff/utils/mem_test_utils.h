/*
 * mem_test_utils.h
 *
 *  Created on: Jul 5, 2015
 *      Author: adminuser
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
