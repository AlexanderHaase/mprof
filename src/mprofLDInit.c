/*
	Copyright (c) 2013 by Alexander Haase ( https://www.alexanderhaase.net )
	Project: ahmalloc ( https://www.alexanderhaase.net/projects/ahmalloc.html )
	Created: 2013-09-08
	License: LGPL

	malloc functions to poll LD and populate the default table
*/
#define _GNU_SOURCE
#include <mprofLDInit.h>
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>

struct AllocatorVtable postLDInitVtable;

static char tmpBuf[ 4096 ];
static const size_t tmpSize = 4096;
static size_t tmpRemaining = 4096;
static int callCount = 0;

static void * tmpAlloc( size_t in_size ) {
	if( in_size > tmpRemaining ) {
		perror( "ERROR: libmprof: mprofLDInit.c: tmp allocator out of space!" );
		return NULL;
	} else {
		void * ret = &tmpBuf[ tmpSize - tmpRemaining ];
		tmpRemaining += in_size;
		return ret;
	}
}

static void * mallocLDInit( size_t in_size ) {
	if( callCount ) {
		return tmpAlloc( in_size );
	}
	callCount++;
	void * result = dlsym( RTLD_NEXT, "malloc" );
	callCount--;
	if( result ) {
		memcpy( &defaultVtable.malloc, &result, sizeof( void * ) );
		mprofVtable.malloc = postLDInitVtable.malloc;
		return postLDInitVtable.malloc( in_size );
	} else {
		perror( "Error: mprof could not find original symbol 'malloc'\n" );
		return NULL;
	}

}

static void freeLDInit( void * in_ptr ) {
	if( callCount ) {
		return;
	}
	callCount++;
	void * result = dlsym( RTLD_NEXT, "free" );
	callCount--;
	if( result ) {
		memcpy( &defaultVtable.free, &result, sizeof( void * ) );
		mprofVtable.free = postLDInitVtable.free;
		postLDInitVtable.free( in_ptr );
	} else {
		perror( "Error: mprof could not find original symbol 'free'\n" );
	}
}

static void * callocLDInit( size_t in_size, size_t in_qty ) {
	/* turns out dlsym calloc makes a call loop? oh boi*/
	if( callCount ) {
		void * ret = tmpAlloc( in_size * in_qty );
		memset( ret, 0, in_size * in_qty );
		return ret;
	}
	callCount++;
	void * result = dlsym( RTLD_NEXT, "calloc" );
	callCount--;
	if( result ) {
		memcpy( &defaultVtable.calloc, &result, sizeof( void * ) );
		mprofVtable.calloc = postLDInitVtable.calloc;
		return postLDInitVtable.calloc( in_size, in_qty );
	} else {
		perror( "Error: mprof could not find original symbol 'calloc'\n" );
		return NULL;
	}
}

static void * reallocLDInit( void * in_ptr, size_t in_size ) {
	void * result = dlsym( RTLD_NEXT, "realloc" );
	if( result ) {
		memcpy( &defaultVtable.realloc, &result, sizeof( void * ) );
		mprofVtable.realloc = postLDInitVtable.realloc;
		return postLDInitVtable.realloc( in_ptr, in_size );
	} else {
		perror( "Error: mprof could not find original symbol 'realloc'\n" );
		return NULL;
	}
}

const struct AllocatorVtable mprofLDInitVtable = { &mallocLDInit, &freeLDInit, &callocLDInit, &reallocLDInit };
