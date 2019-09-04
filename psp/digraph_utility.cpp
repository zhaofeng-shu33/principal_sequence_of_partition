#include "digraph_utility.h"

namespace lemon {

    void digraph_copy(const ListDigraph& oldGraph, const ArcMap& oldArcMap, ListDigraph& G, ArcMap& A) {
        for (int i = 0; i <= oldGraph.maxNodeId(); i++)
            G.addNode();
        for (ListDigraph::NodeIt n(G); n != INVALID; ++n) {
            if (!oldGraph.valid(n))
                G.erase(n);
        }
        for (ListDigraph::ArcIt a(oldGraph); a != INVALID; ++a) {
            Arc a1 = G.addArc(oldGraph.source(a), oldGraph.target(a));
            A[a1] = oldArcMap[a];
        }
    }
    // contract S to i
    void digraph_contract(ListDigraph& G, ArcMap& A, const stl::CSet& S, int i, Tolerance<double> tol) {
        // we don't use _g->contract function since it can not handle multiple arcs within two nodes
        bool is_single = true;
        std::map<int, std::pair<double,double>> capacity_map;
        for (ListDigraph::NodeIt n(G); n != lemon::INVALID; ++n) {
            int j = G.id(n);
            if (S.HasElement(j)) {
                if(j != i)
                    is_single = false;
                continue;
            }
            capacity_map[j] = std::make_pair(0,0);
        }
        if (is_single)
            return;
        // iterate over all arcs
        for (ListDigraph::ArcIt a(G); a != lemon::INVALID; ++a) {
            int s_id = G.id(G.source(a));
            int t_id = G.id(G.target(a));
            if (S.HasElement(s_id) && !S.HasElement(t_id)) {
                capacity_map.at(t_id).first += A[a];  // out is first
            }
            else if (!S.HasElement(s_id) && S.HasElement(t_id)) {
                capacity_map.at(s_id).second += A[a]; // in is second
            }
            if (s_id == i || t_id == i) { // delete all arcs connected with Node(i)
                G.erase(a);
            }
        }
        //delete S\{i} in _g
        for (int j : S) {
            Node J = G.nodeFromId(j);
            if (G.valid(J) && j != i)
                G.erase(J);
        }
        Node _S = G.nodeFromId(i);

        for (const std::pair<int, std::pair<double, double> >& val : capacity_map) {
            Arc a;
            if (val.second.first > tol.epsilon()) {
                a = G.addArc(_S, G.nodeFromId(val.first));
                A[a] = val.second.first;
            }
            else if(std::abs(val.second.second) > tol.epsilon()) {
                a = G.addArc(G.nodeFromId(val.first), _S);
                A[a] = val.second.second;
            }
        }
    }
}
