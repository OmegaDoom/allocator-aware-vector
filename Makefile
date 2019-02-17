# -*- Makefile -*-

CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++11 --coverage
LIBS := --coverage

all: main check

main: main.o
	$(CXX) $^ $(LIBS) -o $@

check: tests/check.o
	$(CXX) $^ $(LIBS) -o $@

.PHONY: clean

clean:
	rm *.o *.gcov *.gcno *.gcda main check \
	rm tests/*.o tests/*.gcov tests/*.gcno tests/*.gcda
