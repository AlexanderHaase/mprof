/*
	Copyright (c) 2013 by Alexander Haase ( https://www.alexanderhaase.net )
	Project: mprof ( https://www.alexanderhaase.net/projects/mprof.html )
	Created: 2013-09-12
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
	Functionality for the mprof profiler program.

	-dump binary formats to text.
	-set up preloading
*/
#define _GNU_SOURCE
#include <mprofRecord.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <mprofProfile.h>

void mprofCountsPrint( FILE * out_stream, const struct MprofRecordCount * in_counts ) {
	unsigned long long transactions =
		(unsigned long long) in_counts->malloc +
		(unsigned long long) in_counts->free +
		(unsigned long long) in_counts->calloc + 
		(unsigned long long) in_counts->realloc; 
	fprintf( out_stream, "Thread:\t%llu\tmalloc:\t%llu\tfree:\t%llu\tcalloc:\t%llu\trealloc:\t%lluTotal:\t%llu\n",
		(unsigned long long) in_counts->thread,
		(unsigned long long) in_counts->malloc,
		(unsigned long long) in_counts->free,
		(unsigned long long) in_counts->calloc, 
		(unsigned long long) in_counts->realloc,
		transactions );
}


size_t mprofCountsTotal( const struct MprofRecordCount * in_counts, const size_t in_size, struct MprofRecordCount * out_total ) {
	size_t ret = 0;
	for( size_t index = 0; index < in_size; index++ ) {
		ret += in_counts[ index ].malloc;
		ret += in_counts[ index ].free;
		ret += in_counts[ index ].calloc;
		ret += in_counts[ index ].realloc;
	}

	if( out_total ) {
		for( size_t index = 0; index < in_size; index++ ) {
			out_total->malloc += in_counts[ index ].malloc;
			out_total->free += in_counts[ index ].free;
			out_total->calloc += in_counts[ index ].calloc;
			out_total->realloc += in_counts[ index ].realloc;
		}
	}

	return ret;
}

bool mprofCountsDumpFile( FILE * out_stream, const char * in_path ) {
	struct mmapArea area;
	
	bool ret = false;
	
	do {
		if( ! mmapOpen( &area, in_path, O_RDONLY ) ) {
			break;
		}
		if( ! NONZEROMULTIPLE( area.fileSize, sizeof( struct MprofRecordCount ) ) ) {
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

bool mprofTestProgram( const char * in_cmdArg, const char * in_countsPath, const char * in_logPath ) {
	
	struct mmapArea area;
	
	bool ret = false;
	char * mprofConfig = NULL;
	
	do {
		asprintf( &mprofConfig, "INIT=LD_NEXT MODE=Count COUNTS_PATH='%s'", in_countsPath );
		if( mprofConfig == NULL ) {
			break;
		}
		if( setenv( "LD_PRELOAD", "./libmprof.so", true ) ) {
			perror( "mprof: set 'LD_PRELOAD'" );
			break;
		}
		if( setenv( "MPROF_CONF", mprofConfig, true ) ) {
			perror( "mprof: set 'MPROF_CONF'" );
			break;
		}
		if( system( in_cmdArg ) ) {
			fprintf( stderr, "mprof: Warning, non-zero result for command '%s'\n", in_cmdArg );
		}

		if( ! mmapOpen( &area, in_countsPath, O_RDONLY ) ) {
			break;
		}

		if( ! NONZEROMULTIPLE( area.fileSize, sizeof( struct MprofRecordCount ) ) ) {
			break;
		}

		const size_t nThreads = area.fileSize / sizeof( struct MprofRecordCount );

		const struct MprofRecordCount * counts = (struct MprofRecordCount *) area.base;
	
		size_t index;
		for( index = 0; index < nThreads; ++index ) {
			if( counts->header.mode != MPROF_MODE_COUNTS ) {
				break;	
			}
		}
		if( index < nThreads ) {
			break;
		}

		size_t cacheQty = 100;
		size_t totalAllocs = mprofCountsTotal( counts, nThreads, NULL );
		totalAllocs += cacheQty * nThreads;

		mmapClose( &area );

		free( mprofConfig );
		asprintf( &mprofConfig, "INIT=LD_NEXT MODE=LogMmap LOG_QTY=%llu CACHE_QTY=%llu LOG_PATH='%s'", UNSIGNEDPAD( totalAllocs ), UNSIGNEDPAD( cacheQty ), in_logPath );
		if( NULL == mprofConfig ) {
			break;
		}

		if( setenv( "MPROF_CONF", mprofConfig, true ) ) {
			perror( "mprof: set 'MPROF_CONF'" );
			break;
		}

		if( system( in_cmdArg ) ) {
			fprintf( stderr, "mprof: Warning, non-zero result for command '%s'\n", in_cmdArg );
		}

		if( ! mmapOpen( &area, in_logPath, O_RDONLY ) ) {
			break;
		}

		if( ! NONZEROMULTIPLE( area.fileSize, sizeof( struct MprofRecordCount ) ) ) {
			break;
		}

		const size_t recordQty = area.fileSize / sizeof( struct MprofRecordCount );
		mprofSizeAnalysis( stdout, (struct MprofRecordAlloc *) area.base, recordQty );
		mprofCompactnessAnalysis( stdout, (struct MprofRecordAlloc *) area.base, recordQty );
		
	} while( false );

	free( mprofConfig );
	mmapClose( &area );
	
	return ret;
}

#define MODE_DUMP_COUNTS	(0u)
#define MODE_HELP		(1u)
#define MODE_TEST		(2u)
#define MODE_DUMP_LOG		(3u)

struct MprofMainModes {
	char * modeName;
	size_t modeValue;
};

struct MprofMainModes modes[] = { 
					{ "DUMP", MODE_DUMP_COUNTS },	
					{ "TEST", MODE_TEST },
					{ "HELP", MODE_HELP } 
				};

int mprofMain( __attribute__(( unused )) int argc, __attribute__(( unused )) char ** argv ) {

	size_t mode = MODE_DUMP_COUNTS;
	const char * testArg = NULL;
	const char * countsArg = "mprof.counts";
	const char * logArg = "mprof.log";
	int c;

	while( ( c = getopt( argc, argv, "hm:t:c:l:" ) ) != -1 ) {
		switch( c ) {
		case ('t'):
			mode = MODE_TEST;
			testArg = optarg;
			break;
		case ('h'):
			mode = MODE_HELP;
			break;
		case ('c'):
			countsArg = optarg;
			break;
		case ('l'):
			logArg = optarg;
			break;
		case ('m'):
			for( size_t index = 0;; ++index ) {
				if( modes[ index ].modeName == NULL ) {
					fprintf( stderr, "Unknown mode '%s'\n", optarg );
					return -1;
				}
				if( strcmp( optarg, modes[ index ].modeName ) == 0 ) {
					mode = modes[ index ].modeValue;
					break;
				}
			}
			break;
		case ('?'):
			switch( optopt ) {
			case ('t'):
			case ('c'):
			case ('m'):
				fprintf( stderr, "-%c requires and argument\n", optopt );
				break;
			default:
				fprintf( stderr, "Unknown option '-%c'.\n", optopt );
			}
			return -1;
		default:
			abort();
		}
	}
	
	switch( mode ) {
	case ( MODE_DUMP_COUNTS ):
		return ( mprofCountsDumpFile( stdout, countsArg ) ? 0 : -1 );
	case ( MODE_TEST ):
		return ( mprofTestProgram( testArg, countsArg, logArg ) ? 0 : -1 );
	case ( MODE_HELP ):
		fprintf( stderr, "I'm a jerk, no help yet\n" );
		return 0;
	default:
		abort();
	}
}
