/* -*- c++ -*- */
/* 
 * Copyright 2014 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_FBMC_COARSE_CFO_CORRECTION_IMPL_H
#define INCLUDED_FBMC_COARSE_CFO_CORRECTION_IMPL_H

#include <fbmc/coarse_cfo_correction.h>
#include <fftw3.h>
#include <boost/circular_buffer.hpp>


namespace gr {
  namespace fbmc {

    class coarse_cfo_correction_impl : public coarse_cfo_correction
    {
     private:
      int d_L; // number of subcarriers
      boost::circular_buffer<int> d_channel_map; // channel map (not fft-shifted)

      int d_interp_fac; // interpolation factor (should be a power of 2)
      int d_nfft; // FFT length
      fftwf_complex* d_buf; // FFTW buffer
      fftwf_plan d_plan; // FFTW plan
      int d_delta; // number of carriers/noncarriers to take into account for the filter window
      std::vector< std::vector<gr_complex> > d_w_u; // upper BP with all possible shifts
      std::vector< std::vector<gr_complex> > d_w_l; // lower BP with all possible shifts
      std::vector<gr_complex> d_shifts; // vector of shift indices
      float d_cfo; // estimated carrier frequency offset
      float d_phi; // continuous phase

      bool d_signal_found; // switch if signal was found

     public:
      coarse_cfo_correction_impl(std::vector<int> channel_map);
      ~coarse_cfo_correction_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace fbmc
} // namespace gr

#endif /* INCLUDED_FBMC_COARSE_CFO_CORRECTION_IMPL_H */

