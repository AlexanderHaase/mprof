Copyright (c) 2013 by Alexander Haase ( https://www.alexanderhaase.net )
Project: mprof ( https://www.alexanderhaase.net/projects/mprof.html )
Created: 2013-09-10
License: LGPL
Contact: See https://www.alexanderhaase.net/contact.html

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

**********************************************************************

The very novelly named MPROF malloc profiling project!

Mprof seeks to provide a set of tools for both profiling application and library malloc behavoir. It's principly divided into two fronts: A performance benchmarker, and an allocation profiler. At the moment, only the later exists, and mostly in a TODO state.

Quickstart: make 'mprof' and 'libmprof.so', from the project directory, run "mprof -t 'cat test.c'" where -t specifies a command to run.

Performance Benchmarking(TODO): Because allocator performance is very sensitive to actual access patterns inside the application, performance profing resorts to iterative benchmarking of real-world workloads.

*****Allocation Profiling*****
Mprof approaches profiling from the LD_PRELOAD perspective. libmprof.so combines a modular profiler interface and the ability to chainload an allocator to facilitate memory use profiling. The configuration is defined via the "MPROF_CONF" environment variable. The hooks attempt to be as low-overhead as possible, amounting to a function pointer per call.

Configuration:
libmprof.so requires two central parameters, an "INIT" module to find the backing allocator, and a "MODE" module for profiling( yes, MODE *could* be optional, but it would make more sense to LD_PRELOAD="myAllocator.so" instead at that point ). Each module can have options specified via MPROF_CONF environment variable.

Init modules:
-LD_NEXT	Uses the dlsym RLTD_NEXT flag to resolve the backing allocator. It has no options.
-SO_LOAD(TODO)	Loads allocator symbols from the library specified with the "SO_PATH" variable.

Profile modules:
-LogFD		Logs all allocations to a file descriptor in the order ( <fn name>\t<arg1>\t...<argN>\t<ret> ). TODO: add option for output FD, currently fixed on stderr.

-Count		Logs call counts using a per-thread cache. Caches are collected and dumped at program termination( some programs don't exit cleanly, so no output ). 
	Option: COUNTS_PATH="<path to counts file>"

-LogMmap Binary logs each call argument, result, thread, and time stamp to an mmap'd file of static size( probably determined from the count module ).
	Option: LOG_PATH="<path to log file>"
	Option: LOG_QTY=<base 10 records to pre-allocate at initialization, and the log growth step size, default 1000>
	Option: CACHE_QTY=<base 10 records to thread-local cache to reduce contention, default 10>	

*****Analysis (In Progress)*****
mprof is an application that reads Count & Record logs, generates memory usage data, images/movie?, sorts records for playback, and does playback. Even more usefully, it attempts to wrap generating data and appropriately size count opperations.

Currently, analysis only logs size classes and virtual/physical compactness.

mprof <>
	displays any found counts files

mprof -t "<argument>" ( -c "<counts file>" -l "<log file>"
	attempts to profile command <argument>.	Runs the argument twice,
	once with mode "Count" to guestimate required log space, once
	with mode "LogMmap". Runs analysis on output.
mprof -m "<mode>"
 	comming soon!

