# -*- Makefile -*-

CXX := g++
CXXFLAGS := -std=c++11 -g

main: main.o
	$(CXX) $^ -o $@

.PHONY: clean

clean:
	rm *.o main
