CC=clang++
CXXFLAGS= -std=c++11

OBJS = manager.o parser.o object.o main.o ##tests/test.o

all: ${OBJS}
	$(CC) $(CXXFLAGS) -o jsonParser ${OBJS}
clean:
	rm -rf *.o jsonParser
