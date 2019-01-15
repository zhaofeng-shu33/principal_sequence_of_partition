// Copyright 2018 Kentaro Minami. All Rights Reserved.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// =============================================================================

#ifndef LINALG_H
#define LINALG_H

#include <vector>
//#include <list>
#include <utility>
#include <limits>
//#include <iostream>

#include <Eigen/Core>
#include <Eigen/LU>
#include <Eigen/QR>
#include <Eigen/Cholesky>

#include "utils.h"

namespace linalg {

//enum LinearSolverStrategy struct { LUStable, QRStable, QRFast };

template <typename T>
Eigen::VectorXd stdvec2vxd(std::size_t n, const std::vector<T>& vec) {
  Eigen::VectorXd vxd = Eigen::VectorXd::Zero(n);
  for (std::size_t i = 0; i < n; ++i) {
    vxd(i) = static_cast<double>(vec[i]);
  }
  return vxd;
}

template <typename T>
Eigen::MatrixXd stdvecvec2mxd(std::size_t n, std::size_t m, const std::vector<T>& mat) {
  Eigen::MatrixXd mxd = Eigen::MatrixXd::Zero(n, m);
  for (std::size_t ic = 0; ic < m; ++ic) {
    for (std::size_t ir = 0; ir < n; ++ ir) {
      mxd(ir, ic) = static_cast<double>(mat[ic][ir]);
    }
  }
  return mxd;
}

// Reduce number of bases
// Preconditions:
// - bases.size() == coef.size() == indices.size() == m
// - bases[0], ..., bases[m-1] are vectors with equal dimension
// - coef[0], ..., coef[m-1] are non-negative, and the sum of coefs equals to 1
std::size_t reduce_bases(std::size_t n, std::vector<std::vector<double>>& bases,
                        std::vector<double>& coef);


std::size_t reduce_bases_with_order_swap(std::size_t n, std::vector<std::vector<double>>& bases,
                        std::vector<double>& coef, std::vector<std::vector<std::size_t>>& orders);

void calc_affine_minimizer(std::size_t n, const std::vector<std::vector<double>>& bases, std::vector<double>& coef,
                          std::vector<double>& solution);

}

#endif
