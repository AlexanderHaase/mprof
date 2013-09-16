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
	if( ! in_args || ! in_key || ! out_size ) {
		return NULL;
	}
	const char * key = findKey( in_args, in_key );
	if( ! key ) {
		out_size = 0;
		return NULL;
	} else {
		return getValue( key + strlen( in_key ), out_size );
	}
}
