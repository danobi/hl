all: hl

hl: hl.cc
	$(CXX) -o hl hl.cc -Wall -W -pedantic -std=c++11 -g

clean:
	rm hl
