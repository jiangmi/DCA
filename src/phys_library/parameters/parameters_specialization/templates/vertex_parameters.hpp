// Copyright (C) 2009-2016 ETH Zurich
// Copyright (C) 2007?-2016 Center for Nanophase Materials Sciences, ORNL
// All rights reserved.
//
// See LICENSE.txt for terms of usage.
// See CITATION.txt for citation guidelines if you use this code for scientific publications.
//
// Author: Peter Staar (peter.w.j.staar@gmail.com)
//         Urs R. Haehner (haehneru@itp.phys.ethz.ch)
//
// This class contains all possible parameters that define the two-particle Greens-function and the
// analysis of it.
//
// TODO: - Clean-up.
//       - Make this file self-contained.

#ifndef PHYS_LIBRARY_PARAMETERS_PARAMETERS_SPECIALIZATION_TEMPLATES_VERTEX_PARAMETERS_HPP
#define PHYS_LIBRARY_PARAMETERS_PARAMETERS_SPECIALIZATION_TEMPLATES_VERTEX_PARAMETERS_HPP

#include <stdexcept>
#include <vector>
#include <iostream>
#include <string>
#include "phys_library/domain_types.hpp"
using namespace types;

class vertex_parameters {

public:
  vertex_parameters();

  /******************************************
   ***        CONCURRENCY                 ***
   ******************************************/

  template <class concurrency_type>
  int get_buffer_size(concurrency_type& concurrency);

  template <class concurrency_type>
  void pack(concurrency_type& concurrency, int* buffer, int buffer_size, int& position);

  template <class concurrency_type>
  void unpack(concurrency_type& concurrency, int* buffer, int buffer_size, int& position);

  /******************************************
   ***        READ/WRITE                  ***
   ******************************************/
  template <class read_write_type>
  void read_write(read_write_type& read_write_obj);

  /******************************************
   ***        DATA                        ***
   ******************************************/

  vertex_measurement_type get_vertex_measurement_type();

  // This function returns the 'exact' q-channel vector, i.e. the k-cluster vector whose distance
  // (L2 norm) to the input q-channel vector is minimal.
  // It assumes that the input q-channel vector's distance to the next k-cluster vector is smaller
  // than 10^-3.
  const std::vector<double>& get_q_channel_vec();
  // Returns the index of the 'exact' q-channel vector.
  const int& get_q_channel_ind();
  int get_w_channel();

  bool compute_chi_0();
  bool compute_chi();

  bool compute_eigenvalues();
  bool compute_P_q_cluster();
  bool compute_P_q_lattice();

  bool do_deconvolution_of_Gamma();

  double get_singular_value_cut_off();
  int get_singular_value_index_cut_off();

  bool do_symmetrization_of_Gamma();

  bool do_diagonolization_on_folded_Gamma_chi_0();
  double get_BSE_cut_off_radius();

private:
  std::string vertex_measurement_type_str;

  std::vector<double> q_channel_vec_input;
  int w_channel;

  double singular_value_cut_off;
  int singular_value_index_cut_off;

  std::string diagonolize_folded_Gamma_chi_0;
  double BSE_cut_off_radius;

  std::string do_deconvolution_of_Gamma_str;

  std::string do_symmetrization_of_Gamma_str;

  std::string compute_chi_0_str;
  std::string compute_chi_str;

  std::string compute_eigenvalues_str;
  std::string compute_P_q_cluster_str;
  std::string compute_P_q_lattice_str;
};

vertex_parameters::vertex_parameters()
    : vertex_measurement_type_str("NONE"),

      q_channel_vec_input(model::DIMENSION, 0),
      w_channel(0),

      singular_value_cut_off(0.5),
      singular_value_index_cut_off(128),

      diagonolize_folded_Gamma_chi_0("false"),
      BSE_cut_off_radius(10.),

      do_deconvolution_of_Gamma_str("yes"),

      do_symmetrization_of_Gamma_str("yes"),

      compute_chi_0_str("no"),
      compute_chi_str("no"),

      compute_eigenvalues_str("yes"),
      compute_P_q_cluster_str("no"),
      compute_P_q_lattice_str("no") {}

/******************************************
 ***        CONCURRENCY                 ***
 ******************************************/

template <class concurrency_type>
int vertex_parameters::get_buffer_size(concurrency_type& concurrency) {
  int buffer_size = 0;

  buffer_size += concurrency.get_buffer_size(vertex_measurement_type_str);

  buffer_size += concurrency.get_buffer_size(q_channel_vec_input);
  buffer_size += concurrency.get_buffer_size(w_channel);

  buffer_size += concurrency.get_buffer_size(singular_value_cut_off);
  buffer_size += concurrency.get_buffer_size(singular_value_index_cut_off);

  buffer_size += concurrency.get_buffer_size(diagonolize_folded_Gamma_chi_0);
  buffer_size += concurrency.get_buffer_size(BSE_cut_off_radius);

  buffer_size += concurrency.get_buffer_size(do_deconvolution_of_Gamma_str);

  buffer_size += concurrency.get_buffer_size(do_symmetrization_of_Gamma_str);

  buffer_size += concurrency.get_buffer_size(compute_chi_0_str);
  buffer_size += concurrency.get_buffer_size(compute_chi_str);

  buffer_size += concurrency.get_buffer_size(compute_eigenvalues_str);
  buffer_size += concurrency.get_buffer_size(compute_P_q_cluster_str);
  buffer_size += concurrency.get_buffer_size(compute_P_q_lattice_str);

  return buffer_size;
}

template <class concurrency_type>
void vertex_parameters::pack(concurrency_type& concurrency, int* buffer, int buffer_size,
                             int& position) {
  concurrency.pack(buffer, buffer_size, position, vertex_measurement_type_str);

  concurrency.pack(buffer, buffer_size, position, q_channel_vec_input);
  concurrency.pack(buffer, buffer_size, position, w_channel);

  concurrency.pack(buffer, buffer_size, position, singular_value_cut_off);
  concurrency.pack(buffer, buffer_size, position, singular_value_index_cut_off);

  concurrency.pack(buffer, buffer_size, position, diagonolize_folded_Gamma_chi_0);
  concurrency.pack(buffer, buffer_size, position, BSE_cut_off_radius);

  concurrency.pack(buffer, buffer_size, position, do_deconvolution_of_Gamma_str);

  concurrency.pack(buffer, buffer_size, position, do_symmetrization_of_Gamma_str);

  concurrency.pack(buffer, buffer_size, position, compute_chi_0_str);
  concurrency.pack(buffer, buffer_size, position, compute_chi_str);

  concurrency.pack(buffer, buffer_size, position, compute_eigenvalues_str);
  concurrency.pack(buffer, buffer_size, position, compute_P_q_cluster_str);
  concurrency.pack(buffer, buffer_size, position, compute_P_q_lattice_str);
}

template <class concurrency_type>
void vertex_parameters::unpack(concurrency_type& concurrency, int* buffer, int buffer_size,
                               int& position) {
  concurrency.unpack(buffer, buffer_size, position, vertex_measurement_type_str);

  concurrency.unpack(buffer, buffer_size, position, q_channel_vec_input);
  concurrency.unpack(buffer, buffer_size, position, w_channel);

  concurrency.unpack(buffer, buffer_size, position, singular_value_cut_off);
  concurrency.unpack(buffer, buffer_size, position, singular_value_index_cut_off);

  concurrency.unpack(buffer, buffer_size, position, diagonolize_folded_Gamma_chi_0);
  concurrency.unpack(buffer, buffer_size, position, BSE_cut_off_radius);

  concurrency.unpack(buffer, buffer_size, position, do_deconvolution_of_Gamma_str);

  concurrency.unpack(buffer, buffer_size, position, do_symmetrization_of_Gamma_str);

  concurrency.unpack(buffer, buffer_size, position, compute_chi_0_str);
  concurrency.unpack(buffer, buffer_size, position, compute_chi_str);

  concurrency.unpack(buffer, buffer_size, position, compute_eigenvalues_str);
  concurrency.unpack(buffer, buffer_size, position, compute_P_q_cluster_str);
  concurrency.unpack(buffer, buffer_size, position, compute_P_q_lattice_str);
}

/******************************************
 ***        READ/WRITE                  ***
 ******************************************/

template <class read_write_type>
void vertex_parameters::read_write(read_write_type& read_write_obj) {
  try {
    read_write_obj.open_group("vertex-channel");

    try {
      read_write_obj.execute("vertex-measurement-type", vertex_measurement_type_str);
    }
    catch (const std::exception& r_e) {
    }

    try {
      read_write_obj.execute("q-channel", q_channel_vec_input);
    }
    catch (const std::exception& r_e) {
    }
    try {
      read_write_obj.execute("w-channel", w_channel);
    }
    catch (const std::exception& r_e) {
    }

    try {
      read_write_obj.execute("deconvolute-Gamma", do_deconvolution_of_Gamma_str);
    }
    catch (const std::exception& r_e) {
    }
    try {
      read_write_obj.execute("symmetrize-Gamma", do_symmetrization_of_Gamma_str);
    }
    catch (const std::exception& r_e) {
    }

    {
      read_write_obj.open_group("lattice-mapping");

      try {
        read_write_obj.execute("singular-value-sigma-cut-off", singular_value_cut_off);
      }
      catch (const std::exception& r_e) {
      }
      try {
        read_write_obj.execute("singular-value-index-cut-off", singular_value_index_cut_off);
      }
      catch (const std::exception& r_e) {
      }

      read_write_obj.close_group();
    }

    {
      read_write_obj.open_group("lattice-solver");

      try {
        read_write_obj.execute("diagonolize-folded-Gamma-chi_0", diagonolize_folded_Gamma_chi_0);
      }
      catch (const std::exception& r_e) {
      }
      try {
        read_write_obj.execute("BSE-cut-off-radius", BSE_cut_off_radius);
      }
      catch (const std::exception& r_e) {
      }

      read_write_obj.close_group();
    }

    {
      read_write_obj.open_group("options");

      try {
        read_write_obj.execute("compute-chi", compute_chi_str);
      }
      catch (const std::exception& r_e) {
      }
      try {
        read_write_obj.execute("compute-chi_0", compute_chi_0_str);
      }
      catch (const std::exception& r_e) {
      }

      try {
        read_write_obj.execute("compute-eigenvalues", compute_eigenvalues_str);
      }
      catch (const std::exception& r_e) {
      }
      try {
        read_write_obj.execute("compute-Pq-cluster", compute_P_q_cluster_str);
      }
      catch (const std::exception& r_e) {
      }
      try {
        read_write_obj.execute("compute-Pq-lattice", compute_P_q_lattice_str);
      }
      catch (const std::exception& r_e) {
      }

      read_write_obj.close_group();
    }

    read_write_obj.close_group();
  }
  catch (const std::exception& r_e) {
    std::cout << "\n\t vertex-parameters defined !!  \n\n";
    throw std::logic_error(__PRETTY_FUNCTION__);
  }
}

/******************************************
 ***        DATA                        ***
 ******************************************/

vertex_measurement_type vertex_parameters::get_vertex_measurement_type() {
  if (vertex_measurement_type_str == "PARTICLE_HOLE_TRANSVERSE")
    return PARTICLE_HOLE_TRANSVERSE;

  if (vertex_measurement_type_str == "PARTICLE_HOLE_MAGNETIC")
    return PARTICLE_HOLE_MAGNETIC;

  if (vertex_measurement_type_str == "PARTICLE_HOLE_CHARGE")
    return PARTICLE_HOLE_CHARGE;

  if (vertex_measurement_type_str == "PARTICLE_PARTICLE_SUPERCONDUCTING")
    return PARTICLE_PARTICLE_SUPERCONDUCTING;

  if (vertex_measurement_type_str == "NONE")
    return NONE;

  std::cout << vertex_measurement_type_str << " is not of the type : "
            << "PARTICLE_HOLE_TRANSVERSE"
            << ", "
            << "PARTICLE_HOLE__MAGNETIC"
            << ", "
            << "PARTICLE_HOLE_CHARGE"
            << ", "
            << "PARTICLE_PARTICLE_SUPERCONDUCTING"
            << ", "
            << "NONE"
            << ", "
            << "\n";

  throw std::logic_error(__FUNCTION__);
}

const int& vertex_parameters::get_q_channel_ind() {
  static const int q_channel_ind(cluster_operations::index(
      get_q_channel_vec(), DCA_k_cluster_type::get_elements(), DCA_k_cluster_type::SHAPE));

  return q_channel_ind;
}

const std::vector<double>& vertex_parameters::get_q_channel_vec() {
  static const std::vector<double> q_channel_vec(cluster_operations::find_closest_cluster_vector(
      q_channel_vec_input, DCA_k_cluster_type::get_elements(),
      DCA_k_cluster_type::get_super_basis_vectors(), 1.e-3));

  return q_channel_vec;
}

int vertex_parameters::get_w_channel() {
  return w_channel;
}

double vertex_parameters::get_singular_value_cut_off() {
  return singular_value_cut_off;
}

int vertex_parameters::get_singular_value_index_cut_off() {
  return singular_value_index_cut_off;
}

double vertex_parameters::get_BSE_cut_off_radius() {
  return BSE_cut_off_radius;
}

bool vertex_parameters::do_diagonolization_on_folded_Gamma_chi_0() {
  if (diagonolize_folded_Gamma_chi_0 == "true")
    return true;

  if (diagonolize_folded_Gamma_chi_0 == "false")
    return false;

  throw std::logic_error(__FUNCTION__);
}

bool vertex_parameters::do_deconvolution_of_Gamma() {
  if (do_deconvolution_of_Gamma_str == "yes")
    return true;

  if (do_deconvolution_of_Gamma_str == "no")
    return false;

  throw std::logic_error(__FUNCTION__);
}

bool vertex_parameters::do_symmetrization_of_Gamma() {
  if (do_symmetrization_of_Gamma_str == "yes")
    return true;

  if (do_symmetrization_of_Gamma_str == "no")
    return false;

  throw std::logic_error(__FUNCTION__);
}

bool vertex_parameters::compute_chi_0() {
  if (compute_chi_0_str == "yes")
    return true;

  if (compute_chi_0_str == "no")
    return false;

  throw std::logic_error(__FUNCTION__);
}

bool vertex_parameters::compute_chi() {
  if (compute_chi_str == "yes")
    return true;

  if (compute_chi_str == "no")
    return false;

  throw std::logic_error(__FUNCTION__);
}

bool vertex_parameters::compute_eigenvalues() {
  if (compute_eigenvalues_str == "yes")
    return true;

  if (compute_eigenvalues_str == "no")
    return false;

  throw std::logic_error(__FUNCTION__);
}

bool vertex_parameters::compute_P_q_cluster() {
  if (compute_P_q_cluster_str == "yes")
    return true;

  if (compute_P_q_cluster_str == "no")
    return false;

  throw std::logic_error(__FUNCTION__);
}

bool vertex_parameters::compute_P_q_lattice() {
  if (compute_P_q_lattice_str == "yes")
    return true;

  if (compute_P_q_lattice_str == "no")
    return false;

  throw std::logic_error(__FUNCTION__);
}

#endif  // PHYS_LIBRARY_PARAMETERS_PARAMETERS_SPECIALIZATION_TEMPLATES_VERTEX_PARAMETERS_HPP