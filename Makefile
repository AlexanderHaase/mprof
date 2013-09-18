#	Copywrite (c) 2013 by Alexander Haase.
#	This file is part of mprof.
#
#	mprof is free software: you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation, either version 3 of the License, or
#	(at your option) any later version.
#
#	mprof is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#
#	You should have received a copy of the GNU General Public License
#	along with mprof.  If not, see <http://www.gnu.org/licenses/>.

COMMON_OBJECTS := 			\
	src/Personality.o		\
	src/ParseEnv.o			\
	src/mprofRecord.o		\

LIB_OBJECTS := $(COMMON_OBJECTS)	\
	src/mprofLDInit.o		\
	src/mprofLogFD.o		\
	src/mprofCount.o		\
	src/TmpAlloc.o			\
	src/mprofLogMmap.o		\

MPROF_OBJECTS := $(COMMON_OBJECTS)	\
	src/mprof.o			\
	src/main.o			\

CPP_OBJECTS :=				\
	src/mprofProfile.o

CXXFLAGS := -Wall -Wextra -Werror --pedantic -I./include -g -std=c++0x
CFLAGS := -Wall -Wextra -Werror --pedantic -I./include -g -std=c99 -fPIC
LDLIBS := -lpthread -ldl -lrt

#LDLIBS must be the last arg to gcc for some reason....
libmprof.so: $(LIB_OBJECTS)
	$(CC) src/libmprof.c -shared -Wl,-soname,libmprof.so.0.1 -o libmprof.so $(CFLAGS) $(LIB_OBJECTS) $(LDLIBS)

mprof: $(MPROF_OBJECTS) $(CPP_OBJECTS)
	$(CXX) -o mprof $(CXXFLAGS) $(MPROF_OBJECTS) $(CPP_OBJECTS) $(LDLIBS) -std=c++0x

test: src/test.o
	$(CC) src/test.o $(CFLAGS) -O0 -o test -lpthread -lrt

latencyBench: src/latencyBench.o
	$(CC) src/latencyBench.o $(CFLAGS) -O3 -o latencyBench
clean:
	rm -f src/*.o
	rm -f test mprof latencyBench libmprof.so

edit-all:
	gedit Makefile src/*.c src/*.cpp include/*.h &
commit-all: 
	git add Makefile src/*.c src/*.cpp include/*.h

