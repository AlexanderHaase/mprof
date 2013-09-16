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

	-analysis components
*/
#pragma once
#include <mprofRecord.h>
#include <stdio.h>

#ifdef __cplusplus
#include <map>
#include <vector>
#include <list>

namespace mprof {
	inline bool olderThan( const struct MprofRecordAlloc * in_recordA, const struct MprofRecordAlloc * in_recordB ) {
		return ( in_recordA->sec < in_recordB->sec ) || ( in_recordA->sec == in_recordB->sec && in_recordA->usec < in_recordB->usec );
	}

	uint64_t deltaT( const struct MprofRecordAlloc * in_recordA, const struct MprofRecordAlloc * in_recordB ) {
		if( olderThan( in_recordA, in_recordB ) ) {
			return ( in_recordB->sec - in_recordA->sec ) * 1000000lu + in_recordB->usec - in_recordA->usec;
		} else {
			return ( in_recordA->sec - in_recordB->sec ) * 1000000lu + in_recordA->usec - in_recordB->usec;
		}
	}
	/*!	\brief Counts the number of allocations of each size, total
	 *	allocations, and total allocated.
	 *
	 *	Returns the number of allocations and total allocated.
	 *
	 *	\param[ in ]	record(s)	c array of records to count
	 *	\param[ in ]	quantity	number of records in array
	 *	\param[ out ]	sizes		map of size->count results
	 */
	std::pair<uint64_t,uint64_t> countSizes( const struct MprofRecordAlloc * in_record, const size_t in_recordQty, std::map<uint64_t,uint64_t> & out_sizes );

	/*!	\breif Coppies elements from range to result if they meet
	 *	a condition. (see std::copy_if) result must begin a range
	 *	of at least size [ begin, end ).
	 *
	 *	returns the position beyond the last output iterator.
	 *
	 *	\param[ in ]	begin	iterator to start
	 *	\param[ in ]	end	iterator to end
	 *	\param[ in ]	result	iterator to output
	 *	\param[ in ]	cond
	 */
	template <class T, class Y, class C >
	Y copy_if( T begin, T end, Y result, C & condition ) {
		for( ; begin != end; ++begin ) {
			if( filter( *begin ) ) {
				*result = *begin;
				++result;
			}
		}
		return result;
	}

	class RecordModeCondition {
	protected:
		const struct MprofRecordAlloc * records;
		const uint8_t * modes;
		const size_t nModes;
	public:
		RecordModeCondition( const struct MprofRecordAlloc * in_records, const uint8_t * in_modes, const size_t in_nModes )
		: records( in_records )
		, modes( in_modes )
		, nModes( in_nModes ) {}

		bool operator()( size_t i ) {
			for( size_t index  = 0; index < nModes; index++ ) {
				if( records[ i ].header.mode == modes[ index ] ) {
					return true;
				}
			}
			return false;
		}
	};

	/*!	\brief Sorts the records chronologically into an index vector.
	 *
	 *	Returns a vector of indices into the input array in ascending
	 *	order. Note: empty records will probably be first.
	 *
	 *	\param[ in ]	record(s)	c array of records
	 *	\param[ in ]	quantity	number of records in array
	 */
	std::vector<size_t> sortChrono( const struct MprofRecordAlloc * in_record, const size_t in_recordQty );

	/*	\breif Class for analyzing allocation compactness.
		Mostly a stub class at the moment-will flesh out more when we
		record sbreak/mmap events.
	*/
	class CompactnessAnalysis {
	protected:

		std::map< uint64_t, std::list< std::pair<uint64_t, uint64_t> > > allocationMap;
		std::map< uint64_t,uint64_t > vmPagesImplicit;
		uint64_t clientHighWaterMark;
		uint64_t serverHighWaterMark;
		uint64_t vmPageSize;
		uint64_t clientSize;
		void reset( const uint64_t in_pageSize );
		static const uint64_t nullIndex;
		std::list<float> compactness;
		
		void refPages( uint64_t in_address, uint64_t in_size, bool in_ref );

		void setCompactness( uint64_t size );

		static bool getAlloc( const struct MprofRecordAlloc * in_record, uint64_t & out_address, uint64_t & out_size );
		static bool getFree( const struct MprofRecordAlloc * in_record, uint64_t & out_address );

		bool addRecord( const struct MprofRecordAlloc * in_record, const size_t in_index );

		void build( const struct MprofRecordAlloc * in_record, std::vector<size_t>::const_iterator in_orderBegin, std::vector<size_t>::const_iterator in_orderEnd );
	public:
		void analyze( const struct MprofRecordAlloc * in_record, const std::vector<size_t> in_order, const uint64_t in_pageSize = 4096u );

		float min( void ) const;
		float max( void ) const;
		float mean( void ) const;
		std::pair< uint64_t, uint64_t> highWaterMarks( void ) const;
	};
}

extern "C" {
#endif

	void mprofSizeAnalysis( FILE * out_file, const struct MprofRecordAlloc * in_record, const size_t in_recordQty );

	void mprofCompactnessAnalysis( FILE * out_file, const struct MprofRecordAlloc * in_record, const size_t in_recordQty );
#ifdef __cplusplus
}
#endif
