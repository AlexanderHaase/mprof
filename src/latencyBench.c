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
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <stdbool.h>


size_t counter;

void * doSomething( size_t val ) {
	counter += val;
	return ( void* ) (counter & 1ul);
}

void doSomethingElse( void * ptr ) {
	counter = (size_t)ptr;
}

#define TEST_DO doSomething
#define TEST_UNDO doSomethingElse

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

	size_t requestNumber = 1000000000;
	int c;	

	while( (c = getopt( argc, argv, "i:" )) != -1 ) {
		switch( c ) {
		case ('i'):
			requestNumber = strtoll( optarg, NULL, 10 );
			break;
		case ('?'):
			if( optopt == 'i' ) {
				fprintf( stderr, "-i requires an iteration argument\n" );
			} else {
				fprintf( stderr, "Unknown option '-%c'.\n", optopt );
			}
			return -1;
		default:
			abort();
		}
	}

	/*if( argc != 2 ) {
		fprintf( stderr, "Too many parameters %d--Takes only iterations.\n", argc - 1 );
		return -1;
	}*/

	const size_t iterations = requestNumber; //strtoll( argv[ 1 ], NULL, 10 );

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
			branchCondition = TEST_DO( sizeof( size_t ) );
		}
		tmpPtr = TEST_DO( sizeof( size_t ) );
		TEST_UNDO( tmpPtr );
	}

	gettimeofday( &end, NULL );

	TEST_UNDO( branchCondition );	

	getDuration( &begin, &end, &results[ BRANCH ] );

	/* jump test */
	void * (*mallocPtr)( size_t ) = &TEST_DO;
	void ( * freePtr )( void * ) = &TEST_UNDO;

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
		tmpPtr = TEST_DO( sizeof( size_t ) );
		TEST_UNDO( tmpPtr );
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

