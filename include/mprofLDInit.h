/*
	Copyright (c) 2013 by Alexander Haase ( https://www.alexanderhaase.net )
	Project: ahmalloc ( https://www.alexanderhaase.net/projects/ahmalloc.html )
	Created: 2013-09-08
	License: LGPL

	malloc functions to poll LD and populate the default table
*/
#pragma once
#include <Personality.h>

extern const struct AllocatorVtable mprofLDInitVtable;
extern struct AllocatorVtable postLDInitVtable;
