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

