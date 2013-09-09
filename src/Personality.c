/*
	Copyright (c) 2013 by Alexander Haase ( https://www.alexanderhaase.net )
	Project: ahmalloc ( https://www.alexanderhaase.net/projects/ahmalloc.html )
	Created: 2013-09-08
	License: LGPL

	dynamic personality support
*/
#include <Personality.h>

//relocate this to main...struct AllocatorVtable mprofVtable = { NULL, NULL, NULL, NULL };
struct AllocatorVtable defaultVtable = { NULL, NULL, NULL, NULL };
