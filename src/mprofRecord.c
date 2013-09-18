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
	Record format for binary per-allocation tracking. Records should be
	equally sized.
*/
#define _GNU_SOURCE
#include <mprofRecord.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <sys/time.h>

/*
#include <time.h>
//#include <linux/time.h>
#define CLOCK_REALTIME			0
#define CLOCK_MONOTONIC			1
#define CLOCK_PROCESS_CPUTIME_ID	2
#define CLOCK_THREAD_CPUTIME_ID		3
#define CLOCK_MONOTONIC_RAW		4
#define CLOCK_REALTIME_COARSE		5
#define CLOCK_MONOTONIC_COARSE		6
#define CLOCK_BOOTTIME			7
#define CLOCK_REALTIME_ALARM		8
#define CLOCK_BOOTTIME_ALARM		9
*/

#define HAS_FLAG( _value_, _flag_ ) \
	( ( ( _value_ ) & ( _flag_ ) ) == ( _flag_ ) )

void mprofRecordTimeStamp( struct MprofRecordAlloc * in_out_record ) {
	struct timeval tv;
	gettimeofday( &tv, NULL );
	in_out_record->sec = tv.tv_sec;
	in_out_record->usec = tv.tv_usec;
	/* faster, but causes problems....
	struct timespec tv;
	clock_gettime( CLOCK_REALTIME_COARSE, &tv );
	in_out_record->sec = tv.tv_sec;
	in_out_record->usec = tv.tv_nsec;*/
}

void mprofRecordInit( void ) {
	assert( sizeof( struct MprofRecordEmpty ) == sizeof( struct MprofRecordCount ) );
	assert( sizeof( struct MprofRecordAlloc ) == sizeof( struct MprofRecordCount ) );
}

bool mmapOpen( struct mmapArea * out_area, const char * in_path, int in_openFlags ) {
	bool ret = false;

	memset( out_area, 0, sizeof( struct mmapArea ) );

	do {
		out_area->fd = open( in_path, in_openFlags, S_IRUSR | S_IWUSR );
	
		if( out_area->fd < 0 ) {
			perror("mmapOpen: ");
			break;
		}

		struct stat st;
		if( fstat( out_area->fd, &st ) ) {
			break;
		}

		out_area->fileSize = st.st_size;

		int mmapProt = 0;
		if( HAS_FLAG( in_openFlags, O_RDWR ) ) {
			mmapProt |= PROT_READ | PROT_WRITE;
		} else if( HAS_FLAG( in_openFlags, O_RDONLY ) ) {
			mmapProt |= PROT_READ;
		} else if( HAS_FLAG( in_openFlags, O_WRONLY ) ) {
			mmapProt |= PROT_WRITE;
		} else {
			fprintf( stderr, "mmapOpen: couldn't determine access mode for mapping\n" );
			break;
		}

		if( out_area->fileSize ) {
			out_area->base = mmap( NULL, out_area->fileSize, mmapProt, MAP_SHARED, out_area->fd, 0 );
			if( out_area->base == (void*) -1 ) {
				out_area->base = NULL;
				break;
			}
		}
		ret = true;
	} while( false );

	if( false == ret ) {
		if( out_area->base ) {
			munmap( out_area->base, out_area->fileSize );
		}
		close( out_area->fd );
		out_area->fileSize = 0;
	}
	return ret;
}


bool mmapSize( struct mmapArea * in_out_area, const size_t in_size, const int in_mode ) {
	bool ret = false;

	do {
		size_t finalSize = 0;
		switch( in_mode ) {
		case ( MMAP_AREA_SET ):
			finalSize = in_size;
			ret = true;
			break;
		case ( MMAP_AREA_GROW ):
			finalSize = in_size + in_out_area->fileSize;
			break;
		case ( MMAP_AREA_SHRINK ):
			if( in_size > in_out_area->fileSize ) {
				break;
			}
			finalSize = in_out_area->fileSize - in_size;
			ret = true;
		default:
			break;
		}
		if( ! ret ) {
			break;
		} else {
			ret = false;
		}

		/*if( finalSize ) {
			finalSize = ( ( finalSize - 1 )/( 4096 * 1024 ) + 1 ) * (4096 * 1024 );
		}*/

		if( ftruncate( in_out_area->fd, finalSize ) ) {
			perror( "mmapSize: ftruncate:" );
			break;
		}

		if( in_out_area->base ) {
			void * result = mremap( in_out_area->base, in_out_area->fileSize, finalSize, 0 );
			if( in_out_area->base != result ) {
				perror( "mmap:" );
				break;
			}
		} else {
			in_out_area->base = mmap( NULL, finalSize, PROT_READ | PROT_WRITE, /*MAP_HUGETLB |*/ MAP_SHARED, in_out_area->fd, 0 );
			if( (void*) -1 == in_out_area->base ) {
				perror( "mmap:" );
				in_out_area->base = NULL;
				break;
			}
		}

		in_out_area->fileSize = finalSize;
		ret = true;
	} while( false );

	return ret;
}


void mmapClose( struct mmapArea * in_out_area ) {
	close( in_out_area->fd );
	munmap( in_out_area->base, in_out_area->fileSize );
	in_out_area->fileSize = 0;
}
