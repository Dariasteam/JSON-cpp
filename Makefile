CC=clang++
CXXFLAGS=-g -std=c++11

OBJS = serializable.o object.o manager.o parser.o main.o
TESTS = object.o manager.o parser.o tests/test.o


all: ${OBJS}
	$(CC) $(CXXFLAGS) -o jsonParser ${OBJS}

tests: ${TESTS}
	${CC} ${CXXFLAGS} -o jsonTests ${TESTS}

doc:
	mermaid docs/abstract_object_diagram.md -o docs/pics
	cldoc generate -std=c++11 -- --merge docs/manual_doc.md --output docs --report *.hpp
	

clean:
	rm -rf *.o jsonParser
	rm -rf tests/*.o jsonTests

