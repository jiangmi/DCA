// Copyright (C) 2009-2016 ETH Zurich
// Copyright (C) 2007?-2016 Center for Nanophase Materials Sciences, ORNL
// All rights reserved.
//
// See LICENSE.txt for terms of usage.
// See CITATION.txt for citation guidelines if you use this code for scientific publications.
//
// Author: Peter Staar (taa@zurich.ibm.com)
//
// G-matrix tools class.
// It is templated on the device type (CPU|GPU).

#ifndef DCA_PHYS_DCA_STEP_CLUSTER_SOLVER_CTAUX_WALKER_TOOLS_G_MATRIX_TOOLS_G_MATRIX_TOOLS_HPP
#define DCA_PHYS_DCA_STEP_CLUSTER_SOLVER_CTAUX_WALKER_TOOLS_G_MATRIX_TOOLS_G_MATRIX_TOOLS_HPP

#include <cassert>

#include "dca/linalg/device_type.hpp"
#include "dca/linalg/matrix.hpp"
#include "dca/linalg/vector.hpp"

#ifdef DCA_HAVE_CUDA
#include "dca/phys/dca_step/cluster_solver/ctaux/walker/tools/g_matrix_tools/g_matrix_tools_kernels.hpp"
#endif

namespace dca {
namespace phys {
namespace solver {
namespace ctaux {
// dca::phys::solver::ctaux::

// Empty class template
template <dca::linalg::DeviceType device_t, typename parameters_type>
class G_MATRIX_TOOLS {};

// Specialization for CPU
#include "g_matrix_tools_cpu.inc"

#ifdef DCA_HAVE_CUDA
// Specialization for GPU
#include "g_matrix_tools_gpu.inc"
#endif

}  // ctaux
}  // solver
}  // phys
}  // dca

#endif  // DCA_PHYS_DCA_STEP_CLUSTER_SOLVER_CTAUX_WALKER_TOOLS_G_MATRIX_TOOLS_G_MATRIX_TOOLS_HPP
