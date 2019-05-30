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
// use maxflow to solve a special case of submodular function minimization problem, that is the submodular function is (di)graph cut function.
#pragma once
#include <algorithm>
#include <lemon/adaptors.h>
#include <lemon/list_graph.h>
#include <lemon/preflow.h>
#ifdef _DEBUG
#include <lemon/lgf_writer.h>
#include <fstream>
#endif
#include "core/graph/graph.h"
namespace submodular{

// Base class of SFM algorithms
class SFMAlgorithm {
public:
	typedef stl::CSet Set;
	SFMAlgorithm() : done_sfm_(false) {}
	virtual ~SFMAlgorithm() {}

	// Perform SFM algorithm.
	// The minimum value (and a minimizer) should be stored in minimum_value (resp. minimizer_)
	// Some statistics should be reported as stats_.
	// NOTE: SubmodularOracle object F will possibly be modified by the algorithm.
	virtual void Minimize(std::vector<double>& xl, double lambda_, lemon::ListDigraph* _g, lemon::ListDigraph::ArcMap<double>* _edge_map) {}
	virtual std::string GetName() = 0;

	double GetMinimumValue();
	stl::CSet GetMinimizer();

protected:
	bool done_sfm_;
	stl::CSet minimizer_;
	double minimum_value_;
	void SetResults(double minimum_value, const Set& minimizer);
};

class MF: public SFMAlgorithm {
public:
	typedef lemon::ListDigraph Digraph;
	typedef typename Digraph::ArcMap<double> ArcMap;
	typedef typename lemon::FilterNodes<Digraph> SubDigraph;
	typedef typename lemon::PreflowDefaultTraits<SubDigraph, ArcMap> PreflowSubgraphTraits;
	void Minimize(std::vector<double>& xl, double lambda_, Digraph* _g, ArcMap* _edge_map);
    std::string GetName() { return "maximal flow"; }
	private:
		lemon::Tolerance<double> _tolerance;
};
class BruteForce : public SFMAlgorithm {
public:
	typedef lemon::ListDigraph Digraph;
	typedef typename Digraph::ArcMap<double> ArcMap;
	BruteForce() = default;

	std::string GetName() { return "Brute Force"; }

	void Minimize(std::vector<double>& xl, double lambda_, Digraph* _g, ArcMap* _edge_map);
};
}
