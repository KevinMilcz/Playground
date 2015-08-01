#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>

#include "memcpy_funcs.h"
#include "memset_funcs.h"
#include "utils/x86_alignment_check.h"
#include "utils/c11_macros.h"
#include "utils/mem_test_utils.h"

#define BUFFER_ZONE 128

static const uint64_t target_bytes = 2ULL * GB;
static const size_t mem_sizes[] = {	0x8, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200,
		1*KB, 2*KB, 4*KB, 8*KB, 16*KB, 32*KB, 64*KB, 128*KB, 256*KB, 512*KB,
		1*MB, 2*MB, 4*MB, 8*MB, 16*MB, 32*MB, 64*MB, 128*MB, 256*MB, 512*MB
};

static const size_t small_mem_sizes[] = { 7, 4, 5, 2, 1, 6 , 3 };

/**
 * SIGBUS handling derived from:
 * http://www.linuxprogrammingblog.com/code-examples/SIGBUS-handling
 */
static sigjmp_buf ff_env;

static memcpy_container memcpy_tests[] =
{
		{sse2_memcpy, 	"sse2_memcpy",	TEST_ALL, NO_LIMIT},
		{movs_memcpy, 	"movs_memcpy",	TEST_ALL, NO_LIMIT},
		{c_memcpy,	  	"c_memcpy",	 	TEST_ALL, NO_LIMIT},
		{memcpy,	  	"libc memcpy",	TEST_ALL, NO_LIMIT},
		{byte_copy,	  	"byte_copy",	TEST_VALIDITY, NO_LIMIT},
		{small_copy,  	"small_copy",	TEST_VALIDITY, 63},
		{really_small_copy,	"really_small_copy",	 TEST_ALL, 7},
};
static const size_t num_tests = ARRAY_SIZE(memcpy_tests);

static memset_test_container memset_tests[] =
{
		{stos_memset,	"stos_memset", 	TEST_ALL},
		{c_memset,		"c_memset", 	TEST_ALL},
		{sse2_memset,	"sse2_memset", 	TEST_ALL},
		{memset,	  	"libc memset", 	TEST_SPEED},
};
static const size_t num_memset_tests = ARRAY_SIZE(memset_tests);

/* Not thread/signal safe */
static const char* bytes_to_string(size_t bytes) {
	static char buf[2048];
	if(bytes < KB)
		snprintf(buf, sizeof(buf), "%zu bytes", bytes);
	else if(bytes < MB)
		snprintf(buf, sizeof(buf), "%.2f KB", (double)bytes/KB);
	else if(bytes < GB)
		snprintf(buf, sizeof(buf), "%.2f MB", (double)bytes/MB);
	else
		snprintf(buf, sizeof(buf), "%.2f GB", (double)bytes/GB);

	return buf;
}
void small_speed_test(size_t max_size, mem_test_container *test)
{
	clock_t start, end;

	for(size_t i = 0; i < ARRAY_SIZE(small_mem_sizes) && small_mem_sizes[i] <= max_size; ++i) {
		uint64_t iterations = GB / small_mem_sizes[i];

		start = clock();
		for(size_t k = 0; k < iterations; ++k){
			test->func(small_mem_sizes[i], test->data);
		}
		end = clock();

		double diff = ((double)(end - start)) / CLOCKS_PER_SEC;
		double GiB = (double)1;
		double GiBPerSec = (double)GiB / diff;

		printf("small mem test for (%s) on %s repeated %lu times (%.2f GiB) took %.4fs"
				" %.2f GiB/sec\n",test->name, bytes_to_string(small_mem_sizes[i]), iterations, GiB, diff, GiBPerSec);
	}
}

void testSpeed(size_t max_size, mem_test_container *test)
{
	clock_t start, end;

	for(size_t i = 0; i < ARRAY_SIZE(mem_sizes) && mem_sizes[i] <= max_size; ++i) {
		uint64_t iterations = target_bytes / mem_sizes[i];

		start = clock();
		for(size_t k = 0; k < iterations; ++k){
			test->func(mem_sizes[i], test->data);
		}
		end = clock();

		double diff = ((double)(end - start)) / CLOCKS_PER_SEC;
		double GiB = (double)target_bytes / GB;
		double GiBPerSec = (double)GiB / diff;

		printf("mem test for (%s) on %s repeated %lu times (%.2f GiB) took %.4fs"
				" %.2f GiB/sec\n",test->name, bytes_to_string(mem_sizes[i]), iterations, GiB, diff, GiBPerSec);
	}
}

/* Handle a signal while in the forceFault function */
void forceFaulthandler(int sig)
{
	clear_ac();
	if(sig != SIGBUS)
	{
		printf("%s : got unknown signal %d, terminating \n", __FUNCTION__, sig);
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("%s: Got expected signal %d \n", __FUNCTION__, sig);
		set_ac();
		siglongjmp (ff_env, 1);
	}
}

void mainSigHandler(int sig)
{
	clear_ac();
	printf("%s Got signal %d, trying to continue without align checks \n", __FUNCTION__, sig);
	return;
}

/* Make sure that a misaligned access actually triggers a signal*/
static void forceFault(void)
{
	signal(SIGBUS, forceFaulthandler);
	char* buf = memalign(16, 32);

	/* save this point as a state that we can return to using
	 * siglongjmp(). If this function returns a value other than 0
	 * it means that we are here because of siglongjmp(). */
	if (sigsetjmp(ff_env, 1) == 0) {
		volatile uint32_t *p = (uint32_t *)&buf[2];
		set_ac();
		uint32_t i = *p; //should fault;
		clear_ac();
		printf("Got past a misaligned access, your OS must turn on AM in CR0 for"
				" alignment checks to work. i = %d", i); //Prevent i from being optimized away
	}
	clear_ac();
	free(buf);
	signal(SIGBUS, SIG_DFL);
}

/* Set up 2 memory regions, dest bigger than src. Copy src into dest, leaving buffer zones
 * at the start and end of dest, like this: (X = buffer, S = src)
 * SRC:     [SSSSSSSS]
 * DST:  [XXXSSSSSSSSXXX]
 * Make sure that src is copied correctly, and that the buffer zones are unchanged */
static int memcpy_validity_test(size_t size, memcpy_container *test)
{
	int rc = 0;

	if(test->max_size != NO_LIMIT)
		size = MIN(size, test->max_size);

	const uint32_t padded_size = size + 2*BUFFER_ZONE;

	void* src = memalign(16, padded_size); //Needs to be big enough to compare the buffer zones later
	void* dest = memalign(16, padded_size);

	if(!src || !dest)
	{
		printf("Not enough Memory for validity test! \n");
		rc = -1;
		goto cleanup;
	}

	memset(src, 0xFF, padded_size);
	memset(dest, 0, padded_size);

	void* padded_dest = (void*)((uintptr_t)dest + BUFFER_ZONE);

	signal(SIGBUS, mainSigHandler);
	set_ac();

	test->func(padded_dest, src, size);

	clear_ac();
	signal(SIGBUS, SIG_DFL);

	/* Make sure the data got there */
	rc = memcmp(src, padded_dest, size);

	/* Make sure we didn't copy anything else */
	memset(src, 0, padded_size);
	rc |= memcmp(src, dest, BUFFER_ZONE);
	rc |= memcmp(src, (void*)((uintptr_t)padded_dest + size), BUFFER_ZONE);

	if(rc)
	{
		printf("Destination and source not copied correctly for test %s \n", test->name);
	}
	else
	{
		printf("memcpy validity test passed for %s!\n", test->name);
	}

cleanup:
	if(src) free(src);
	if(dest) free(dest);

	return rc;
}

struct memcpy_data {
	memcpy_func_t func;
	const void* src;
	void* dst;
};

struct memset_data {
	memset_func_t func;
	void* dst;
	char  c;
};

static int memcpy_wrap(size_t bytes, void*data)
{
	struct memcpy_data *input = (struct memcpy_data*)data;
	input->func(input->dst, input->src, bytes);
	return 0;
}

static int memset_wrap(size_t bytes, void*data)
{
	struct memset_data *input = (struct memset_data*)data;
	input->func(input->dst, input->c, bytes);
	return 0;
}


/* Copy src into dest a fixed number of times, and see how long it takes */
static void memcpy_time(void *dest, const void* src, size_t bytes, memcpy_container *test)
{
	struct memcpy_data data = {test->func, src, dest };
	mem_test_container test_data = {memcpy_wrap, &data, test->name};

	small_speed_test(bytes, &test_data);
	if(test->max_size == NO_LIMIT || test->max_size >= 8) {
		testSpeed(bytes, &test_data);
	}

}

static void memset_time(void *dest, int c, size_t bytes, memset_test_container *test)
{
	struct memset_data data = {test->func,dest, c };
	mem_test_container test_data = {memset_wrap, &data, test->name};
	testSpeed(bytes, &test_data);
}

static int memset_validity_test(void* buf, size_t bufSize, memset_test_container *test)
{
	int faultsFound = 0;
	test->func(buf, 'x', bufSize);

	/* TODO add boundary checking like memcpy */
	char* data = buf;
	for(size_t i = 0; i < bufSize; ++i) {
		if(data[i] != 'x') {
			faultsFound++;
		}
	}
	if(faultsFound > 0)
		printf("Memset [%s] validity test failed for %d locations\n", test->name, faultsFound);
	else
		printf("Memset [%s] validity test passed!\n", test->name);

	return faultsFound;
}

static void memset_test() {
	size_t bufSize = MB * 32;
	char* buf = malloc(bufSize);
	if(!buf) return;

	for(size_t i = 0; i < num_memset_tests; i++)
	{
		if(memset_tests[i].tests & TEST_VALIDITY) {
			memset_validity_test(buf, bufSize, &memset_tests[i]);
		}
		if(memset_tests[i].tests & TEST_SPEED) {
			memset_time(buf, 'x', bufSize, &memset_tests[i]);
		}
	}
	free(buf);
}

int main (int argc, char** argv)
{
	int err = 0;
	size_t i;

	if(argc < 2)
	{
		printf("Usage: <size in MB to test>\n");
		exit(EXIT_FAILURE);
	}
	const uint32_t size_MB = atoi(argv[1]);

	if(size_MB < 1 || size_MB > 1024)
	{
		fprintf(stderr, "Error, input size %.16s (possibly truncated) not valid\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	/* Make sure we can do alignment checks */
	forceFault();
	signal(SIGBUS, mainSigHandler);

	memset_test();

	const uint32_t size = size_MB * MB;
	void* src = memalign(16, size);
	void* dest = memalign(16, size);

	if(!src || !dest)
	{
		printf("Not enough memory for tests! \n");
		err = -1;
		goto cleanup;
	}

	//The memory is probably not really allocated until we use it...
	memset(src, 0, size);
	memset(dest, 0, size);

	for(i = 0; i < num_tests; i++)
	{
		if(memcpy_tests[i].tests & TEST_VALIDITY)
		{
			err |= memcpy_validity_test(1*KB + 3, &memcpy_tests[i]);
		}

		if(memcpy_tests[i].tests & TEST_SPEED)
		{
			memcpy_time(dest, src, size, &memcpy_tests[i]);
		}
	}

	printf("\nTests complete, return code = %d \n", err);
cleanup:
	if(src) 	free(src);
	if(dest)	free(dest);

    return err;
}
