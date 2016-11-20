CC=clang++
CXXFLAGS=-g -std=c++11

OBJS = object.o manager.o parser.o serializable.o main.o

all: ${OBJS}
	$(CC) $(CXXFLAGS) -o jsonParser ${OBJS}
clean:
	rm -rf *.o jsonParser

