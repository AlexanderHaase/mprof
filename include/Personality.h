/*
	Copyright (c) 2013 by Alexander Haase ( https://www.alexanderhaase.net )
	Project: ahmalloc ( https://www.alexanderhaase.net/projects/ahmalloc.html )
	Created: 2013-09-08
	License: LGPL

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
