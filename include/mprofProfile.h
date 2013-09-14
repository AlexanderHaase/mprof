/*
	Copyright (c) 2013 by Alexander Haase ( https://www.alexanderhaase.net )
	Project: mprof ( https://www.alexanderhaase.net/projects/mprof.html )
	Created: 2013-09-12
	License: LGPL

	Functionality for the mprof profiler program.

	-analysis components
*/
#pragma once
#include <mprofRecord.h>
#include <stdio.h>

#ifdef __cplusplus
#include <map>
std::pair<uint64_t,uint64_t> mprofCountSizes( const struct MprofRecordAlloc * in_record, const size_t in_recordQty, std::map<uint64_t,uint64_t> & out_sizes );

extern "C" {
#endif

	void mprofSizeAnalysis( FILE * out_file, const struct MprofRecordAlloc * in_record, const size_t in_recordQty );
#ifdef __cplusplus
}
#endif
