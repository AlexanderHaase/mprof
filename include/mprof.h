/*
	Copyright (c) 2013 by Alexander Haase ( https://www.alexanderhaase.net )
	Project: mprof ( https://www.alexanderhaase.net/projects/mprof.html )
	Created: 2013-09-12
	License: LGPL

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
