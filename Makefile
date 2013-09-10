PROJECT=libmprof.so
OBJECTS=src/Personality.o		\
	src/mprofLDInit.o		\
	src/mprofLogFD.o		\
	src/ParseEnv.o			\
	src/mprofCount.o

CFLAGS := -Wall -Wextra -Werror --pedantic -I./include -g -std=c99 -fPIC
LDLIBS := -ldl

#LDLIBS must be the last arg to gcc for some reason....
$(PROJECT): $(OBJECTS)
	gcc src/libmprof.c -fPIC -shared -ldl -Wl,-soname,libmprof.so.0.1 -o $(PROJECT) $(CFLAGS) $(OBJECTS) $(LDLIBS)

test: test.o
	gcc test.o $(CFLAGS) -O0 -o test $(LDLIBS)

clean:
	rm -f $(PROJECT)
	rm -f $(OBJECTS)
	rm -f src/libmprof.o
	rm -f test test.o

edit-all:
	gedit Makefile src/*.c include/*.h &
commit-all: 
	git add Makefile src/*.c include/*.h

