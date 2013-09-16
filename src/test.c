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
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>

void * threadFn( __attribute__ ((unused)) void * in_arg ) {
	for( int i = 0; i < 1000; i++ ) {
		int * ptr = malloc( sizeof( int ) );
		*ptr = i;
		free( ptr );
	}

	pthread_exit( NULL );
}

int main( void ) {

	size_t nThreads = 100;

	pthread_t * threads = calloc( sizeof( pthread_t ), nThreads );	
	
	for( size_t index = 0; index < nThreads; index++ ) {
		pthread_create( &threads[ index ], NULL, &threadFn, NULL );
	}

	for( size_t index = 0; index < nThreads; index++ ) {
		pthread_join( threads[ index ], NULL );
	}

	free( threads );

	return 0;
}
