/*
	Copyright (c) 2013 by Alexander Haase ( https://www.alexanderhaase.net )
	Project: mprof ( https://www.alexanderhaase.net/projects/mprof.html )
	Created: 2013-09-12
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
	Functionality for the mprof profiler program.

	-dump binary formats to text.
	-set up preloading
*/
#pragma once
#include <mprofRecord.h>
#include <stdio.h>

void mprofCountsPrint( FILE * out_stream, const struct MprofRecordCount * in_counts );

void mprofCountsTotal( const struct MprofRecordCount * in_counts, const size_t in_size, struct MprofRecordCount * out_total );

bool mprofCountsDump( FILE * out_stream, const char * in_path );

int mprofMain( int argc, char ** argv );
