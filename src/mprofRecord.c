/*
	Copyright (c) 2013 by Alexander Haase ( https://www.alexanderhaase.net )
	Project: ahmalloc ( https://www.alexanderhaase.net/projects/ahmalloc.html )
	Created: 2013-09-08
	License: LGPL

	Record format for binary per-allocation tracking.
*/
#define _GNU_SOURCE
#include <mprofRecord.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

void mprofRecordTimeStamp( struct MprofRecord * in_out_record ) {
	struct timeval tv;
	gettimeofday( &tv, NULL );
	in_out_record->sec = tv.tv_sec;
	in_out_record->usec = tv.tv_usec;
}


bool mmapOpen( struct mmapArea * out_area, const char * in_path, bool in_trunc ) {
	bool ret = false;

	memset( out_area, 0, sizeof( struct mmapArea ) );

	do {
		out_area->fd = open( in_path, ( in_trunc ? O_RDWR | O_TRUNC | O_CREAT : O_RDWR | O_CREAT ), S_IRUSR | S_IWUSR );
	
		if( out_area->fd < 0 ) {
			perror("mmapOpen: ");
			break;
		}

		struct stat st;
		if( fstat( out_area->fd, &st ) ) {
			break;
		}

		out_area->fileSize = st.st_blocks * 512;

		if( out_area->fileSize ) {
			out_area->base = mmap( NULL, out_area->fileSize, PROT_READ | PROT_WRITE, MAP_HUGETLB | MAP_SHARED, out_area->fd, 0 );
			if( out_area->base == NULL ) {
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
