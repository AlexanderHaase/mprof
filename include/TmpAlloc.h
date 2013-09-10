/*
	Copyright (c) 2013 by Alexander Haase ( https://www.alexanderhaase.net )
	Project: ahmalloc ( https://www.alexanderhaase.net/projects/ahmalloc.html )
	Created: 2013-09-10
	License: LGPL

	Very limited temporary allocator for boostrapping.
*/
#pragma once
#include <Personality.h>

extern const struct AllocatorVtable tmpAllocVtable;

size_t tmpAllocRemaining( void );

