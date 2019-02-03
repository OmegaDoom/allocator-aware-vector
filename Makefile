# -*- Makefile -*-

CXX := g++
CXXFLAGS := -std=c++11

all: main tests

main: main.o
	$(CXX) $^ -o $@

tests: tests.o
	$(CXX) $^ -o $@

.PHONY: clean

clean:
	rm *.o main
