# -*- Makefile -*-

CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++11 --coverage
LIBS := --coverage

all: main tests

main: main.o
	$(CXX) $^ $(LIBS) -o $@

tests: tests.o
	$(CXX) $^ $(LIBS) -o $@

.PHONY: clean

clean:
	rm *.o main *.gcov *.gcno *.gcda
