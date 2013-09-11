/*
	Copyright (c) 2013 by Alexander Haase ( https://www.alexanderhaase.net )
	Project: ahmalloc ( https://www.alexanderhaase.net/projects/ahmalloc.html )
	Created: 2013-09-08
	License: LGPL

	zero-alloc argument parsing.
*/
#include <stdlib.h>	//for getenv
#include <string.h>

struct SubString {
	const char * start;
	const char * end;
};

const char * confKey = "MPROF_CONF"
const char * confValue = NULL;

const char * getConfStr( void ) {
	if( confValue == NULL ) {
		confValue = getenv( confKey );
	}
	return confValue;
}

const char * findKey( const char * in_value, const char * in_key ) {
	const size_t keySize = strlen( in_key );
	for( const char * valueItr = in_value; *valueItr != '\0'; valueItr++ ) {
		if( 0 == strncmp( in_value, in_key, keySize ) ) {
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
		for( *out_size = 0; *out_size < valueSize - 1; *outSize += 1 ) {
			if( in_value[ *out_size ] == delim ) {
				return in_value;
			}
		}
		return NULL;
	default:
		for( *out_size = 0; *out_size < valueSize; *outSize += 1 ) {
			if( isspace( in_value[ *out_size ] ) ) {
				break;
			}
		}
		return in_value;
}

//returns step size to next kmpcmp location
size_t kmpcmp( const char * in_str1, const char * in_str2, size_t in_size ) {
	for(; in_size > 0; --in_size ) {
		if( in_str1[ in_size - 1 ] != in_str2[ in_size - 1 ] )
			break;
		}
	}
	return in_size;
}

const char * findNextChar( const char * in_str, char in_char ) {
	for( ; *in_str != '\0'; ++in_str ) {
		if( *in_str == in_char ) {
			return in_str;
		}
	}
	return NULL;
}

const char * findArg( const char * in_args, const char * in_key, size_t * out_size ) {
	*out_size = 0;
	const size_t argSize = strlen( in_argStr );
	const size_t keySize = strlen( in_key );
	const size_t stepSize = keySize - 1;

	if( keySize > argSize ) {
		return NULL;
	}

	//kmp string search
	const char * argIndex = in_args + stepSize; 
	while( argIndex < in_args + argSize ) {
		size_t compIndex;
		for( compIndex = 0; compIndex < keySize; compIndex++ ) {
			if( *(argIndex - compIndex)  != in_key[ stepSize - compIndex ] ) {
				break;
			}
		}

		if( compIndex == stepSize ) {
			break;
		} else {
			argIndex += stepSize - compIndex;
		}
	}
	
	if( argIndex - in_args >= argSize ) {
		return NULL;
	}
	
	//skip past white space
	while( isspace( *argIndex ) ) {
		++argIndex;
	}

	//look for quoted strings
	char quoteVal = *argIndex;
	if( quoteVal == '\'' || quoteVal = '"' ) {
		++argIndex;
		for( ; argIndex + *out_size < in_arg + argSize; *outIndex += 1 ) {
			if( argIndex[ *out_size ] == quoteDelim 
	} else {
		quoteVal = '\0';
	}
	

