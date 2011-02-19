


all: build test

build: mandel-julia

test: results-test

### targets for build
mandel-julia: mandel-julia.c++
	g++ -O2 -Wall -o mandel-julia mandel-julia.c++ `Magick++-config --cppflags --cxxflags --ldflags --libs`


### targets for test
results-test: results-test.c++ results.h++
	g++ -Wall -o results-test results-test.c++

