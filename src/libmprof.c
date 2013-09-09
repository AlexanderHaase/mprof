/*
	Copyright (c) 2013 by Alexander Haase ( https://www.alexanderhaase.net )
	Project: ahmalloc ( https://www.alexanderhaase.net/projects/ahmalloc.html )
	Created: 2013-09-08
	License: LGPL

	base file for the shared library, sets up mprof bindings.
*/
#include <Personality.h>
#include <mprofLDInit.h>
#include <mprofLogFD.h>
#include <stdio.h>

static void __attribute__(( constructor )) libmprofInit( void ) {
	mprofVtable = mprofLDInitVtable;
	postLDInitVtable = mprofLogFDVtable;
}

void * malloc( size_t in_size ) {
	return mprofVtable.malloc( in_size );
}

void free( void * in_ptr ) {
	mprofVtable.free( in_ptr );
}

void * calloc( size_t in_qty, size_t in_size ) {
	return mprofVtable.calloc( in_qty, in_size );
}

void * realloc( void * in_ptr, size_t in_size ) {
	return mprofVtable.realloc( in_ptr, in_size );
}

/* we need to provide on-demand init functionality..construct order not dependable*/
static void * mallocLibInit( size_t in_size ) {
	libmprofInit();
	return mprofVtable.malloc( in_size );
}

static void freeLibInit( void * in_ptr ) {
	libmprofInit();
	mprofVtable.free( in_ptr );
}

static void * callocLibInit( size_t in_qty, size_t in_size ) {
	libmprofInit();
	return mprofVtable.calloc( in_qty, in_size );
}

static void * reallocLibInit( void * in_ptr, size_t in_size ) {
	libmprofInit();
	return mprofVtable.realloc( in_ptr, in_size );
}
	
struct AllocatorVtable mprofVtable = { mallocLibInit, freeLibInit, callocLibInit, reallocLibInit };
