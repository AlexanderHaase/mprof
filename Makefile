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
LDLIBS := -lpthread -ldl

#LDLIBS must be the last arg to gcc for some reason....
libmprof.so: $(LIB_OBJECTS)
	$(CC) src/libmprof.c -shared -Wl,-soname,libmprof.so.0.1 -o libmprof.so $(CFLAGS) $(LIB_OBJECTS) $(LDLIBS)

mprof: $(MPROF_OBJECTS) $(CPP_OBJECTS)
	$(CXX) -o mprof $(CXXFLAGS) $(MPROF_OBJECTS) $(CPP_OBJECTS) $(LDLIBS) -std=c++0x

test: test.o
	$(CC) test.o $(CFLAGS) -O0 -o test -lpthread

latencyBench: src/latencyBench.o
	$(CC) src/latencyBench.o $(CFLAGS) -O3 -o latencyBench
clean:
	rm -f src/*.o
	rm -f test test.o mprof latencyBench libmprof.so

edit-all:
	gedit Makefile src/*.c src/*.cpp include/*.h &
commit-all: 
	git add Makefile src/*.c src/*.cpp include/*.h

