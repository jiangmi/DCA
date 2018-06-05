// Copyright (C) 2009-2016 ETH Zurich
// Copyright (C) 2007?-2016 Center for Nanophase Materials Sciences, ORNL
// All rights reserved.
//
// See LICENSE.txt for terms of usage.
// See CITATION.txt for citation guidelines if you use this code for scientific publications.
//
// Author: Giovanni Balduzzi (gbalduzz@itp.phys.ethz.ch)
//
// This class organizes the MC walker in the CT-INT QMC purely on the CPU.

#ifndef DCA_PHYS_DCA_STEP_CLUSTER_SOLVER_CTINT_WALKER_CTINT_WALKER_CPU_HPP
#define DCA_PHYS_DCA_STEP_CLUSTER_SOLVER_CTINT_WALKER_CTINT_WALKER_CPU_HPP

#include "dca/phys/dca_step/cluster_solver/ctint/walker/ctint_walker_base.hpp"

#include <cassert>
#include <cmath>
#include <stdexcept>
#include <vector>

#include "dca/linalg/linalg.hpp"
#include "dca/phys/dca_step/cluster_solver/ctint/walker/tools/d_matrix_builder.hpp"
#include "dca/phys/dca_step/cluster_solver/ctint/walker/tools/walker_methods.hpp"
#include "dca/phys/dca_step/cluster_solver/ctint/structs/ct_int_configuration.hpp"
#include "dca/phys/dca_step/cluster_solver/ctint/walker/tools/function_proxy.hpp"
#include "dca/phys/dca_step/cluster_solver/ctint/walker/tools/g0_interpolation.hpp"
#include "dca/util/integer_division.hpp"

namespace dca {
namespace phys {
namespace solver {
namespace ctint {
// dca::phys::solver::ctint::

template <class Parameters>
class CtintWalker<linalg::CPU, Parameters> : public CtintWalkerBase<linalg::CPU, Parameters> {
public:
  using this_type = CtintWalker<linalg::CPU, Parameters>;
  using BaseClass = CtintWalkerBase<linalg::CPU, Parameters>;
  using Rng = typename BaseClass::Rng;

public:
  CtintWalker(Parameters& pars_ref, Rng& rng_ref, const InteractionVertices& vertices,
              const DMatrixBuilder<linalg::CPU>& builder_ref, int id = 0);

  void doSweep();

protected:
  bool tryVertexInsert();
  bool tryVertexRemoval();

  // Test handle.
  const auto& getM() const {
    return M_;
  }

private:
  void doStep();

  double insertionProbability(int delta_vertices);

  using Matrix = typename BaseClass::Matrix;
  using MatrixPair = typename BaseClass::MatrixPair;
  using MatrixView = typename linalg::MatrixView<double, linalg::CPU>;

  void applyInsertion(const MatrixPair& S, const MatrixPair& Q, const MatrixPair& R);

  double removalProbability();
  void applyRemoval();

  virtual void smallInverse(const MatrixView& in, MatrixView& out, int s);
  virtual void smallInverse(MatrixView& in_out, int s);
  virtual double separateIndexDeterminant(Matrix& m, const std::vector<ushort>& indices, int s);

protected:
  using BaseClass::parameters_;
  using BaseClass::configuration_;
  using BaseClass::rng_;
  using BaseClass::d_builder_;
  using BaseClass::total_interaction_;
  using BaseClass::beta_;
  using BaseClass::sign_;
  using BaseClass::thread_id_;

  using BaseClass::thermalized_;
  using BaseClass::total_steps_;
  using BaseClass::total_sweeps_;
  using BaseClass::nb_steps_per_sweep_;
  using BaseClass::order_sum_;
  using BaseClass::partial_order_sum_;
  using BaseClass::partial_num_steps_;
  using BaseClass::number_of_annihilations_;
  using BaseClass::number_of_creations_;
  using BaseClass::removal_candidates_;
  using BaseClass::removal_matrix_indices_;

  using BaseClass::M_;
  using BaseClass::det_ratio_;

private:
  std::array<linalg::Matrix<double, linalg::CPU>, 2> S_, Q_, R_;
  // work spaces
  MatrixPair M_Q_;
  Matrix ws_dn_;
  linalg::Vector<double, linalg::CPU> v_work_;
  linalg::Vector<int, linalg::CPU> ipiv_;
};

template <class Parameters>
CtintWalker<linalg::CPU, Parameters>::CtintWalker(Parameters& parameters_ref, Rng& rng_ref,
                                                  const InteractionVertices& vertices,
                                                  const DMatrixBuilder<linalg::CPU>& builder_ref,
                                                  int id)
    : BaseClass(parameters_ref, rng_ref, vertices, builder_ref, id) {}

template < class Parameters>
void CtintWalker<linalg::CPU, Parameters>::doSweep() {
  int nb_of_steps;
  if (not thermalized_)
    nb_of_steps = BaseClass::avgOrder() + 1;
  else
    nb_of_steps = nb_steps_per_sweep_ * parameters_.get_sweeps_per_measurement();

  for (int i = 0; i < nb_of_steps; i++) {
    doStep();
    ++total_steps_;
    order_sum_ += BaseClass::order();
  }
  ++total_sweeps_;

  // Keep tha average after half tantalization for deciding the order.
  if ((not thermalized_) and (total_sweeps_ == parameters_.get_warm_up_sweeps() / 2)) {
    partial_order_sum_ = order_sum_;
    partial_num_steps_ = total_steps_;
  }
}

template <class Parameters>
void CtintWalker<linalg::CPU, Parameters>::doStep() {
  if (int(rng_() * 2)) {
    number_of_creations_ += tryVertexInsert();
  }
  else {
    if (configuration_.size())
      number_of_annihilations_ += tryVertexRemoval();
  }
}

template <class Parameters>
bool CtintWalker<linalg::CPU, Parameters>::tryVertexInsert() {
  configuration_.insertRandom(rng_);
  const int delta_vertices = configuration_.lastInsertionSize();

  // Compute the new pieces of the D(= M^-1) matrix.
  d_builder_.buildSQR(S_, Q_, R_, configuration_);

  const double accept_prob = insertionProbability(delta_vertices);

  const bool accept = std::abs(accept_prob) > rng_();

  if (not accept)
    configuration_.pop(delta_vertices);
  else {
    if (accept_prob < 0)
      sign_ *= -1;
    applyInsertion(S_, Q_, R_);
  }
  return accept;
}

template <class Parameters>
bool CtintWalker<linalg::CPU, Parameters>::tryVertexRemoval() {
  const double accept_prob = removalProbability();
  const bool accept = std::abs(accept_prob) > rng_();

  if (accept) {
    if (accept_prob < 0)
      sign_ *= -1;
    applyRemoval();
  }
  return accept;
}

template <class Parameters>
double CtintWalker<linalg::CPU, Parameters>::insertionProbability(const int delta_vertices) {
  const int old_size = configuration_.size() - delta_vertices;

  for (int s = 0; s < 2; ++s) {
    const auto& Q = Q_[s];
    if (Q.nrCols() == 0) {
      det_ratio_[s] = 1.;
      continue;
    }
    const auto& R = R_[s];
    auto& S = S_[s];
    auto& M = M_[s];

    if (M.nrRows()) {
      auto& M_Q = M_Q_[s];
      M_Q.resizeNoCopy(Q.size());
      linalg::matrixop::gemm(M, Q, M_Q);
      // S <- S_tilde^(-1) = S - R*M*Q
      linalg::matrixop::gemm(-1., R, M_Q, 1., S);
    }

    det_ratio_[s] = details::smallDeterminant(S);
  }

  const int combinatorial_factor =
      delta_vertices == 1 ? old_size + 1
                          : (old_size + 2) * configuration_.occupationNumber(old_size + 1);

  return details::computeAcceptanceProbability(
      delta_vertices, det_ratio_[0] * det_ratio_[1], total_interaction_, beta_,
      configuration_.getStrength(old_size), combinatorial_factor, details::VERTEX_INSERTION);
}

template <class Parameters>
void CtintWalker<linalg::CPU, Parameters>::applyInsertion(const MatrixPair& Sp,
                                                          const MatrixPair& Qp,
                                                          const MatrixPair& Rp) {
  for (int s = 0; s < 2; ++s) {
    const int delta = Qp[s].nrCols();
    if (not delta)
      continue;
    // update M matrix.
    const auto& R = Rp[s];
    const auto S = linalg::makeConstantView(Sp[s]);
    auto& M = M_[s];
    const auto& M_Q = M_Q_[s];
    const int m_size = M.nrCols();

    if (not m_size) {
      M.resizeNoCopy(delta);
      auto M_view = MatrixView(M);
      smallInverse(*S, M_view, s);
      continue;
    }

    auto& R_M = ws_dn_;
    R_M.resizeNoCopy(R.size());
    linalg::matrixop::gemm(R, M, R_M, thread_id_, s);

    M.resize(m_size + delta);
    //  S_tilde = S^-1.
    MatrixView S_tilde(M, m_size, m_size, delta, delta);
    smallInverse(*S, S_tilde, s);

    // R_tilde = - S * R * M
    MatrixView R_tilde(M, m_size, 0, delta, m_size);
    linalg::matrixop::gemm(-1., S_tilde, R_M, double(0.), R_tilde, thread_id_, s);

    // Q_tilde = -M * Q * S
    MatrixView Q_tilde(M, 0, m_size, m_size, delta);
    linalg::matrixop::gemm(-1., M_Q, S_tilde, 0., Q_tilde, thread_id_, s);

    // update bulk: M += M*Q*S*R*M
    MatrixView M_bulk(M, 0, 0, m_size, m_size);
    linalg::matrixop::gemm(-1., Q_tilde, R_M, 1., M_bulk, thread_id_, s);
  }
}

template <class Parameters>
double CtintWalker<linalg::CPU, Parameters>::removalProbability() {
  removal_candidates_ = configuration_.randomRemovalCandidate(rng_);

  const int n = configuration_.size();
  const int n_removed = removal_candidates_.second == -1 ? 1 : 2;

  for (int s = 0; s < 2; ++s) {
    removal_matrix_indices_[s].clear();
    for (int i = 0; i < n_removed; ++i) {
      const int index = i ? removal_candidates_.second : removal_candidates_.first;
      const std::vector<ushort> vertex_indices = configuration_.findIndices(index, s);
      removal_matrix_indices_[s].insert(removal_matrix_indices_[s].end(), vertex_indices.begin(),
                                        vertex_indices.end());
    }
    if (removal_matrix_indices_[s].size() == 0) {
      det_ratio_[s] = 1.;
      continue;
    }
    std::sort(removal_matrix_indices_[s].begin(), removal_matrix_indices_[s].end());
    det_ratio_[s] = separateIndexDeterminant(M_[s], removal_matrix_indices_[s], s);
  }
  assert((removal_matrix_indices_[0].size() + removal_matrix_indices_[1].size()) % 2 == 0);

  const int combinatorial_factor =
      n_removed == 1 ? n : n * configuration_.occupationNumber(removal_candidates_.second);

  return details::computeAcceptanceProbability(n_removed, det_ratio_[0] * det_ratio_[1],
                                               total_interaction_, beta_,
                                               configuration_.getStrength(removal_candidates_.first),
                                               combinatorial_factor, details::VERTEX_REMOVAL);
}

template <class Parameters>
void CtintWalker<linalg::CPU, Parameters>::applyRemoval() {
  const int n_removed = removal_candidates_.second == -1 ? 1 : 2;
  // TODO maybe: don't copy values to be popped.
  BaseClass::pushToEnd(removal_matrix_indices_, removal_candidates_);
  configuration_.pop(n_removed);

  for (int s = 0; s < 2; ++s) {
    auto& M = M_[s];
    const int delta = removal_matrix_indices_[s].size();
    if (delta == 0)  // Nothing to update.
      continue;
    const int m_size = M.nrCols() - delta;
    if (m_size == 0) {  // removing last vertex
      M.resize(0);
      continue;
    }

    MatrixView Q(M, 0, m_size, m_size, delta);
    MatrixView R(M, m_size, 0, delta, m_size);

    MatrixView S(M, m_size, m_size, delta, delta);
    smallInverse(S, s);

    auto& Q_S = M_Q_[s];
    Q_S.resizeNoCopy(Q.size());
    linalg::matrixop::gemm(Q, S, Q_S, thread_id_, s);

    // M -= Q*S^-1*R
    MatrixView M_bulk(M, 0, 0, m_size, m_size);
    linalg::matrixop::gemm(-1., Q_S, R, 1., M_bulk, thread_id_, s);
    M.resize(m_size);
  }
}

template <class Parameters>
void CtintWalker<linalg::CPU, Parameters>::smallInverse(const MatrixView& in, MatrixView& out,
                                                        const int s) {
  details::smallInverse(in, out, det_ratio_[s], ipiv_, v_work_);
}
template <class Parameters>
void CtintWalker<linalg::CPU, Parameters>::smallInverse(MatrixView& in_out, const int s) {
  details::smallInverse(in_out, det_ratio_[s], ipiv_, v_work_);
}

template <class Parameters>
double CtintWalker<linalg::CPU, Parameters>::separateIndexDeterminant(
    Matrix& m, const std::vector<ushort>& indices, int /*s*/) {
  return details::separateIndexDeterminant(m, indices);
}

}  // ctint
}  // solver
}  // phys
}  // dca

#endif  // DCA_PHYS_DCA_STEP_CLUSTER_SOLVER_CTINT_WALKER_CTINT_WALKER_CPU_HPP
