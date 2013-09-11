/*
	Copyright (c) 2013 by Alexander Haase ( https://www.alexanderhaase.net )
	Project: mprof ( https://www.alexanderhaase.net/projects/mprof.html )
	Created: 2013-09-08
	License: LGPL

	breif: benchmark for various calling latencies.

	not-breif: Is it better to branch or function pointer? We need to
	support multiple behaviors in allocators like initernal initialization,
	external initialization, etc. Each behaviour/personality/branch can
	be expressed multiple ways. If we know ahead of time a particular 
	branch is the correct option, we can express it as a function pointer
	rather than a conditional. 

	However the implicit hypothesis of ( call faster than branch ) should
	be evaluated, especially in light of branch prediction being far more
	prevelant and mature than call target prediction.

	May end up being a dumb test, but that's never stopped me before.

	proceedure: lets time a large number of calls via branch, call, and
	direct. we can even make them real calls for reality sake. Deltas will
	hopefully be telling.
*/
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

bool getDuration( const struct timeval * in_begin, const struct timeval * in_end, struct timeval * out_result ) {
	for( *out_result = *in_end; in_begin->tv_usec > out_result->tv_usec; ) {
		out_result->tv_usec += 1000000;
		out_result->tv_sec -= 1;
	}
	if( in_begin->tv_sec > out_result->tv_sec ) {
		return false;
	} else {
		out_result->tv_sec -= in_begin->tv_sec;
		out_result->tv_usec -= in_begin->tv_usec;
		return true;
	}
}

int main( int argc, char ** argv ) {

	if( argc != 2 ) {
		fprintf( stderr, "Too many parameters %d--Takes only iterations.\n", argc - 1 );
		return -1;
	}

	const size_t iterations = strtoll( argv[ 1 ], NULL, 10 );

	enum Tests {
		BRANCH,
		JUMP,
		CONTROL,
		NTESTS
	};

	struct timeval begin;
	struct timeval end;
	struct timeval results[ NTESTS ];

	/* branch test */
	size_t * branchCondition = NULL;
	void * tmpPtr;

	gettimeofday( &begin, NULL );

	for( size_t itr = 0; itr < iterations; itr++ ) {
		if( branchCondition == NULL ) {
			branchCondition = malloc( sizeof( size_t ) );
		}
		tmpPtr = malloc( sizeof( size_t ) );
		free( tmpPtr );
	}

	free( branchCondition );
	
	gettimeofday( &end, NULL );

	getDuration( &begin, &end, &results[ BRANCH ] );

	/* jump test */
	void * (*mallocPtr)( size_t ) = &malloc;
	void ( * freePtr )( void * ) = &free;

	gettimeofday( &begin, NULL );

	for( size_t itr = 0; itr < iterations; itr++ ) {
		tmpPtr = mallocPtr( sizeof( size_t ) );
		freePtr( tmpPtr );
	}

	gettimeofday( &end, NULL );

	getDuration( &begin, &end, &results[ JUMP ] );

	/* direct test */
	gettimeofday( &begin, NULL );

	for( size_t itr = 0; itr < iterations; itr++ ) {
		tmpPtr = malloc( sizeof( size_t ) );
		free( tmpPtr );
	}

	gettimeofday( &end, NULL );

	getDuration( &begin, &end, &results[ CONTROL ] );

	const char * testNames[ NTESTS ] = { "Branch", "Jump", "Control" };

	/* results */
	for( size_t index = 0; index < NTESTS; index++ ) {
		printf( "Test '%s': %llu.%05llu sec for %llu iterations\n", 
			testNames[ index ],
			(unsigned long long) results[ index ].tv_sec, 
			(unsigned long long) results[ index ].tv_usec, 
			(unsigned long long) iterations );
	}

	return 0;
}

