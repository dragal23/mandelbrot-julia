/// file: results.h++
/// project: mandelbrot-julia
/// author: Gregory Rickeard
/// 
/// Header file containing classes to enable handling of different
/// result types. When generating a Mandelbrot or Julia fractal,
/// iterating on a point in the complex plane will produce one of
/// three results:
/// 1) The point is known to escape (escaped)
/// 2) The poing is known to converge (converged)
/// 3) The point has not be demonstrated to converge or escape (unknown)
///
/// Eash of these result types has different data associated with it.
/// escaped points have the iteration count before the escape happened
/// and the value of Z immediatly after the escape.
/// Converged points have the iteration depth at which the point was
/// known to have converged and the length of the loop the point
/// cycles through. It may also be usefull to store the path that Z
/// passes through.
/// Unknown points have only the number of iterations before we gave
/// up. It may be desirable to store information to determine if all
/// points are within some local though. This would enable generation
/// of heuristics to determine when to continue iteration...


#if !defined __DRG_RESULTS_H
#define __DRG_RESULTS_H

#include <limits>
#include <iostream> // for debugging

namespace drg {
  namespace results {
  
  
  
  
  }
}



#endif // #if !defined __DRG_RESULTS_H
