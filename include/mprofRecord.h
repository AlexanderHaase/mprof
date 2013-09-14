/*
	Copyright (c) 2013 by Alexander Haase ( https://www.alexanderhaase.net )
	Project: ahmalloc ( https://www.alexanderhaase.net/projects/ahmalloc.html )
	Created: 2013-09-08
	License: LGPL

	Record format for binary per-allocation tracking. Records should be
	equally sized.
*/
#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <fcntl.h>

#define NONZEROMULTIPLE( _value_, _base_ ) \
	( (_value_) != 0 && ( _value_ ) % ( _base_ ) == 0 )

#define UNSIGNEDPAD( _value_ )	\
	( (unsigned long long) ( _value_ ) )

#define MPROF_MODE_EMPTY	( 0u )
#define MPROF_MODE_MALLOC	( 1u )
#define MPROF_MODE_FREE		( 2u )
#define MPROF_MODE_REALLOC	( 3u )
#define MPROF_MODE_CALLOC	( 4u )
#define MPROF_MODE_COUNTS	( 5u )

struct MprofRecordHeader {
	uint8_t mode;
	uint8_t flags;
} __attribute__ (( packed ));

struct MprofRecordMalloc {
	uint64_t size;
	uint64_t result;
} __attribute__ (( packed ));

struct MprofRecordFree {
	uint64_t pointer;
} __attribute__ (( packed ));

struct MprofRecordRealloc {
	uint64_t size;
	uint64_t pointer;
	uint64_t result;
} __attribute__ (( packed ));

struct MprofRecordCalloc {
	uint64_t size;
	uint64_t count;
	uint64_t result;
} __attribute__ (( packed ));

struct MprofRecordAlloc {
	struct MprofRecordHeader header;
	uint16_t thread;
	uint64_t sec;
	uint32_t usec;
	union {
		struct MprofRecordMalloc malloc;
		struct MprofRecordFree free;
		struct MprofRecordRealloc realloc;
		struct MprofRecordCalloc calloc;
	} type;
} __attribute__ (( packed ));

struct MprofRecordEmpty {
	struct MprofRecordHeader header;
	uint8_t padding[ sizeof( struct MprofRecordAlloc ) - sizeof( struct MprofRecordHeader ) ];
} __attribute__ (( packed ));

struct MprofRecordCount {
	struct MprofRecordHeader header;
	uint16_t thread;
	uint16_t padding[ 2 ];
	uint64_t malloc;
	uint64_t free;
	uint64_t calloc;
	uint64_t realloc;
} __attribute__ (( packed ));


/*!	Sets the record time stamp to now.

	\param[ in, out ]	record	The record to timestamp
*/
void mprofRecordTimeStamp( struct MprofRecordAlloc * in_out_record );


/*!	Initializes the recording interface
	Run time sanity checks at this point
*/
void mprofRecordInit( void );

struct mmapArea {
	int fd;
	size_t fileSize;
	void * base;
};

#define MMAP_AREA_NULL { 0, 0, NULL }
#define MMAP_AREA_SET ( 0 )
#define MMAP_AREA_GROW ( 1 )
#define MMAP_AREA_SHRINK ( 2 )

bool mmapOpen( struct mmapArea * out_area, const char * in_path, int in_openFlags );
bool mmapSize( struct mmapArea * in_out_area, const size_t in_size, const int in_mode );
void mmapClose( struct mmapArea * in_out_area );
