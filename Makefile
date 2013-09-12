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

CFLAGS := -Wall -Wextra -Werror --pedantic -I./include -g -std=c99 -fPIC
LDLIBS := -lpthread -ldl

#LDLIBS must be the last arg to gcc for some reason....
libmprof.so: $(LIB_OBJECTS)
	gcc src/libmprof.c -shared -Wl,-soname,libmprof.so.0.1 -o libmprof.so $(CFLAGS) $(LIB_OBJECTS) $(LDLIBS)

mprof: $(MPROF_OBJECTS)
	gcc -o mprof $(CFLAGS) $(MPROF_OBJECTS) $(LDLIBS)

test: test.o
	gcc test.o $(CFLAGS) -O0 -o test -lpthread

latencyBench: src/latencyBench.o
	gcc src/latencyBench.o $(CFLAGS) -O3 -o latencyBench
clean:
	rm -f src/*.o
	rm -f test test.o mprof latencyBench libmprof.so

edit-all:
	gedit Makefile src/*.c include/*.h &
commit-all: 
	git add Makefile src/*.c include/*.h

