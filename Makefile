PROJECT=libmprof.so
OBJECTS=src/Personality.o		\
	src/mprofLDInit.o		\
	src/mprofLogFD.o		\

CFLAGS := -Wall -Wextra -Werror --pedantic -I./include -g -std=c99
LDLIBS := 

$(PROJECT): $(OBJECTS)
	gcc src/libmprof.c -fPIC -shared -ldl -Wl,-soname,libmprof.so.0.1 -o $(PROJECT) $(CFLAGS) $(LDLIBS) $(OBJECTS)

test: test.o
	gcc test.o $(CFLAGS) -O0 -o test

clean:
	rm -f $(PROJECT)
	rm -f $(OBJECTS)
	rm -f test test.o

edit-all:
	gedit Makefile src/*.c include/*.h &
commit-all: 
	git add Makefile src/*.c include/*.h

