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

	size_t nThreads = 1;

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
