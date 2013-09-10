/*
	Copyright (c) 2013 by Alexander Haase ( https://www.alexanderhaase.net )
	Project: ahmalloc ( https://www.alexanderhaase.net/projects/ahmalloc.html )
	Created: 2013-09-10
	License: LGPL

	Module that counts application allocations

	TODO: rewrite with mmap to avoid destructor worries, maybe leverage MprofRecord?
*/
#include <mprofCount.h>
#include <string.h>
#include <stdio.h>

struct MprofAllocCount {
	size_t malloc;
	size_t free;
	size_t calloc;
	size_t realloc;
	struct MprofAllocCount * nextThread;
};

volatile struct MprofAllocCount * mprofGlobalCounts = NULL;

__thread struct MprofAllocCount * mprofLocalCounts = NULL;

//put a thread-local counter struct on the global list if we need one
static void mprofCountThreadInit( void ) {
	if( mprofLocalCounts == NULL ) {
		mprofLocalCounts = defaultVtable.calloc( sizeof( struct MprofAllocCount ), 1 );

		//atomically push onto global list
		struct MprofAllocCount * oldValue = ( struct MprofAllocCount * ) mprofGlobalCounts;
		struct MprofAllocCount * testValue;
		do {
			testValue = oldValue;
			mprofLocalCounts->nextThread = oldValue;
			oldValue = ( struct MprofAllocCount * ) __sync_val_compare_and_swap( &mprofGlobalCounts, oldValue, mprofLocalCounts );
		} while( testValue != oldValue );
	}
}

static void * mallocCount( size_t in_size ) {
	mprofCountThreadInit();
	mprofLocalCounts->malloc += 1;
	return defaultVtable.malloc( in_size );
}

static void * callocCount( size_t in_size, size_t in_qty ) {
	mprofCountThreadInit();
	mprofLocalCounts->calloc += 1;
	return defaultVtable.calloc( in_size, in_qty );
}

static void freeCount( void * in_ptr ) {
	mprofCountThreadInit();
	mprofLocalCounts->free += 1;
	defaultVtable.free( in_ptr );
}

static void * reallocCount( void * in_ptr, size_t in_size ) {
	mprofCountThreadInit();
	mprofLocalCounts->realloc += 1;
	return defaultVtable.realloc( in_ptr, in_size );
}

static void mprofCountPrintf( const struct MprofAllocCount * counts ) {
	printf( "malloc:\t%llu\tfree:\t%llu\tcalloc:\t%llu\trealloc:\t%llu", 
		(unsigned long long) counts->malloc, 
		(unsigned long long) counts->free, 
		(unsigned long long) counts->calloc, 
		(unsigned long long) counts->realloc );
}

static void mprofCountDestruct( void ) {
	//printf may alloc, protect ourselves
	mprofVtable = defaultVtable;

	struct MprofAllocCount totals;
	memset( &totals, 0, sizeof( struct MprofAllocCount ) );

	size_t nThreads = 0;

	while( mprofGlobalCounts ) {
		struct MprofAllocCount * counts = ( struct MprofAllocCount * ) mprofGlobalCounts;
		totals.malloc += counts->malloc;
		totals.free += counts->free;
		totals.calloc += counts->calloc;
		totals.realloc += counts->realloc;
		printf( "Thread %llu call counts: ", (unsigned long long) nThreads++ );
		mprofCountPrintf( counts );
		printf( "\n" );

		//pop record
		mprofGlobalCounts = ( volatile struct MprofAllocCount * ) counts->nextThread;
		defaultVtable.free( counts );
	}

	printf( "Total of %llu thread(s) call counts: ", (unsigned long long) nThreads );
	mprofCountPrintf( &totals );
	printf( "\n" );
}

const struct AllocatorVtable mprofCountVtable = { &mallocCount, &freeCount, &callocCount, &reallocCount, NULL, &mprofCountDestruct, "Count" };
