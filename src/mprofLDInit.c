/*
	Copyright (c) 2013 by Alexander Haase ( https://www.alexanderhaase.net )
	Project: mprof ( https://www.alexanderhaase.net/projects/mprof.html )
	Created: 2013-09-08
	License: LGPL

	mprof is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	mprof is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with mprof.  If not, see <http://www.gnu.org/licenses/>.
*/
/*	Description:
	malloc functions to poll LD and populate the default table
*/
#define _GNU_SOURCE
#include <mprofLDInit.h>
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <TmpAlloc.h>
#include <stdlib.h>
#include <stdint.h>
#include <ParseEnv.h>

static const char * ldNames[] = { "malloc", "free", "calloc", "realloc", NULL };
static void * ldHandle = RTLD_NEXT;

static void mprofLDInit( void ) {
	mprofVtable = tmpAllocVtable;

	/* look for and handle LD_PRELOAD argument in MPROF_CONF */
	size_t valueSize;
	const char * value = findArg( getConfStr(), "LD_PRELOAD", &valueSize );
	if( value ) {
		char * path = strndup( value, valueSize );
		ldHandle = dlopen( path, RTLD_LAZY | RTLD_GLOBAL | RTLD_DEEPBIND );
		if( NULL == ldHandle ) {
			fprintf( stderr,  "Error: libmprof: Could not open library '%s' specified for preload in env var 'MPROF_CONF'.\n", path );
			exit( -1 );
		}
		free( path );
	}

	uint8_t * fnPtr = (uint8_t*) &defaultVtable.malloc;
	for( size_t index = 0; ldNames[ index ] != NULL; index++ ) {
		void * result = dlsym( ldHandle, ldNames[ index ] );
		if( result ) {
			memcpy( &fnPtr[ index * sizeof( int(*)( void ) ) ], &result, sizeof( int(*)( void ) ) );
		} else {
			fprintf( stderr, "Error: libmprof: could not find original symbol '%s'\n", ldNames[ index ] );
			exit( -1 );
		}
	}
	mprofVtable = postInitVtable;
}
	/*result = dlsym( RTLD_NEXT, "free" );
	if( result ) {
		memcpy( &defaultVtable.free, &result, sizeof( void * ) );
	} else {
		perror( "Error: mprof could not find original symbol 'free'\n" );
	}

	void * result = dlsym( RTLD_NEXT, "calloc" );
	callCount--;
	if( result ) {
		memcpy( &defaultVtable.calloc, &result, sizeof( void * ) );
	} else {
		perror( "Error: mprof could not find original symbol 'calloc'\n" );
		exit( -1 );
	}*/
	

static void * mallocLDInit( __attribute__((unused)) size_t in_size ) {
	perror( "Error: libmprof: race condition detected during LD init! Are you initializing in a multithreaded environment?" );
	exit( -1 );
	return NULL;
}

static void freeLDInit( __attribute__((unused)) void * in_ptr ) {
	perror( "Error: libmprof: race condition detected during LD init! Are you initializing in a multithreaded environment?" );
	exit( -1 );
}

static void * callocLDInit( __attribute__((unused)) size_t in_size, __attribute__((unused)) size_t in_qty ) {
	perror( "Error: libmprof: race condition detected during LD init! Are you initializing in a multithreaded environment?" );
	exit( -1 );
	return NULL;
}

static void * reallocLDInit( __attribute__((unused)) void * in_ptr, __attribute__((unused)) size_t in_size ) {
	perror( "Error: libmprof: race condition detected during LD init! Are you initializing in a multithreaded environment?" );
	exit( -1 );
	return NULL;
}

const struct AllocatorVtable mprofLDInitVtable = { &mallocLDInit, &freeLDInit, &callocLDInit, &reallocLDInit, &mprofLDInit, NULL, "LD_NEXT" };
