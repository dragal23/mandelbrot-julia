// Simple command line prog to generate mandelbrot and julia sets.
// By Gregory Rickeard.
//
// Compile with:
// g++ -O2 -Wall -o mandel-julia mandel-julia.c++ `Magick++-config --cppflags --cxxflags --ldflags --libs`
#include <Magick++.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <stdexcept>
#include <cstring>
#include <tclap/CmdLine.h>
#include <algorithm>
#include <typeinfo>
#include "results.h++"

using namespace std;
using namespace Magick;

string geom, output_name;
bool debug, verbose;
unsigned int max_iter;
double x_min, y_max, x_max, y_min, Cr, Ci;
char which;




void parse_args(int argc, char **argv);
drg::iteration::result *mandelbrot_julia(double x, double y, double a, double b);
vector<drg::iteration::result *> *get_iteration_results(unsigned int width, unsigned int height);
void draw_image(Image &image,
		const unsigned int width,
		const unsigned int height,
		const drg::iteration::colour_mapper &mapper,
		vector<drg::iteration::result *> *results);

  


int main(int argc, char **argv) {
  parse_args(argc, argv);

  // Make our blank image.
  Image image(geom, "black");
  
  unsigned int width  = image.baseColumns();
  unsigned int height = image.baseRows(); 
  
  // Get iteration results...
  vector<drg::iteration::result *> *results = get_iteration_results(width, height);

  // Find some stats form the iteration results to pass to our mapper functor
  drg::iteration::result_statistics stats;
  for_each(results->begin(), results->end(), stats);
  
  // Create the mapper and use it to draw the fractal!
  drg::iteration::colour_mapper mapper(stats.quickest_escape(), stats.slowest_escape());
  draw_image(image, width, height, mapper, results);
  // We no longer need results - so delete them!
  // DRG TODO - need to delete content of results as well...
  delete results;
  
  
  if (verbose)
    cout << stats << endl;
  
  image.write(output_name);
  return 0;
}



vector<drg::iteration::result *> *get_iteration_results(unsigned int width, unsigned int height) {
  vector<drg::iteration::result *> *results = new vector<drg::iteration::result *>(width * height);
  double dx = x_max - x_min;
  double dy = y_max - y_min;
  unsigned int i = 0;
  for (int x = 0 ; x < width ; x++) {
    for (int y = 0 ; y < height ; y++) {
      double x_ = (((double)x * dx) / width) + x_min;
      double y_ = (((double)y * dy) / height) + y_min;
      switch (which) {
      case 'm': (*results)[(x * width) + y] = mandelbrot_julia(0, 0, x_, y_);
                break;
      case 'j': (*results)[(x * width) + y] = mandelbrot_julia(x_, y_, Cr, Ci);
	break;
      }
    }
  }
  return results;
}

void draw_image(Image &image,
		const unsigned int width,
		const unsigned int height,
		const drg::iteration::colour_mapper &mapper,
		vector<drg::iteration::result *> *results) {
  for (int x = 0 ; x < width ; x++) {
    for (int y = 0 ; y < height ; y++) {
      image.pixelColor(x, y, mapper((*results)[(x * width) + y]));
    }
  }
  return;
}
 
 
 
// Returns the number of iterations taken before we iterate beyond the
// escape radus. If we never escape returns 0.
 drg::iteration::result *mandelbrot_julia(double x, double y, double a, double b) {
   double xx, yy;
  for (int i = 1 ; i <= max_iter ; i++) {
    xx = x*x;
    yy = y*y;
    if (x*x + y*y > 4) {
      return new drg::iteration::escaped(i);
    } else {
      y = 2*x*y + b;
      x = xx - yy + a;
    }
  }
  return new drg::iteration::non_terminated(max_iter);
}



// Parses command line options and packs into global variables for use!
void parse_args(int argc, char **argv) {
  // setup argument checking and store results!
  try {
    TCLAP::CmdLine cmd("Generates Mandelbrot and Julia fractals", ' ', "0.1");
    
    TCLAP::ValueArg<std::string> geomArg("g", "geometry", "Geometry of the resultants image, eg 100x100", false, "100x100", "string");
    cmd.add(geomArg);
    
    TCLAP::ValueArg<std::string> outputArg("o", "output", "Name of outputfile", false, "out.gif", "string");
    cmd.add(outputArg);

    TCLAP::ValueArg<double> stepArg("s", "step", "Step size to use whenfollowing rays", false, .01, "double");
    cmd.add(stepArg);

    TCLAP::ValueArg<unsigned int> max_iterArg("i", "iterations", "Maximum number of iterations to test each point on", false, 30, "unsigned integer");
    cmd.add(max_iterArg);

    TCLAP::ValueArg<double> x_minArg("", "x-min", "minimum value of x", false, -2.0, "unsigned integer");
    cmd.add(x_minArg);

    TCLAP::ValueArg<double> y_maxArg("", "y-max", "Maximum value of y", false, 2.0, "unsigned integer");
    cmd.add(y_maxArg);

    TCLAP::ValueArg<double> x_maxArg("", "x-max", "Maximum value of x", false, 2.0, "unsigned integer");
    cmd.add(x_maxArg);

    TCLAP::ValueArg<double> y_minArg("", "y-min", "Minimum value of y ", false, -2.0, "unsigned integer");
    cmd.add(y_minArg);
    

    TCLAP::ValueArg<double> CrArg("", "Cr", "Real part of C. This is only meaningfull with -j", false, 0.0, "unsigned integer");
    cmd.add(CrArg);
    
    TCLAP::ValueArg<double> CiArg("", "Ci", "Imagionary part of C. This is only meaningfull with -j", false, 0.0, "unsigned integer");
    cmd.add(CiArg);
    

    TCLAP::SwitchArg debugSwitch("d", "debug", "If present causes debug information to be displayed! This flag also disbales saving the end image!", cmd, false);
    TCLAP::SwitchArg verboseSwitch("v", "verbose", "Verbose output.", cmd, false);
    TCLAP::SwitchArg juliaSwitch("j", "julia", "Generate a Julia fractal. Cannot be specified with -m.", false);
    TCLAP::SwitchArg mandelSwitch("m", "mandelbrot", "Generate a Mandelbrot fractal. Cannot be specified with -j.", false);
    cmd.xorAdd(juliaSwitch, mandelSwitch);

    cmd.parse(argc, argv);
    
    geom        = geomArg.getValue();
    output_name = outputArg.getValue();
    debug       = debugSwitch.getValue();
    verbose     = verboseSwitch.getValue();
    max_iter    = max_iterArg.getValue();
    x_min       = x_minArg.getValue();
    y_max       = y_maxArg.getValue();
    x_max       = x_maxArg.getValue();
    y_min       = y_minArg.getValue();
    which       = juliaSwitch.getValue() ? 'j' : 'm';
    Cr          = CrArg.getValue();
    Ci          = CiArg.getValue();

  } catch (TCLAP::ArgException &e) {
    cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; 
  }

  if (debug || verbose) {
    switch (which) {
    case 'm': cout << "Generating Mandelbrot..." << endl;
              break;
    case 'j': cout << "Generating Julia with (Cr, Ci) = (" << Cr << ", " << Ci << ")" << endl;
              break;
    }
    cout << "geometry    = " << geom << endl
	 << "output file = " << output_name << endl
	 << "debug       = " << debug << endl
	 << "verbose     = " << verbose << endl
	 << "max_iter    = " << max_iter << endl
	 << "x_min       = " << x_min << endl
	 << "y_max       = " << y_max << endl
	 << "x_max       = " << x_max << endl
	 << "y_min       = " << y_min << endl;
  }
  return;
}
