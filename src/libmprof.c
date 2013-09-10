/*
	Copyright (c) 2013 by Alexander Haase ( https://www.alexanderhaase.net )
	Project: ahmalloc ( https://www.alexanderhaase.net/projects/ahmalloc.html )
	Created: 2013-09-08
	License: LGPL

	base file for the shared library, sets up mprof bindings.

	getEnv MPROF_CONF
	INIT->{ LD_NEXT, SO_LOAD }
	SO_LOAD->{ SO_NAME }
	MODE->{ logFD, mmapPrep, mmapRecord }
*/
#include <Personality.h>
#include <mprofLDInit.h>
#include <mprofLogFD.h>
#include <mprofCount.h>
#include <ParseEnv.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

const char * defaultConfig = "INIT=LD_NEXT MODE=LogFD";

const struct AllocatorVtable * mprofInitTargets[] = { &mprofLDInitVtable, NULL };
const struct AllocatorVtable * mprofModeTargets[] = { &mprofLogFDVtable, &mprofCountVtable, NULL };


const struct AllocatorVtable * lookupVtable( const char * in_str, size_t in_strSize,  const struct AllocatorVtable * in_tables[] ) {
	size_t index;
	for( index = 0; in_tables[ index ] != NULL; index++ ) {
		if( strncmp( in_tables[ index ]->name, in_str, in_strSize ) == 0 ) {
			return in_tables[ index ];
		}
	}
	return NULL;
}

void installVtableOrDie( const char * in_key, const struct AllocatorVtable * in_tables[], struct AllocatorVtable * out_table ) {
	const char * config = getConfStr();
	if( NULL == config ) {
		config = defaultConfig;
	}

	size_t valueSize;
	const char * value = findArg( config, in_key, &valueSize );
	if( NULL == value ) {
		perror( "Error: libmprof: Malformed or inadequte config in MPROF_CONF" );
		exit( -1 );
	}

	const struct AllocatorVtable * table = lookupVtable( value, valueSize, in_tables );
	if( NULL == table ) {
		perror( "Error: libmprof: cannot find specified vtable" );
		exit( -1 );
	}

	*out_table = *table;
	if( table->construct ) {
		table->construct();
	}
}

static void /*__attribute__(( constructor ))*/ libmprofInit( void ) {
	installVtableOrDie( "INIT", mprofInitTargets, &mprofVtable );
	installVtableOrDie( "MODE", mprofModeTargets, &postLDInitVtable );
}

void __attribute__(( destructor )) libmprofDestruct( void ) {
	if( mprofVtable.destruct ) {
		mprofVtable.destruct();
	}
	printf( "Libmprof destruct\n" );
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
	
struct AllocatorVtable mprofVtable = { mallocLibInit, freeLibInit, callocLibInit, reallocLibInit, NULL, NULL, "LIBMPROF_BOOTSTRAP" };
