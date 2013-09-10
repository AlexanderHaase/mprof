/*
	Copyright (c) 2013 by Alexander Haase ( https://www.alexanderhaase.net )
	Project: ahmalloc ( https://www.alexanderhaase.net/projects/ahmalloc.html )
	Created: 2013-09-08
	License: LGPL

	Record format for binary per-allocation tracking.
*/
#pragma once
#include <stdint.h>

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

struct MProfRecord {
	union {
		struct MProfRecordMalloc malloc;
		struct MProfRecordFree free;
		struct MProfRecordRealloc realloc;
		struct MProfRecordCalloc calloc;
	};
	uint64_t sec;
	uint32_t msec;
	uint8_t mode;
	uint8_t padding[ 3 ];
} __attribute__(( __packed__ ));


/*!	Sets the record time stamp to now.

	\param[ in, out ]	record	The record to timestamp
*/
void mprofRecordTimeStamp( struct MprofRecord * in_out_record );
