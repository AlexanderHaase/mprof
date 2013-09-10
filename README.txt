Copyright (c) 2013 by Alexander Haase ( https://www.alexanderhaase.net )
Project: mprof ( https://www.alexanderhaase.net/projects/mprof.html )
Created: 2013-09-10
License: LGPL

The very novelly named MPROF malloc profiling project!

Mprof seeks to provide a set of tools for both profiling application and library malloc behavoir. It's principly divided into two fronts: A performance benchmarker, and an allocation profiler. At the moment, only the later exists, and mostly in a TODO state.

Performance Benchmarking(TODO): Because allocator performance is very sensitive to actual access patterns inside the application, performance profing resorts to iterative benchmarking of real-world workloads.

Allocation Profiling:
Mprof approaches profiling from the LD_PRELOAD perspective. libmprof.so combines a modular profiler interface and the ability to chainload an allocator to facilitate memory use profiling. The configuration is defined via the "MPROF_CONF" environment variable. The hooks attempt to be as low-overhead as possible, amounting to a function pointer per call.

Configuration:
libmprof.so requires two central parameters, an "INIT" module to find the backing allocator, and a "MODE" module for profiling( yes, MODE *could* be optional, but it would make more sense to LD_PRELOAD="myAllocator.so" instead at that point ). Each module can have options specified via MPROF_CONF environment variable.

Init modules:
-LD_NEXT	Uses the dlsym RLTD_NEXT flag to resolve the backing allocator. It has no options.
-SO_LOAD(TODO)	Loads allocator symbols from the library specified with the "SO_PATH" variable.

Profile modules:
-LogFD		Logs all allocations to a file descriptor in the order ( <fn name>\t<arg1>\t...<argN>\t<ret> ). TODO: add option for output FD, currently fixed on stderr.
-Count		Logs call counts using a per-thread cache. Caches are collected and dumped at program termination( some programs don't exit cleanly, so no output ). TODO: move to an mmap/file backing to be more resilient against craches, add option to anticipate number of thread caches.
-RecordMmap(TODO) Binary logs each call argument, result, thread, and time stamp to an mmap'd file of static size( probably determined from the count module ).

Analysis (TODO): Application that reads Count & Record logs, generates memory usage data, images/movie?, sorts records for playback, and does playback.
