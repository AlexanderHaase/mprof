/*
	Copyright (c) 2013 by Alexander Haase ( https://www.alexanderhaase.net )
	Project: ahmalloc ( https://www.alexanderhaase.net/projects/ahmalloc.html )
	Created: 2013-09-08
	License: LGPL

	Record format for binary per-allocation tracking.
*/
#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define MPROF_MODE_EMPTY	( 0u )
#define MPROF_MODE_MALLOC	( 1u )
#define MPROF_MODE_FREE		( 2u )
#define MPROF_MODE_REALLOC	( 3u )
#define MPROF_MODE_CALLOC	( 4u )

struct MProfRecordMalloc {
	uint64_t size;
	uint64_t result;
};

struct MProfRecordFree {
	uint64_t pointer;
};

struct MProfRecordRealloc {
	uint64_t size;
	uint64_t pointer;
	uint64_t result;
};

struct MProfRecordCalloc {
	uint64_t size;
	uint64_t count;
	uint64_t result;
};

struct MprofRecord {
	union mprofRecord_ignored {
		struct MProfRecordMalloc malloc;
		struct MProfRecordFree free;
		struct MProfRecordRealloc realloc;
		struct MProfRecordCalloc calloc;
	} type;
	uint64_t sec;
	uint32_t usec;
	uint16_t thread;
	uint8_t mode;
	uint8_t padding[ 1 ];
} ; //__attribute__ (( packed ));


/*!	Sets the record time stamp to now.

	\param[ in, out ]	record	The record to timestamp
*/
void mprofRecordTimeStamp( struct MprofRecord * in_out_record );

struct mmapArea {
	int fd;
	size_t fileSize;
	void * base;
};

#define MMAP_AREA_NULL { 0, 0, NULL }
#define MMAP_AREA_SET ( 0 )
#define MMAP_AREA_GROW ( 1 )
#define MMAP_AREA_SHRINK ( 2 )

bool mmapOpen( struct mmapArea * out_area, const char * in_path, bool in_trunc );
bool mmapSize( struct mmapArea * in_out_area, const size_t in_size, const int in_mode );
void mmapClose( struct mmapArea * in_out_area );
