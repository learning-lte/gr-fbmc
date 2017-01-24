/* -*- c++ -*- */
/* 
 * Copyright 2017 <+YOU OR YOUR COMPANY+>.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>

#include "rx_freq_despread_cvc_impl.h"

namespace gr {
  namespace fbmc {

    rx_freq_despread_cvc::sptr
    rx_freq_despread_cvc::make(std::vector<float> taps, int subcarriers, int bands, int payload_bits, float pilot_amplitude, int pilot_timestep, std::vector<int> pilot_carriers)
    {
      return gnuradio::get_initial_sptr
        (new rx_freq_despread_cvc_impl(taps, subcarriers, bands, payload_bits, pilot_amplitude, pilot_timestep, pilot_carriers));
    }

    /*
     * The private constructor
     */
    rx_freq_despread_cvc_impl::rx_freq_despread_cvc_impl(std::vector<float> taps, int subcarriers, int bands, int payload_bits, float pilot_amplitude, int pilot_timestep, std::vector<int> pilot_carriers)
      : gr::block("rx_freq_despread_cvc",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex) * subcarriers * bands)),
        d_prototype_taps(taps), d_subcarriers(subcarriers), d_pilot_amplitude(pilot_amplitude),
        d_pilot_timestep(pilot_timestep), d_payload_bits(payload_bits), d_bands(bands)
    {
      d_frame_len = (int)std::ceil((d_payload_bits-d_subcarriers) / (d_subcarriers - (d_pilot_carriers.size() / d_pilot_timestep))) + 2;

      d_o = (d_prototype_taps.size() + 1) / 2;  // overlap factor
      d_frame_items = d_subcarriers * (d_o * d_bands + (d_frame_len - 1) / 2);
      //std::cout << "d_frame_len = " << d_frame_len << " d_frame_items = " << d_frame_items << std::endl;
      for(int b = 0; b < d_bands; b++) {
        std::for_each(pilot_carriers.begin(), pilot_carriers.end(), [&](int &c) {
          d_pilot_carriers.push_back(c);
          //std::cout << c << std::endl;
          c += d_subcarriers;
        });
      }

      d_fft = new gr::fft::fft_complex(d_subcarriers * d_o * d_bands, true);
      d_G = spreading_matrix();
      /*std::cout << "========= G ==========" << std::endl;
      for(int n = 0; n < d_G.rows(); n++) {
        for(int k = 0; k < d_G.cols(); k++) {
          std::cout << d_G(n, k) << ", ";
        }
        std::cout << std::endl;
      } */
      d_helper = new helper(d_pilot_carriers);
      set_output_multiple(d_frame_len);
    }

    /*
     * Our virtual destructor.
     */
    rx_freq_despread_cvc_impl::~rx_freq_despread_cvc_impl()
    {
      delete d_fft;
      delete d_helper;
    }

    void
    rx_freq_despread_cvc_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {

      /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
      ninput_items_required[0] = d_frame_items;
    }

    Matrixf
    rx_freq_despread_cvc_impl::spreading_matrix() {
      Matrixf result(d_subcarriers* d_bands, d_subcarriers * d_o* d_bands);
      // build first row
      for(unsigned int k = 0; k < d_subcarriers * d_o* d_bands; k++) {
        result(0, k) = 0.0;
      }
      for(unsigned int k = 0; k < d_prototype_taps.size(); k++) {
        if(k < d_prototype_taps.size()/2) {
          result(0, d_subcarriers * d_o* d_bands - d_prototype_taps.size()/2 + k) = d_prototype_taps[k];
        }
        else {
          result(0, k - d_prototype_taps.size()/2) = d_prototype_taps[k];
        }
      }
      int offset = 1;
      for(unsigned int n = 1; n < d_subcarriers * d_bands; n++) {
        for(unsigned int k = 0; k < d_subcarriers * d_o * d_bands; k++) {
          result(n, k) = 0.0;
          if (k >= offset && k < offset+d_prototype_taps.size()) {
            result(n, k) = d_prototype_taps[k-offset];
          }
        }
        offset += d_o;
      }
      return result;
    }

    void
    rx_freq_despread_cvc_impl::write_output(gr_complex* out, int end) {
      for(unsigned int k = 0; k < d_matrix.cols(); k++) {
        for(unsigned int n = 0; n < d_matrix.rows(); n++) {
          if(k*d_matrix.rows()+n >= end) { break ;}
          // TODO phase shift in next block, delete here
          if((k+n) % 2 != 0) {
            out[k * d_matrix.rows() + n] = gr_complex(d_matrix(n, k).imag(), -d_matrix(n, k).real());
          }
          else {
            out[k*d_matrix.rows()+n] = d_matrix(n, k);
          }
        }
      }
    }

    void
    rx_freq_despread_cvc_impl::channel_estimation(Matrixc R) {
      unsigned int K = (R.cols()-2)/d_pilot_timestep + 1; // number of symbols containing pilots
      Matrixc estimate(d_pilot_carriers.size(), K);
      for(unsigned int k = 0; k < K; k++) {
        int i = 0;
        for (std::vector<int>::iterator it = d_pilot_carriers.begin(); it != d_pilot_carriers.end(); ++it) {
          estimate(i, k) = R(*it, k * d_pilot_timestep + 2) / d_pilot_amplitude;  // channel estimation
          //std::cout << estimate(i, k) << ", ";
          i++;
        }
        //std::cout << std::endl;
      }
      d_channel = estimate;
    }

    Matrixc
    rx_freq_despread_cvc_impl::equalize(Matrixc R) {
      // vector of symbol indexes with pilots
      std::vector<int> pilot_times;
      Matrixc R_eq(R.rows(), R.cols());
      for(unsigned int i = 2; i < R.cols(); i += d_pilot_timestep) {
        pilot_times.push_back(i);
      }
      d_helper->set_params(pilot_times, d_channel);
      for(unsigned int k = 0; k < R.cols(); k++) {
        for(unsigned int n = 0; n < R.rows(); n++) {
          R_eq(n, k) = d_helper->get_value(k, n);
          //std::cout << R_eq(n, k) << ", ";
        }
        //std::cout << std::endl;
      }
      //std::cout << "Channel: " << R_eq(0, 2) << std::endl;
      return R.cwiseQuotient(R_eq);
    }

    float
    rx_freq_despread_cvc_impl::fine_freq_sync() {
      std::vector<gr_complex> mean = matrix_mean(d_channel, 0);
      std::vector<float> phase;
      // build phase vector
      for(unsigned int i = 0; i < mean.size(); i++) {
        phase.push_back(d_helper->unwrap(std::arg(mean[i])));
      }
      d_helper->reset_angle();
      float f_o = d_helper->linear_regression(phase)[0];
      f_o /= 2*M_PI*d_subcarriers;
      return f_o;
    }

    float
    rx_freq_despread_cvc_impl::fine_time_sync() {
      std::vector<gr_complex> mean = matrix_mean(d_channel, 1);
      std::vector<float> phase;
      // build phase vector
      for(unsigned int i = 0; i < mean.size(); i++) {
        phase.push_back(d_helper->unwrap(std::arg(mean[i])));
      }
      d_helper->reset_angle();
      float t_o = d_helper->linear_regression(phase)[0];
      t_o /= 2*M_PI;
      return t_o;
    }

    std::vector<gr_complex>
    rx_freq_despread_cvc_impl::matrix_mean(Matrixc matrix, int axis) {
      std::vector<gr_complex> result;
      if(axis == 0) { // rowwise
        for(unsigned int k = 0; k < matrix.cols(); k++) {
          gr_complex mean(0, 0);
          for (unsigned int n = 0; n < matrix.rows(); n++) {
            mean += matrix(n, k);
          }
          mean /= gr_complex(matrix.rows(), 0);
          result.push_back(mean);
        }
      }
      else { // columnwise
        for(unsigned int n = 0; n < matrix.rows(); n++) {
          gr_complex mean(0, 0);
          for (unsigned int k = 0; k < matrix.cols(); k++) {
            mean += matrix(n, k);
          }
          mean /= gr_complex(matrix.cols(), 0);
          result.push_back(mean);
        }
      }
      return result;
    }

    int
    rx_freq_despread_cvc_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];

      // Do <+signal processing+>
      Matrixc R(d_o * d_subcarriers * d_bands, d_frame_len);  // spread receive matrix (freq * time)
      // do symbol wise fft and build matrix
      gr_complex fft_result[d_o * d_subcarriers * d_bands];
      float normalize = std::sqrt(d_subcarriers * d_o * d_bands)/5.0;
      for(unsigned int k = 0; k < d_frame_len; k++) {
        memcpy(d_fft->get_inbuf(), &in[k*d_subcarriers/2], d_o * d_subcarriers * d_bands *sizeof(gr_complex));
        d_fft->execute();
        memcpy(fft_result, d_fft->get_outbuf(), d_o*d_subcarriers* d_bands *sizeof(gr_complex));
        for(unsigned int n = 0; n < d_o * d_subcarriers * d_bands; n++) {
          R(n, k) = fft_result[n] / normalize;
        }
      }

      // despread
      Matrixc curr_data(d_subcarriers * d_bands, d_frame_len);
      d_matrix = curr_data;
      /*std::cout << "========= R ==========" << std::endl;
      for(int n = 0; n < R.rows(); n++) {
        for(int k = 0; k < R.cols(); k++) {
          std::cout << R(n, k) << ", ";
        }
        std::cout << std::endl;
      }
      throw std::runtime_error("Stop; Can't touch this!");*/

      d_matrix = d_G * R;
      // estimate channel with pilots
      channel_estimation(d_matrix);
      // equalize spread matrix
      R = equalize(R);
      // 2nd despread with equalized symbols
      d_matrix = d_G * R;
      // TODO fine freq / timing estimation

      write_output(out, noutput_items * d_subcarriers);

      /* int n = 0;
      for(unsigned int i = 0; i < d_subcarriers * d_frame_len; i++) {
        if(n % d_subcarriers == 0 ) { std::cout << n/d_subcarriers << ": "; }
        std::cout << out[i] << ", ";
        n++;
        if(n % d_subcarriers == 0 ) { std::cout << std::endl; }
      }
      std::cout << "==========================================================" << std::endl; */

      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (d_frame_items);
      //std::cout << "rx_freq_despread: consume " << d_frame_items << " produce " << d_frame_len << std::endl;

      // Tell runtime system how many output items we produced.
      return d_frame_len;  // FIXME floor correct?
    }

  } /* namespace fbmc */
} /* namespace gr */
