/*
	Copyright (c) 2013 by Alexander Haase ( https://www.alexanderhaase.net )
	Project: ahmalloc ( https://www.alexanderhaase.net/projects/ahmalloc.html )
	Created: 2013-09-08
	License: LGPL

	dynamic personality support
*/
#pragma once
#include <stddef.h>

struct AllocatorVtable {
	void * ( * malloc )( size_t in_size );
	void ( * free ) ( void * in_ptr );
	void * ( * calloc )( size_t in_size, size_t in_qty );
	void * ( * realloc )( void * in_ptr, size_t in_size );
};


extern struct AllocatorVtable mprofVtable;
extern struct AllocatorVtable defaultVtable;
