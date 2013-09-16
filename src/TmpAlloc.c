/*
	Copyright (c) 2013 by Alexander Haase ( https://www.alexanderhaase.net )
	Project: mprof ( https://www.alexanderhaase.net/projects/mprof.html )
	Created: 2013-09-10
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
