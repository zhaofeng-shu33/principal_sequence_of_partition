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

#ifndef ALGORITHMS_BRUTE_FORCE_H
#define ALGORITHMS_BRUTE_FORCE_H

#include <limits>
#include "core/oracle.h"
#include "core/set_utils.h"
#include "core/sfm_algorithm.h"
#include "core/graph.h"
#include <lemon/list_graph.h>

namespace submodular {

template <typename ValueType>
class BruteForce: public SFMAlgorithm<ValueType> {
public:
  using value_type = typename ValueTraits<ValueType>::value_type;

  BruteForce() = default;

  std::string GetName() { return "Brute Force"; }

  void Minimize(SubmodularOracle<ValueType>& F);
  void Minimize(SubmodularOracle<ValueType>* sf, std::vector<ValueType>& xl, ValueType lambda_, lemon::ListGraph* _g, lemon::ListGraph::EdgeMap<ValueType>* _edge_map);
};

template <typename ValueType>
void Minimize(SubmodularOracle<ValueType>* sf, std::vector<ValueType>& xl, ValueType lambda_, lemon::ListGraph* _g, lemon::ListGraph::EdgeMap<ValueType>* _edge_map) {
	int n_ground = xl.size();
	stl::CSet minimizer;

	value_type min_value = std::numeric_limits<value_type>::max();
	stl::CSet fixed;
	fixed.AddElement(n_ground);
	for (unsigned long i = 0; i < (1 << n_ground); ++i) {
		stl::CSet X(n_ground, i);
		// divide it by 2 !		
		value_type new_value = get_cut_value(*_g, *_edge_map, X.Union(fixed)) /2;
		for(int j: X)
			new_value -= xl[j]
		if (new_value < min_value) {
			min_value = new_value;
			minimizer = X;
		}
	}
	min_value -= lambda_;

	Set minimizer_set(n_ground);
	for (int i : minimizer)
		minimizer_set.AddElement(i);
	this->SetResults(min_value, minimizer_set);
}

template <typename ValueType>
void BruteForce<ValueType>::Minimize(SubmodularOracle<ValueType>& F) {
  this->reporter_.SetNames(GetName(), F.GetName());
  this->reporter_.EntryTimer(ReportKind::TOTAL);
  this->reporter_.EntryTimer(ReportKind::ORACLE);
  this->reporter_.EntryCounter(ReportKind::ORACLE);

  this->reporter_.TimerStart(ReportKind::TOTAL);

  value_type min_value = std::numeric_limits<value_type>::max();
  auto n_ground = F.GetNGround();
  Set minimizer(n_ground);

  for (unsigned long i = 0; i < (1 << n_ground); ++i) {
    Set X(n_ground, i);

    auto new_value = F.Call(X, &(this->reporter_));

    if (new_value < min_value) {
      min_value = new_value;
      minimizer = X;
    }
  }

  this->reporter_.TimerStop(ReportKind::TOTAL);
  this->SetResults(min_value, minimizer);
}

}

#endif
