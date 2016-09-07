all:
	clang++ tests/test.cpp manager.cpp parser.cpp object.cpp -std=c++11 -o test.out 
