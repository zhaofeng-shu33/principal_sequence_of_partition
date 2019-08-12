#include <map>
#include <lemon/list_graph.h>
#include <lemon/tolerance.h>
#include "set/set_stl.h"

namespace lemon {
	typedef ListDigraph::ArcMap<double> ArcMap;
	typedef ListDigraph::Node Node;
	typedef ListDigraph::Arc Arc;
	void digraph_copy(const ListDigraph& oldGraph, const ArcMap& oldArcMap, ListDigraph& G, ArcMap& A);
	// contract S to i
	void digraph_contract(ListDigraph& G, ArcMap& A, const stl::CSet& S, int i, Tolerance<double> tol = Tolerance<double>());
}
