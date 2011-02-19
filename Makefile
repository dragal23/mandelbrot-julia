
all: mandel-julia

mandel-julia: mandel-julia.c++
	g++ -O2 -Wall -o mandel-julia mandel-julia.c++ `Magick++-config --cppflags --cxxflags --ldflags --libs`