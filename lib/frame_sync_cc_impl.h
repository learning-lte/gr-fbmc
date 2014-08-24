/* -*- c++ -*- */
/* 
 * Copyright 2014 Communications Engineering Lab (CEL), Karlsruhe Institute of Technology (KIT).
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

#ifndef INCLUDED_FBMC_frame_sync_cc_IMPL_H
#define INCLUDED_FBMC_frame_sync_cc_IMPL_H

#include <fbmc/frame_sync_cc.h>
#include <boost/circular_buffer.hpp>
#include <cstdio>
 
namespace gr {
  namespace fbmc {

    unsigned const FRAME_SYNC_ACQUISITION = 0;
    unsigned const FRAME_SYNC_PRETRACKING = 1;
    unsigned const FRAME_SYNC_TRACKING = 2;
    unsigned const FRAME_SYNC_VALIDATION = 3;

    class frame_sync_cc_impl : public frame_sync_cc {
     private:
      int d_L; // number of subcarriers
      int d_frame_len; // number of samples per frame (including preamble)
      int d_fixed_lag_lookahead; // lookahead for fixed lag correlation
      std::vector<gr_complex> d_preamble_sym; // reference preamble symbol
      gr_complex d_preamble_energy; // energy of the preamble signal
      int d_step_size; // number of samples to proceed with every step
      float d_threshold; // threshold for the correlation
      unsigned int d_state; // synchronization state
      int d_num_consec_frames; // number of consecutive frames detected
      int d_sample_ctr; // number of samples of the current frame that already have been written
      int d_pretracking_window; // time window for which the pretracking is performed before going back to acquisition
      int d_pretracking_ctr; // counter for determining the position in the pretracking window
      boost::circular_buffer<gr_complex> d_pretracking_buf; // buffer for incoming samplesl during pretracking

      float d_f_off; // estimated frequency offset
      float d_phi; // phase; updated in every step to avoid phase discontinuities

      gr_complex fixed_lag_corr(gr_complex *x); // fixed lag correlation
      gr_complex ref_corr(gr_complex *x); // reference correlation
      float estimate_cfo(gr_complex corr_val); // estimate carrier frequency offset
      std::string print_state();

      FILE* dbg_fp;
      FILE* dbg_fp2;
      FILE* dbg_fp3;

     public:
      frame_sync_cc_impl(int L, int frame_len, std::vector<gr_complex> preamble_sym, int step_size, float threshold);
      ~frame_sync_cc_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace fbmc
} // namespace gr

#endif /* INCLUDED_FBMC_frame_sync_cc_IMPL_H */

