/*
	Copyright (c) 2013 by Alexander Haase ( https://www.alexanderhaase.net )
	Project: mprof ( https://www.alexanderhaase.net/projects/mprof.html )
	Created: 2013-09-12
	License: LGPL

	Functionality for the mprof profiler program.

	-dump binary formats to text.
	-set up preloading
*/
#include <mprofRecord.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void mprofCountsPrint( FILE * out_stream, const struct MprofRecordCount * in_counts ) {
	fprintf( out_stream, "Thread:\t%llu\tmalloc:\t%llu\tfree:\t%llu\tcalloc:\t%llu\trealloc:\t%llu\n",
		(unsigned long long) in_counts->thread,
		(unsigned long long) in_counts->malloc,
		(unsigned long long) in_counts->free,
		(unsigned long long) in_counts->calloc, 
		(unsigned long long) in_counts->realloc );
}


void mprofCountsTotal( const struct MprofRecordCount * in_counts, const size_t in_size, struct MprofRecordCount * out_total ) {
	for( size_t index = 0; index < in_size; index++ ) {
		out_total->malloc += in_counts[ index ].malloc;
		out_total->free += in_counts[ index ].free;
		out_total->calloc += in_counts[ index ].calloc;
		out_total->realloc += in_counts[ index ].realloc;
	}
}

bool mprofCountsDumpFile( FILE * out_stream, const char * in_path ) {
	struct mmapArea area;
	
	bool ret = false;
	
	do {
		if( ! mmapOpen( &area, in_path, O_RDONLY ) ) {
			break;
		}
		if( area.fileSize == 0 || area.fileSize % sizeof( struct MprofRecordCount ) != 0 ) {
			break;
		}
		const size_t qty = area.fileSize / sizeof( struct MprofRecordCount );

		const struct MprofRecordCount * counts = (struct MprofRecordCount *) area.base;
	
		size_t index;
		for( index = 0; index < qty; ++index ) {
			if( counts->header.mode != MPROF_MODE_COUNTS ) {
				break;	
			}
		}
		if( index < qty ) {
			break;
		}

		struct MprofRecordCount total;
		memset( &total, 0, sizeof( struct MprofRecordCount ) );
		mprofCountsTotal( counts, qty, &total );
		total.thread = 9001; //FIXME

		for( index = 0; index < qty; ++index ) {
			mprofCountsPrint( out_stream, counts + index );
		}
		mprofCountsPrint( out_stream, &total );

		ret = true;
	} while( false );

	mmapClose( &area );

	return ret;
}

int mprofMain( __attribute__(( unused )) int argc, __attribute__(( unused )) char ** argv ) {
	return ( mprofCountsDumpFile( stdout, "mprof.counts" ) ? 0 : -1 );
}
