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
	Module that counts application allocations
*/
#include <mprofLogMmap.h>
#include <mprofRecord.h>
#include <semaphore.h>
#include <assert.h>
#include <ParseEnv.h>
#include <stdlib.h>
#include <TmpAlloc.h>
#include <string.h>

//in terms of records
static size_t cacheQty = 10;
static size_t growthQty = 1000;
static struct mmapArea area = MMAP_AREA_NULL;
volatile size_t areaIndex = 0;
sem_t mmapSem;

__thread size_t cacheRemaining = 1;
__thread struct MprofRecordAlloc * cache = NULL;


static void populateCache( void ) {
	size_t index = __sync_fetch_and_add( &areaIndex, cacheQty );
	size_t requiredSize = ( index + cacheQty ) * sizeof( struct MprofRecordAlloc );
	
	if( requiredSize > area.fileSize ) {
		sem_wait( &mmapSem );
		if( requiredSize > area.fileSize ) {
			assert( mmapSize( &area, requiredSize + growthQty * sizeof( struct MprofRecordAlloc ), MMAP_AREA_SET ) );
		}
		sem_post( &mmapSem );
	}
	cache = ((struct MprofRecordAlloc *) area.base ) + index;
	cacheRemaining = cacheQty;
}

static void mprofLogMmapConstruct( void ) {
	size_t argSize;
	const char * arg = findArg( getConfStr(), "LOG_QTY", &argSize );
	if( arg ) {
		growthQty = strtol( arg, NULL, 10 );
	}

	arg = findArg( getConfStr(), "CACHE_QTY", &argSize );
	if( arg ) {
		cacheQty = strtol( arg, NULL, 10 );
	}

	arg = findArg( getConfStr(), "LOG_PATH", &argSize );
	if( arg ) {
		char * tmp = tmpAllocVtable.calloc( argSize + 1, sizeof( char ) );
		strncpy( tmp, arg, argSize );
		arg = tmp;
	} else {
		arg = "./mprof.log";
	}
	assert( mmapOpen( &area, arg, O_RDWR | O_TRUNC | O_CREAT ) );
	assert( mmapSize( &area, growthQty * sizeof( struct MprofRecordAlloc ), MMAP_AREA_SET ) );
	sem_init( &mmapSem, 0, 1 );
}

static void mprofLogMmapDestruct( void ) {
	sem_destroy( &mmapSem );
	mmapClose( &area );
}

static void * mallocLogMmap( size_t in_size ) {
	void * ret = defaultVtable.malloc( in_size );
	if( --cacheRemaining == 0 ) {
		populateCache();
	}
	cache->type.malloc.size = in_size;
	cache->type.malloc.result = (size_t) ret;
	cache->header.mode = MPROF_MODE_MALLOC;
	mprofRecordTimeStamp( cache++ );
	return ret;
}

static void freeLogMmap( void * in_ptr ) {
	defaultVtable.free( in_ptr );
	if( --cacheRemaining == 0 ) {
		populateCache();
	}
	cache->type.free.pointer = (size_t) in_ptr;
	cache->header.mode = MPROF_MODE_FREE;
	mprofRecordTimeStamp( cache++ );
}

static void * callocLogMmap( size_t in_qty, size_t in_size ) {
	void * ret = defaultVtable.calloc( in_qty, in_size );
	if( --cacheRemaining == 0 ) {
		populateCache();
	}
	cache->type.calloc.size = in_size;
	cache->type.calloc.count = in_qty;
	cache->type.calloc.result = (size_t) ret;
	cache->header.mode = MPROF_MODE_CALLOC;
	mprofRecordTimeStamp( cache++ );
	return ret;
}

static void * reallocLogMmap( void * in_ptr, size_t in_size ) {
	void * ret = defaultVtable.realloc( in_ptr, in_size );
	if( --cacheRemaining == 0 ) {
		populateCache();
	}
	cache->type.realloc.size = in_size;
	cache->type.realloc.pointer = (size_t) in_ptr;
	cache->type.realloc.result = (size_t) ret;
	cache->header.mode = MPROF_MODE_REALLOC;
	mprofRecordTimeStamp( cache++ );
	return ret;
}

const struct AllocatorVtable mprofLogMmapVtable = { &mallocLogMmap, &freeLogMmap, &callocLogMmap, &reallocLogMmap, &mprofLogMmapConstruct, &mprofLogMmapDestruct, "LogMmap" };
