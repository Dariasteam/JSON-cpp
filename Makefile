CC=clang++
CXXFLAGS=-g -std=c++11

OBJS = manager.o parser.o object.o tests/test.o ## main.o

all: ${OBJS}
	$(CC) $(CXXFLAGS) -o jsonParser ${OBJS}
clean:
	rm -rf *.o jsonParser
