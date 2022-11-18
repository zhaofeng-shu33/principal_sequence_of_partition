#ifdef _MSC_VER
#include <ciso646>
#endif
#include "psp/agglomerative.h"
#include <algorithm> 
#include "psp/graph.h"
#include <IC/AIC> // for exact info-clustering solution



namespace psp {

    class DigraphEntropy : public IC::SF {
    public:
        typedef lemon::ListDigraph Digraph;
        typedef Digraph::Node Node;
        typedef Digraph::Arc Arc;
        typedef typename Digraph::ArcMap<double> ArcMap;
        typedef typename lemon::FilterNodes<Digraph> SubDigraph;
    private:
        Digraph* g;
        ArcMap* edge_map; // Incidence matrix
        int node_num;
        // int edge_num;
    public:
        /**
        Construct a submodular function as the entropy function of a hypergraphical source specified by the incidence matrix.
        @param M Incidence matrix.
        */
        DigraphEntropy(Digraph* _g, ArcMap* _edge_map):
            g(_g), edge_map(_edge_map) {
            node_num = lemon::countNodes(*g);
            // edge_num = lemon::countArcs(*g);
        }


        /**
        Calculate the entropy of a hypergraphical source.
        @param B subvector of elements from the ground set.
        @return Entropy of the component sources indexed by elements in B.
        */
        double operator() (const std::vector<size_t>& B) const {
            size_t n = B.size();
            double ent = 0;
            for (int i = 0; i < n; i++) {
                for (lemon::ListDigraph::InArcIt a(*g, g->nodeFromId(B[i])); a != lemon::INVALID; ++a) {
                    if (g->id(g->source(a)) < B[i] && std::find(B.begin(), B.end(), g->id(g->source(a))) !=B.end() ) {
                        continue;
                    }
                    ent += (*edge_map)[a];
                }
                for (lemon::ListDigraph::OutArcIt a(*g, g->nodeFromId(B[i])); a != lemon::INVALID; ++a) {
                    ent += (*edge_map)[a];
                }
            }
            return ent;
        }

        size_t size() const {
            return node_num;
        }
    };
    Agglomerative_PSP::Agglomerative_PSP(Digraph* g, ArcMap* edge_map):
    _g(g),
    _edge_map(edge_map),
    tree_edge_map(tree) {
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
        psp_list.push_front(p);
    }
    void Agglomerative_PSP::run() {
        DigraphEntropy h(_g, _edge_map);
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