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
#include <Personality.h>

//relocated this to main...struct AllocatorVtable mprofVtable = { NULL, NULL, NULL, NULL };
struct AllocatorVtable defaultVtable = { NULL, NULL, NULL, NULL, NULL, NULL, NULL };
struct AllocatorVtable postInitVtable = { NULL, NULL, NULL, NULL, NULL, NULL, NULL };
