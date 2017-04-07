CC=clang++
CXXFLAGS=-g -std=c++11

OBJS = src/abstract_serializable.o src/auxserialization.o src/serializable.o src/object.o src/manager.o src/parser.o src/main.o
TESTS = src/abstract_serializable.o src/auxserialization.o src/object.o src/manager.o src/parser.o tests/test.o

all: ${OBJS}
	$(CC) $(CXXFLAGS) -o jsonParser ${OBJS}

tests: ${TESTS}
	${CC} ${CXXFLAGS} -o jsonTests ${TESTS}

doc:
	mermaid -w 950 docs/abstract_object_diagram.md -o docs/pics
	mermaid -w 950 docs/tree_diagram_example.md -o docs/pics
	cldoc generate -std=c++11 -- --merge docs/manual_doc.md --output docs --report src/*.hpp

clean:
	rm -rf src/*.o jsonParser
	rm -rf tests/*.o jsonTests
