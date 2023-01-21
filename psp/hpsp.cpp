#include "psp/hpsp.h"
#include "psp/psp_i.h"

namespace psp {
    HPSP::HPSP(Digraph* g, ArcMap* edge_map): _g(g), _edge_map(edge_map),
    node_filter(*g), subgraph(*g, node_filter) {
        j = lemon::countNodes(*g);
        K.resize(j);
        W.resize(j);
        for (Digraph::NodeIt n(*g); n != lemon::INVALID; ++n) {
            node_filter[n] = true;
        }
    }
    void HPSP::run() {
        split(0); // 0 is the the id of the first node
    }

    void HPSP::split(int s) {
        int num_of_children = lemon::countNodes(subgraph);
        double weight_total = 0;
        for (lemon::FilterNodes<Digraph>::ArcIt a(subgraph); a != lemon::INVALID; ++a)
            weight_total += (*_edge_map)[a];
        double gamma_apostrophe = weight_total / (num_of_children - 1.0);
        stl::Partition P_apostrophe;
        double min_value;
        if (num_of_children == 2) {
            goto add_lambda;
        }
        else {
            DilworthTruncation dt(gamma_apostrophe, &subgraph, _edge_map);
            dt.run();
            min_value = dt.get_min_value();
            P_apostrophe = dt.get_min_partition();
        }
        if (!(P_apostrophe.Cardinality() != 1 && P_apostrophe.Cardinality() != num_of_children && min_value < -1 * gamma_apostrophe - _tolerance.epsilon())) {
            add_lambda:
                for (lemon::FilterNodes<Digraph>::NodeIt n(subgraph); n != lemon::INVALID; ++n) {
                    W[subgraph.id(n)] = gamma_apostrophe;
                    K[subgraph.id(n)] = s;
                }
        }
        else {
            // reset node_filter
            for (lemon::FilterNodes<Digraph>::NodeIt n(subgraph); n != lemon::INVALID; ++n) {
                node_filter[n] = false;
            }

            for (const stl::CSet& S : P_apostrophe) {
                if (S.Cardinality() == 1)
                    continue;
                // restrict G to S
                for (std::size_t i: S) {
                    node_filter[_g->nodeFromId(i)] = true;
                }
                split(*S.begin());
                // contract the graph
                contract(S, *S.begin());
            }
            split(s); 
        }
    }
    void HPSP::contract(const stl::CSet& S, int i) {
        // we don't use _g->contract function since it can not handle multiple arcs within two nodes
        std::map<int, double> capacity_map;
        for (Digraph::NodeIt n(*_g); n != lemon::INVALID; ++n)
            capacity_map[_g->id(n)] = 0;
        // iterate over all arcs
        for (Digraph::ArcIt a(*_g); a != lemon::INVALID; ++a) {
            int s_id = _g->id(_g->source(a));
            int t_id = _g->id(_g->target(a));
            if (S.HasElement(s_id) && !S.HasElement(t_id)) {
                capacity_map.at(t_id) += (*_edge_map)[a]; //out is positive
            }
            else if (!S.HasElement(s_id) && S.HasElement(t_id)) {
                capacity_map.at(s_id) += (*_edge_map)[a];
            }
            if (s_id == i || t_id == i) { // delete all arcs connected with Node(i)
                _g->erase(a);
            }
        }
        //delete S\{i} in _g
        for (int j : S) {
            Node J = _g->nodeFromId(j);
            if (_g->valid(J) && j != i)
                _g->erase(J);
        }
        Node _S = _g->nodeFromId(i);

        for (const std::pair<int, double>& val : capacity_map) {
            if (S.HasElement(val.first) || std::abs(val.second) < _tolerance.epsilon())
                continue;
            Arc a;
            if (val.first > i) {
                a = _g->addArc(_S, _g->nodeFromId(val.first));
            }
            else {
                a = _g->addArc(_g->nodeFromId(val.first), _S);
            }
            (*_edge_map)[a] = std::abs(val.second);
        }
    }
    std::vector<double>& HPSP::get_W() {
        return W;
    }

    std::vector<int>& HPSP::get_K() {
        return K;
    }
}