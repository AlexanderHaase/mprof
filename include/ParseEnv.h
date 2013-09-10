/*
	Copyright (c) 2013 by Alexander Haase ( https://www.alexanderhaase.net )
	Project: ahmalloc ( https://www.alexanderhaase.net/projects/ahmalloc.html )
	Created: 2013-09-08
	License: LGPL

	zero-alloc argument parsing focused on evironment variables.
*/
#pragma once
#include <stddef.h>

//!	Returns a pointer to the configuration string, or NULL
const char * getConfStr( void );

/*!	Finds the value for an argument of the form <KEY>=<VALUE> where value
	is optionally quoted. If the key is found, it returns a pointer to the
	start of the value sub-string( ommiting quotes, if present ). Otherwise
	it returns NULL. Note that the string may extend beyond the substring.

	@param	in_args		The argument string to search
	@param	in_key		The key to find within the argument string
	@param	out_size	The size of the resultant value sub-string
*/
const char * findArg( const char * in_args, const char * in_key, size_t * out_size );

