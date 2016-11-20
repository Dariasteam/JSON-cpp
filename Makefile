CC=clang++
CXXFLAGS= -std=c++11

OBJS = manager.o parser.o object.o tests/test.o
TEST = tests/test.o
MAIN = main.o

all: ${OBJS}
	$(CC) $(CXXFLAGS) -o tests/jsonParser ${OBJS}
clean:
	rm -rf *.o tests/test

tests: ${OBJS}
	$(CC) $(CXXFLAGS) -o jsonParser ${OBJS}

