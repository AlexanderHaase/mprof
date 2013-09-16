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
#include <mprofLogFD.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>

static int logFD = 2;

static const size_t textBufSize = 1000;
static __thread char textBuf[ 1000 ];
static __thread bool externalCall = true;

int mprofLogFDSet( int in_fd ) {
	int ret = logFD;
	logFD = in_fd;
	return ret;
}


static void * mallocLogFD( size_t in_size ) {
	void * ret = defaultVtable.malloc( in_size );
	if( externalCall ) {
		externalCall = false;
		int msgSize = snprintf( textBuf, textBufSize, "malloc\t%llu\t%llX\n", (unsigned long long) in_size, (unsigned long long)(size_t) ret );
		write( logFD, textBuf, msgSize );
		externalCall = true;
	}
	return ret;
}

static void freeLogFD( void * in_ptr ) {
	defaultVtable.free( in_ptr );
	if( externalCall ) {
		externalCall = false;
		int msgSize = snprintf( textBuf, textBufSize, "free\t%llX\n", (unsigned long long)(size_t) in_ptr );
		write( logFD, textBuf, msgSize );
		externalCall = true;
	}
}


static void * callocLogFD( size_t in_qty, size_t in_size ) {
	void * ret = defaultVtable.calloc( in_qty, in_size );
	if( externalCall ) {
		externalCall = false;
		int msgSize = snprintf( textBuf, textBufSize, "calloc\t%llu\t%llu\t%llx\n", (unsigned long long) in_qty, (unsigned long long) in_size, (unsigned long long)(size_t) ret );
		write( logFD, textBuf, msgSize );
		externalCall = true;
	}
	return ret;
}


static void * reallocLogFD( void * in_ptr, size_t in_size ) {
	void * ret = defaultVtable.realloc( in_ptr, in_size );
	if( externalCall ) {
		externalCall = false;
		int msgSize = snprintf( textBuf, textBufSize, "realloc\t%llX\t%llu\t%llx\n", (unsigned long long)(size_t) in_ptr, (unsigned long long) in_size, (unsigned long long)(size_t) ret );
		write( logFD, textBuf, msgSize );
		externalCall = true;
	}
	return ret;
}

const struct AllocatorVtable mprofLogFDVtable = { mallocLogFD, freeLogFD, callocLogFD, reallocLogFD, NULL, NULL, "LogFD" };
