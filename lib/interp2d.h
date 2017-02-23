//
// Created by senpo on 17.01.17.
//

#ifndef GR_FBMC_INTERP2D_H
#define GR_FBMC_INTERP2D_H

#include <gnuradio/io_signature.h>
#include <cmath>
#include <gsl/gsl_interp2d.h>
#include <gsl/gsl_spline.h>


namespace gr {
  namespace fbmc {
    class interp2d {
    private:
      const gsl_interp2d_type *d_T = gsl_interp2d_bilinear; // liniear interpolation
      std::vector<std::vector<gr_complex> > d_result;

    public:
      interp2d();
      ~interp2d();
      std::vector<std::vector<gr_complex> >* interpolate(int spanx, int spany, std::vector<std::vector<gr_complex> >& pilots);
      std::vector<gr_complex> interp1d(std::vector<int>& pilot_carriers, int span, std::vector<gr_complex>& symbol);
    };
  }
}


#endif //GR_FBMC_INTERP2D_H