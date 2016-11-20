CC=clang++
CXXFLAGS=-g -std=c++11

OBJS = object.o manager.o parser.o main.o
TESTS = object.o manager.o parser.o tests/test.o


all: ${OBJS}
	$(CC) $(CXXFLAGS) -o jsonParser ${OBJS}

tests: ${TESTS}
	${CC} ${CXXFLAGS} -o tests/jsonTests ${TESTS}
clean:
	rm -rf *.o jsonParser
	rm -rf tests/*.o tests/jsonTests 

