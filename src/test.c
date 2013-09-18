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
	A very dumb multithreaded test program.
*/
#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

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

#define COUNT 1000

void * threadFn( void * in_arg ) {
	
	struct timeval * end = ( struct timeval *) in_arg;

	struct timeval begin;
	gettimeofday( &begin, NULL );

	for( int i = 0; i < COUNT; i++ ) {
		int * ptr = malloc( sizeof( int ) );
		*ptr = i;
		free( ptr );
	}

	gettimeofday( end, NULL );

	while( begin.tv_usec > end->tv_usec ) {
		end->tv_usec += 1000000;
		end->tv_sec -= 1;
	}
	end->tv_sec -= begin.tv_sec;
	end->tv_usec -= begin.tv_usec;

	pthread_exit( NULL );
}

void * threadFn2( void * in_arg ) {
	
	struct timeval * end = ( struct timeval *) in_arg;

	struct timeval begin;
	struct timeval ignored0, ignored1;
	gettimeofday( &begin, NULL );

	for( int i = 0; i < COUNT; i++ ) {
		gettimeofday( &ignored0, NULL );
		int * ptr = malloc( sizeof( int ) );
		*ptr = i;
		free( ptr );
		gettimeofday( &ignored1, NULL );
	}

	gettimeofday( end, NULL );

	while( begin.tv_usec > end->tv_usec ) {
		end->tv_usec += COUNT;
		end->tv_sec -= 1;
	}
	end->tv_sec -= begin.tv_sec;
	end->tv_usec -= begin.tv_usec;

	pthread_exit( NULL );
}

void * threadFn3( void * in_arg ) {
	
	struct timeval * end = ( struct timeval *) in_arg;

	struct timeval begin;
	struct timespec ignored0, ignored1;
	gettimeofday( &begin, NULL );

	for( int i = 0; i < COUNT; i++ ) {
		clock_gettime( CLOCK_REALTIME_COARSE, &ignored0 );
		int * ptr = malloc( sizeof( int ) );
		*ptr = i;
		free( ptr );
		clock_gettime( CLOCK_REALTIME_COARSE, &ignored1 );
	}

	gettimeofday( end, NULL );

	while( begin.tv_usec > end->tv_usec ) {
		end->tv_usec += 1000000;
		end->tv_sec -= 1;
	}
	end->tv_sec -= begin.tv_sec;
	end->tv_usec -= begin.tv_usec;

	pthread_exit( NULL );
}

int main( void ) {

	size_t nThreads = 100;

	pthread_t * threads = calloc( sizeof( pthread_t ), nThreads );	
	
	struct timeval * times = calloc( sizeof( struct timeval ), nThreads );

	void * (*threadFns[ 3 ])( void * ) = { &threadFn, &threadFn2, &threadFn3 };

	for( size_t fn = 0; fn < 1; fn++ ) {	

		for( size_t index = 0; index < nThreads; index++ ) {
			pthread_create( &threads[ index ], NULL, threadFns[ fn ], times + index );
		}

		for( size_t index = 0; index < nThreads; index++ ) {
			pthread_join( threads[ index ], NULL );
		}

		struct timeval total = *times;

		for( size_t index = 1; index < nThreads; index++ ) {
			total.tv_sec += times[ index ].tv_sec;
			total.tv_usec += times[ index ].tv_usec;
		}

		total.tv_sec += total.tv_usec / 1000000;
		total.tv_usec = total.tv_usec % 1000000;
	
		//printf( "threads %llu, time: %llu.%06llu\n", (unsigned long long) nThreads, (unsigned long long) total.tv_sec, (unsigned long long) total.tv_usec );
	}
	
	free( threads );
	free( times );

	return 0;
}
