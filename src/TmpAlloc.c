/*
	Copyright (c) 2013 by Alexander Haase ( https://www.alexanderhaase.net )
	Project: ahmalloc ( https://www.alexanderhaase.net/projects/ahmalloc.html )
	Created: 2013-09-10
	License: LGPL

	Very limited temporary allocator for boostrapping.
*/
#include <TmpAlloc.h>
#include <stdio.h>
#include <string.h>

static char tmpBuf[ 4096 ];
static const size_t tmpSize = 4096;
static size_t tmpRemaining = 4096;

size_t tmpAllocRemaining( void ) { return tmpRemaining; }

static void * mallocTmpAlloc( size_t in_size ) {
	if( in_size > tmpRemaining ) {
		perror( "ERROR: libmprof: mprofLDInit.c: tmp allocator out of space!" );
		return NULL;
	} else {
		void * ret = &tmpBuf[ tmpSize - tmpRemaining ];
		tmpRemaining -= in_size;
		return ret;
	}
}

static void freeTmpAlloc( __attribute__((unused)) void * in_ptr ) {}

static void * callocTmpAlloc( size_t in_qty, size_t in_size ) {
	in_size *= in_qty;
	void * ret = mallocTmpAlloc( in_size );
	if( ret ) {
		memset( ret, 0, in_size );
	}
	return ret;
}

static void * reallocTmpAlloc( __attribute__((unused)) void * in_ptr, __attribute__((unused)) size_t in_size ) {
	perror( "Warn: libmprof: TmpAlloc does not impliment realloc." );
	return NULL;
}

const struct AllocatorVtable tmpAllocVtable = { mallocTmpAlloc, freeTmpAlloc, callocTmpAlloc, reallocTmpAlloc, NULL, NULL, "TmpAlloc" };
