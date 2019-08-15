#include <map>
#include <cmath>
#include <lemon/list_graph.h>
#include <lemon/tolerance.h>
#include "set/set_stl.h"

namespace lemon {
	typedef ListDigraph::ArcMap<double> ArcMap;
	typedef ListDigraph::Node Node;
	typedef ListDigraph::Arc Arc;
	void digraph_copy(const ListDigraph& oldGraph, const ArcMap& oldArcMap, ListDigraph& G, ArcMap& A);
	//! contract node set S to a single node i and remove duplicate arcs, requiring i in S
	void digraph_contract(ListDigraph& G, ArcMap& A, const stl::CSet& S, int i, Tolerance<double> tol = Tolerance<double>());
}
