/*
	Copyright (c) 2013 by Alexander Haase ( https://www.alexanderhaase.net )
	Project: ahmalloc ( https://www.alexanderhaase.net/projects/ahmalloc.html )
	Created: 2013-09-08
	License: LGPL

	file descriptor oriented mprof monitors
*/
#pragma once
#include <Personality.h>

extern const struct AllocatorVtable mprofLogFDVtable;

int mprofLogFDSet( int fd );
