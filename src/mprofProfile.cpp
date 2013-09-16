/*
	Copyright (c) 2013 by Alexander Haase ( https://www.alexanderhaase.net )
	Project: mprof ( https://www.alexanderhaase.net/projects/mprof.html )
	Created: 2013-09-12
	License: LGPL

	Functionality for the mprof profiler program.

	-analysis components
*/
#include <mprofProfile.h>
#include <algorithm>
#include <iostream>
#include <limits>

const uint64_t mprof::CompactnessAnalysis::nullIndex = std::numeric_limits<uint64_t>::max();

std::pair<uint64_t,uint64_t> mprof::countSizes( const struct MprofRecordAlloc * in_record, const size_t in_recordQty, std::map<uint64_t,uint64_t> & out_sizes ) {
	const struct MprofRecordAlloc * const recordEnd = in_record + in_recordQty;
	std::pair<uint64_t,uint64_t> ret( 0, 0 );

	for( const struct MprofRecordAlloc * recordItr = in_record; recordItr != recordEnd; ++recordItr ) {
		uint64_t allocSize;
		switch( recordItr->header.mode ) {
		case ( MPROF_MODE_MALLOC ) :
			allocSize = recordItr->type.malloc.size;
			break;
		case ( MPROF_MODE_CALLOC ) :
			allocSize = recordItr->type.calloc.size * recordItr->type.calloc.count;
			break;
		case ( MPROF_MODE_REALLOC ) :
			allocSize = recordItr->type.realloc.size;
			break;
		default:
			continue;
		}

		std::map<uint64_t,uint64_t>::iterator result = out_sizes.find( allocSize );
		if( result != out_sizes.end() ) {
			result->second += 1;
		} else {
			out_sizes.insert( std::pair< uint64_t, uint64_t >( allocSize, 1 ) );
		}
		ret.first += 1;
		ret.second += allocSize;
	}
	return ret;
}


void mprofSizeAnalysis( FILE * out_file, const struct MprofRecordAlloc * in_record, const size_t in_recordQty ) {
	std::map<uint64_t, uint64_t> sizeMap;
	std::pair<uint64_t, uint64_t> total = mprof::countSizes( in_record, in_recordQty, sizeMap );

	fprintf( out_file, "\tBytes\tCount\t%%Size\t%%Qty\n" );
	for( std::map<uint64_t, uint64_t>::const_iterator sizeItr = sizeMap.begin(); sizeItr != sizeMap.end(); ++sizeItr ) {
		fprintf( out_file, "\t%llu\t%llu\t%llu\t%llu\n", 
			UNSIGNEDPAD( sizeItr->first ),
			UNSIGNEDPAD( sizeItr->second ),
			UNSIGNEDPAD( sizeItr->first * sizeItr->second * 100 / total.second ),
			UNSIGNEDPAD( sizeItr->second * 100 / total.first ) );
	}
	fprintf( out_file, "Total\t%llu\t%llu\t100\tNA\n", UNSIGNEDPAD( total.second ), UNSIGNEDPAD( total.first ) );
}


std::vector<size_t> mprof::sortChrono( const struct MprofRecordAlloc * in_record, const size_t in_recordQty ) {
	std::vector<size_t> ret;
	ret.reserve( in_recordQty );
	for( size_t index = 0; index < in_recordQty; index++ ) {
		ret.push_back( index );
	}

	class Compare {
	protected: 
		const struct MprofRecordAlloc * record;
	public:
		Compare( const struct MprofRecordAlloc * in_record )
		: record( in_record ) {}

		bool operator()( size_t i, size_t j ) {
			return ( record[ i ].sec < record[ j ].sec ) || ( record[ i ].sec == record[ j ].sec && record[ i ].usec < record[ j ].usec );
		}
	} compare( in_record );

	std::sort( ret.begin(), ret.end(), compare );
	return ret;
}

void mprof::CompactnessAnalysis::reset( const uint64_t in_pageSize ) {
	allocationMap.clear();
	vmPagesImplicit.clear();
	compactness.clear();
	clientHighWaterMark = 0;
	serverHighWaterMark = 0;
	vmPageSize = in_pageSize;
}

void mprof::CompactnessAnalysis::build( const struct MprofRecordAlloc * in_record, std::vector<size_t>::const_iterator in_orderBegin, std::vector<size_t>::const_iterator in_orderEnd ) {
	uint64_t clientSize = 0;
	for( std::vector<size_t>::const_iterator orderItr = in_orderBegin; orderItr != in_orderEnd; ++orderItr ) {
		const struct MprofRecordAlloc * const record = in_record + *orderItr;
		uint64_t size;
		uint64_t address;

		std::map< uint64_t, std::list< std::pair<uint64_t, uint64_t> > >::iterator mapItr;

		if( getFree( record, address ) && address ) {
			mapItr = allocationMap.find( address );
			if( mapItr == allocationMap.end() || mapItr->second.empty() || mapItr->second.back().second != nullIndex ) {
				std::cerr << "mprof::CompactnessAnalysis::build: found unmatched free at index '" << *orderItr << "' for address '" << std::hex << address << std::dec << "'" << std::endl;
			} else {
				mapItr->second.back().second = *orderItr;
				getAlloc( in_record + mapItr->second.back().first, address, size );
				//adjust client size
				clientSize -= size;

				//mark vmMap
				refPages( address, size, false );

				setCompactness( clientSize );
			}
		}
		if( getAlloc( record, address, size ) && address ) {
			mapItr = allocationMap.find( address );
			if( mapItr == allocationMap.end() ) {
				allocationMap[ address ] = std::list< std::pair<uint64_t, uint64_t> >();
				mapItr = allocationMap.find( address );
			}
			mapItr->second.push_back( std::pair<uint64_t, uint64_t>( *orderItr, nullIndex ) );

			//adjust client size
			clientSize += size;
			if( clientSize > clientHighWaterMark ) {
				clientHighWaterMark = clientSize;
			}

			//mark vmMap
			refPages( address, size, true );

			setCompactness( clientSize );
		}
	}
}

void mprof::CompactnessAnalysis::analyze( const struct MprofRecordAlloc * in_record, const std::vector<size_t> in_order, const uint64_t in_pageSize ) {
	reset( in_pageSize );
	build( in_record, in_order.begin(), in_order.end() );
}

void mprof::CompactnessAnalysis::refPages( uint64_t in_address, uint64_t in_size, bool ref ) {
	uint64_t vmPageStart = in_address / vmPageSize;
	uint64_t vmPageEnd = ( in_address + in_size ) / vmPageSize;
	for( size_t index = vmPageStart; index <= vmPageEnd; index++ ) {
		std::map<size_t,size_t>::iterator mapItr = vmPagesImplicit.find( index );
		if( mapItr == vmPagesImplicit.end() ) {
			vmPagesImplicit[ index ] = ref;
		} else if( ref ) {
			mapItr->second += 1;
		} else {
			mapItr->second -= 1;
			if( mapItr->second == 0 ) {
				vmPagesImplicit.erase( mapItr );
			}
		}
	}
	if( vmPagesImplicit.size() > serverHighWaterMark ) {
		serverHighWaterMark = vmPagesImplicit.size();
	}
}

bool mprof::CompactnessAnalysis::getAlloc( const struct MprofRecordAlloc * in_record, uint64_t & out_address, uint64_t & out_size ) {
	switch( in_record->header.mode ) {
	case ( MPROF_MODE_MALLOC ):
		out_size = in_record->type.malloc.size;
		out_address = in_record->type.malloc.result;
		return true;
	case ( MPROF_MODE_CALLOC ):
		out_size = in_record->type.calloc.count * in_record->type.calloc.size;
		out_address = in_record->type.calloc.result;
		return true;
	case ( MPROF_MODE_REALLOC ):
		out_size = in_record->type.realloc.size;
		out_address = in_record->type.realloc.result;
		return true;
	default:
		return false;
	}
}


bool mprof::CompactnessAnalysis::getFree( const struct MprofRecordAlloc * in_record, uint64_t & out_address ) {
	switch( in_record->header.mode ) {
	case ( MPROF_MODE_REALLOC ):
		out_address = in_record->type.realloc.pointer;
		return true;
	case ( MPROF_MODE_FREE ):
		out_address = in_record->type.free.pointer;
		return true;
	default:
		return false;
	}
}

void mprof::CompactnessAnalysis::setCompactness( const uint64_t in_size ) {
	if( vmPagesImplicit.size() ) {
		const uint64_t minPages = ( in_size ? ( in_size - 1 ) / vmPageSize + 1 : 0 );
		compactness.push_back( minPages / float( vmPagesImplicit.size() ) );
	}
}

float mprof::CompactnessAnalysis::min( void ) const {
	return *std::min_element( compactness.begin(), compactness.end() );
}

float mprof::CompactnessAnalysis::max( void ) const {
	return *std::max_element( compactness.begin(), compactness.end() );
}

float mprof::CompactnessAnalysis::mean( void ) const {
	size_t count = 0;
	float sum = 0.0f;
	for( std::list<float>::const_iterator itr = compactness.begin(); itr != compactness.end(); ++itr ) {
		sum += *itr;
		count += 1;
	}
	return sum / float( count );
}

std::pair< uint64_t, uint64_t > mprof::CompactnessAnalysis::highWaterMarks( void ) const {
	return std::pair< uint64_t, uint64_t > ( clientHighWaterMark, serverHighWaterMark );
}

void mprofCompactnessAnalysis( FILE * out_file, const struct MprofRecordAlloc * in_record, const size_t in_recordQty ) {
	mprof::CompactnessAnalysis ca;

	std::vector<size_t> order = mprof::sortChrono( in_record, in_recordQty );

	ca.analyze( in_record, order );

	fprintf( out_file, "Compactness( VM pages min : implicit ): min: %.2f\t max: %.2f\t mean %.2f\n", ca.min(), ca.max(), ca.mean() );
	std::pair< uint64_t, uint64_t > marks = ca.highWaterMarks();
	fprintf( out_file, "High Water: %llu VM pages( implicit ), %llu application bytes.\n", UNSIGNEDPAD( marks.second ), UNSIGNEDPAD( marks.first ) );
} 
