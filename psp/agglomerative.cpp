#include "psp/agglomerative.h"
#include <IC/AIC> // for exact info-clustering solution
#include <IC/hypergraph> // for hypergraphical source model

namespace psp {
    Agglomerative_PSP::Agglomerative_PSP(Digraph* g, ArcMap* edge_map):
    _g(g),
    _edge_map(edge_map),
    tree_edge_map(tree) {
        node_size = lemon::countNodes(*g);
        int number_of_edges = lemon::countArcs(*g);
        M = Eigen::MatrixXd::Zero(node_size, number_of_edges);
        for (lemon::ListDigraph::ArcIt a(*g); a != lemon::INVALID; ++a) {
            int u = g->id(g->source(a));
            int v = g->id(g->target(a));
            int i = g->id(a);
            M(u, i) = (*_edge_map)[a];
            M(v, i) = (*_edge_map)[a];
        }
    }

    void Agglomerative_PSP::add_partition(const std::vector<std::vector<size_t>>& q) {
        stl::Partition p;
        for (const std::vector<size_t>& set : q) {
            stl::CSet K;
            for (const size_t& e: set) {
                K.AddElement(e);
            }
            p.AddElement(K);
        }
        psp_list.push_back(p);
    }
    void Agglomerative_PSP::run() {
        IC::HypergraphEntropy h(M);
        IC::AIC _psp(h);
        add_partition(_psp.getCoarsestPartition());
        while (_psp.agglomerate()) {
            add_partition(_psp.getCoarsestPartition());
            critical_values.push_front(_psp.getCriticalValues().back());
        }
    }

    std::list<double>& Agglomerative_PSP::get_critical_values() {
        return critical_values;
    }

    std::list<stl::Partition>& Agglomerative_PSP::get_psp() {
        return psp_list;
    }
}