/*
	Copyright (c) 2013 by Alexander Haase ( https://www.alexanderhaase.net )
	Project: ahmalloc ( https://www.alexanderhaase.net/projects/ahmalloc.html )
	Created: 2013-09-08
	License: LGPL

	zero-alloc argument parsing focused on evironment variables.
*/
#include <ParseEnv.h>
#include <stdlib.h>	//for getenv
#include <string.h>	//for string functions
#include <ctype.h>	//for isspace


const char * confKey = "MPROF_CONF";
const char * confValue = NULL;

const char * getConfStr( void ) {
	if( confValue == NULL ) {
		confValue = getenv( confKey );
	}
	return confValue;
}

// stupid inefficient, but simple.
//FIXME: distinguish between quoted and unquoted text
const char * findKey( const char * in_value, const char * in_key ) {
	const size_t keySize = strlen( in_key );
	for( const char * valueItr = in_value; *valueItr != '\0'; valueItr++ ) {
		if( 0 == strncmp( valueItr, in_key, keySize ) ) {
			return valueItr;
		}
	}
	return NULL;
}

// require =<Value> where value is delimited by quotes or white space
const char * getValue( const char * in_value, size_t * out_size ) {
	if( *in_value != '=' ) {
		return NULL;
	}

	const char delim = *(++in_value);
	const size_t valueSize = strlen( in_value );
	switch( delim ) {
	case '\'':
	case '"':
		++in_value;
		for( *out_size = 0; *out_size < valueSize - 1; *out_size += 1 ) {
			if( in_value[ *out_size ] == delim ) {
				return in_value;
			}
		}
		return NULL;
	default:
		for( *out_size = 0; *out_size < valueSize; *out_size += 1 ) {
			if( isspace( in_value[ *out_size ] ) ) {
				break;
			}
		}
		return in_value;
	}
}


const char * findArg( const char * in_args, const char * in_key, size_t * out_size ) {
	const char * key = findKey( in_args, in_key );
	if( ! key ) {
		out_size = 0;
		return NULL;
	} else {
		return getValue( key + strlen( in_key ), out_size );
	}
}

