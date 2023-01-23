#include "psp/hpsp.h"
#include "psp/psp_i.h"

namespace psp {
    HPSP::HPSP(Digraph* g, ArcMap* edge_map): _g(g), _edge_map(edge_map),
        node_filter(*g), subgraph(*g, node_filter) {
        node_size = lemon::countNodes(*g);
        K.resize(node_size);
        W.resize(node_size);
        for (Digraph::NodeIt n(*g); n != lemon::INVALID; ++n) {
            node_filter[n] = true;
        }
    }
    void HPSP::run() {
        split(0); // 0 is the the id of the first node
        psp_construct();
    }
    void HPSP::merge(std::list<int>& C, std::map<int, std::vector<int>>& D) {
        double gamma = -1;
        for(std::list<int>::iterator it=C.begin(); it!=C.end(); it++) {
            if (gamma < W[*it])
                gamma = W[*it];
        }
        std::list<int>::iterator it=C.begin();
        while(it != C.end()) {
            int j = *it;
            std::list<int>::iterator old_it = it;
            it++;
            if (W[j] == gamma) {
                C.erase(old_it);              
            } else {
                continue;
            }
            std::map<int, std::vector<int>>::iterator mit = D.find(j);
            std::map<int, std::vector<int>>::iterator mit_K = D.find(K[j]);
            if (mit != D.end()) {
                std::vector<int> & D_j = mit->second;
                if (K[j] == j)
                    continue;                
                if (mit_K == D.end())
                    D[K[j]] = D_j;
                else {
                    mit_K->second.insert(mit_K->second.end(), D_j.begin(), D_j.end());
                }
                D.erase(mit);
            } else {
                if (mit_K == D.end())
                    D[K[j]] = std::vector<int>({j, K[j]});
                else
                    D[K[j]].push_back(j);
            }
        }
        stl::Partition P;
        stl::CSet _total;
        for(auto it = D.begin(); it != D.end(); it++) {
            std::vector<int>& D_k = it->second;
            stl::CSet s;
            for(auto it_D = D_k.begin(); it_D != D_k.end(); it_D++) {
                _total.AddElement(*it_D);
                s.AddElement(*it_D);
            }
            P.AddElement(s);
        }
        if (_total.Cardinality() < node_size) {
            stl::CSet div_set = stl::CSet::MakeDense(node_size).Difference(_total);
            for (auto it = div_set.begin(); it != div_set.end(); it ++){
                stl::CSet s;
                s.AddElement(*it);
                P.AddElement(s);
            }
        }
        psp_list.push_front(P);
        critical_values.push_front(gamma);
    }
    void HPSP::psp_construct() {
        // n: number of nodes
        // Q = [[set([i]) for i in range(n)]]
        // L = []
        std::list<int> C(node_size);
        int i = 0;
        for(std::list<int>::iterator it=C.begin(); it!=C.end(); it++) {
            *it = i;
            i++;
        }
        std::map<int, std::vector<int>> D;
        while (C.size() > 1) {
            merge(C, D);
        }
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
            for (Digraph::NodeIt n(*_g); n != lemon::INVALID; ++n) {
                node_filter[n] = false;
            }

            for (const stl::CSet& S : P_apostrophe) {
                // restrict G to S
                if (S.Cardinality() == 1)
                    continue;
                for (std::size_t i: S) {
                    node_filter[_g->nodeFromId(i)] = true;
                }
                split(*S.begin());
                for (std::size_t i: S) {
                    node_filter[_g->nodeFromId(i)] = false;
                }
                // contract the graph
                contract(S, *S.begin());
            }
            // reset node_filter
            for (Digraph::NodeIt n(*_g); n != lemon::INVALID; ++n) {
                node_filter[n] = true;
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
    std::list<double>& HPSP::get_critical_values() {
        return critical_values;
    }

    std::list<stl::Partition>& HPSP::get_psp() {
        return psp_list;
    }

}