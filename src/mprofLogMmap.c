/*
	Copyright (c) 2013 by Alexander Haase ( https://www.alexanderhaase.net )
	Project: ahmalloc ( https://www.alexanderhaase.net/projects/ahmalloc.html )
	Created: 2013-09-10
	License: LGPL

	Module that counts application allocations
*/
#include <mprofLogMmap.h>
#include <mprofRecord.h>
#include <semaphore.h>
#include <assert.h>

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
	assert( mmapOpen( &area, "mprof.log", O_RDWR | O_TRUNC | O_CREAT ) );
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
	cache->type.malloc.result = (uint64_t) ret;
	cache->header.mode = MPROF_MODE_MALLOC;
	mprofRecordTimeStamp( cache++ );
	return ret;
}

static void freeLogMmap( void * in_ptr ) {
	defaultVtable.free( in_ptr );
	if( --cacheRemaining == 0 ) {
		populateCache();
	}
	cache->type.free.pointer = (uint64_t) in_ptr;
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
	cache->type.calloc.result = (uint64_t) ret;
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
	cache->type.realloc.pointer = (uint64_t) in_ptr;
	cache->type.realloc.result = (uint64_t) ret;
	cache->header.mode = MPROF_MODE_REALLOC;
	mprofRecordTimeStamp( cache++ );
	return ret;
}

const struct AllocatorVtable mprofLogMmapVtable = { &mallocLogMmap, &freeLogMmap, &callocLogMmap, &reallocLogMmap, &mprofLogMmapConstruct, &mprofLogMmapDestruct, "LogMmap" };
