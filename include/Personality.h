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
	dynamic personality support
*/
#pragma once
#include <stddef.h>

/*!	Describes an allocator personality per the stdlib functions. The 
	construct, destruct, and name values are optional.

	To be visible, personalites must be added to the init or mode tables in 
	libmprof.c
*/
struct AllocatorVtable {
	void *	( * malloc )	( size_t in_size );
	void 	( * free )	( void * in_ptr );
	void *	( * calloc )	( size_t in_size, size_t in_qty );
	void *	( * realloc )	( void * in_ptr, size_t in_size );		
	void	( * construct )	( void );					/*!< Optional, called when the vtable is activated */
	void	( * destruct )	( void );					/*!< note, no gaurantee it will be called when application closes */
	const char * name;							/*!< unique name for configuration purposes */
};

//! vtable mprof uses to fulfill request
extern struct AllocatorVtable mprofVtable;

//! bootstrapping vtable for setting MODE
extern struct AllocatorVtable postInitVtable;

//! vtable for backing allocator
extern struct AllocatorVtable defaultVtable;
