#include <tuple>
#include <numeric>
#include <lemon/list_graph.h>
#include <iostream>
#include "core/pmf.h"

double compute_lambda(const std::vector<pair>& parameter_list, const double target_value) {
    // get all breakpoints from parameter_list and sort them from smallest to largest
    std::vector<double> turning_points;
    int infinity_count = 0;
    for (const pair& p : parameter_list) {
        if (p.second != INFINITY)
            turning_points.push_back((p.first - p.second) / 2);
        else
            infinity_count++;
    }
    if (turning_points.size() == 0) {
        double intersept = std::accumulate(parameter_list.begin(), parameter_list.end(), -1*target_value,
            [](double a, pair b) {
                return (a + b.first);
            }
        );
        return intersept / (2 * parameter_list.size());
    }
    std::sort(turning_points.begin(), turning_points.end());
    // compute values at the first breakpoint
    double last_tp = turning_points[0];
    double slope = -infinity_count * 2;

    double sum = 0;
    for (const pair& p : parameter_list)
        sum += std::min(p.first - 2 * last_tp, p.second);
    if (target_value > sum){
        if (slope == 0)
            throw std::range_error("no solution");
        else
            return target_value / (slope);
    }
    
    if (target_value == sum)
        return last_tp;
    // compute values at the other breakpoints
    for (double& tp : turning_points) {
        sum += slope * (tp - last_tp);
        if (sum <= target_value) {
            return (target_value - sum) / slope + tp;
        }
        slope -= 2;
        last_tp = tp;
    }
    return (target_value - sum) / slope + last_tp;
}
namespace parametric {
    using Set = stl::CSet;
    PMF::PMF(lemon::ListDigraph& g, ArcMap& arcMap, std::size_t j, std::vector<pair>& y_lambda) :
        g_ptr(&g), aM(&arcMap), _j(j),
        _y_lambda(y_lambda),
        dig_aM(dig),
        pf(dig, dig_aM, lemon::INVALID, lemon::INVALID) {        
        sink_capacity.resize(_y_lambda.size());
    }
    Set PMF::get_min_cut_source_side() {
        Set s = Set::MakeEmpty(tilde_G_size);
        for (lemon::ListDigraph::NodeIt n(dig); n != lemon::INVALID; ++n) {
            if (pf.minCut(n))
                s.AddElement(dig.id(n));
        }
        return s;
    }
    void PMF::run() {
        //set sink_capacity
        if (_j <= g_ptr->maxNodeId()) {
            for (lemon::ListDigraph::InArcIt arc(*g_ptr, g_ptr->nodeFromId(_j)); arc != lemon::INVALID; ++arc) {
                int i = g_ptr->id(g_ptr->source(arc));
                sink_capacity[i] = (*aM)[arc];
            }
        }
        else {
            throw std::range_error("invalid j is given");
        }
        // copy the graph
        lemon::DigraphCopy<lemon::ListDigraph, lemon::ListDigraph> cg(*g_ptr, dig);
        cg.arcMap(*aM, dig_aM);
        cg.run();

        sink_node = dig.nodeFromId(_j);
        source_node = dig.addNode();
        tilde_G_size = dig.maxNodeId() + 1;
        for (lemon::ListDigraph::NodeIt n(dig); n != lemon::INVALID; ++n) {
            if (n == sink_node || n == source_node)
                continue;
            dig.addArc(source_node, n);
            
        }
        // reset the source_node and sink_node
        pf.source(source_node);
        pf.target(sink_node);

        //find S_0 and T_0
        update_dig(0);
        pf.init();
        pf.startFirstPhase();
        pf.startSecondPhase();
        double minimum_value = pf.flowValue();
        Set S_0 = get_min_cut_source_side();
        Set T_0 = S_0.Complement(tilde_G_size);
        Set T_1 = Set::MakeEmpty(tilde_G_size);
        T_1.AddElement(_j);
        set_list.push_back(T_0);
        set_list.push_back(T_1);
        slice(S_0, T_1);
    }
    void PMF::reset_j(std::size_t j) { 
        _j = j; 
        set_list.clear();
        lambda_list.clear();
    }
    void PMF::update_dig(double lambda) {
        for (lemon::ListDigraph::OutArcIt arc(dig, source_node); arc != lemon::INVALID; ++arc) {
            // get the next node id
            int i = dig.id(dig.target(arc));
            double a_i = _y_lambda[i].first, b_i = _y_lambda[i].second;
            dig_aM[arc] = std::max<double>(0, -std::min<double>(a_i - 2 * lambda, b_i));
        }
        for (lemon::ListDigraph::InArcIt arc(dig, sink_node); arc != lemon::INVALID; ++arc) {
            int i = dig.id(dig.source(arc));
            double a_i = _y_lambda[i].first, b_i = _y_lambda[i].second;
            // get the arc from the original graph
            lemon::ListDigraph::Arc o_arc = g_ptr->arcFromId(dig.id(arc));
            dig_aM[arc] = sink_capacity[i] + std::max<double>(0, std::min<double>(a_i - 2 * lambda, b_i));
        }
    }
    void PMF::insert(double lambda) {
        for (std::list<double>::iterator i = lambda_list.begin(); i != lambda_list.end(); i++) {
            if (*i > lambda) {
                lambda_list.insert(i, lambda);
                return;
            }
        }
        lambda_list.insert(lambda_list.end(), lambda);
    }
    void PMF::insert_set(Set s) {
        for (std::list<Set>::iterator i = set_list.begin(); i != set_list.end(); i++) {
            if (i->IsSubSet(s)) {
                set_list.insert(i, s);
                return;
            }
        }
        set_list.insert(set_list.end(), s);
    }
    void PMF::slice(Set& S, Set& T) {
        // compute lambda_2
        double lambda_const = compute_lambda_eq_const(S, T);
        std::vector<pair> y_lambda_filter;
        for (int i = 0; i < tilde_G_size - 1; i++) {
            if (S.HasElement(i) || T.HasElement(i))
                continue;
            y_lambda_filter.push_back(_y_lambda[i]);
        }
        double lambda_2 = compute_lambda(y_lambda_filter, -lambda_const);
        update_dig(lambda_2);
        // do not use graph contraction
        pf.init();
        pf.startFirstPhase();
        pf.startSecondPhase();
        Set S_apostrophe = get_min_cut_source_side();
        Set T_apostrophe = S_apostrophe.Complement(tilde_G_size);
        if(S_apostrophe != S && T_apostrophe != T){
            // if no graph contraction, S \subseteq S_apostrophe and T \subseteq T_apostrophe
            Set S_Union = S.Union(S_apostrophe);
            Set T_Union = T.Union(T_apostrophe);
            insert_set(T_Union);
            slice(S, T_Union);
            slice(S_Union, T);
        }
        else {
            insert(lambda_2);
        }
    }
    double PMF::compute_lambda_eq_const(Set& S, Set& T) {
        // compute the target value from original graph
        double target_value = compute_cut(*g_ptr, *aM, T);
        target_value -= compute_cut(*g_ptr, *aM, S);
        return target_value;
    }
    PDT make_pdt(std::size_t num_nodes, std::vector<std::tuple<std::size_t, std::size_t, double>>& edges) {
        lemon::ListDigraph g;
        g.reserveNode(num_nodes);
        for(int i = 0; i < num_nodes; i++)
            g.addNode();
        lemon::ListDigraph::ArcMap<double> aM(g);
        for(std::tuple<std::size_t, std::size_t, double>& edge_tuple : edges){
            lemon::ListDigraph::Arc a1 = g.addArc(g.nodeFromId(std::get<0>(edge_tuple)), 
                g.nodeFromId(std::get<1>(edge_tuple)));
            aM[a1] = std::get<2>(edge_tuple);
        }
        PDT pdt(g, aM);
        return pdt;
    }
    PDT::PDT(lemon::ListDigraph& g, ArcMap& arcMap):
        _g(&g),
        _arcMap(&arcMap),
        pmf(g, arcMap, 0, _y_lambda){
        _y_lambda.resize(g.maxNodeId()+1, pair(0, INFINITY));
        partition_list.push_back(Partition());
        Lambda_list.push_back(INFINITY);
    }
    void PDT::run() {
        for (int j = 0; j < _y_lambda.size(); j++) {
            pmf.reset_y_lambda(_y_lambda);
            pmf.reset_j(j);
            pmf.run();
            std::list<Set> s_list = pmf.get_set_list();
            std::list<double> lambda_list = pmf.get_lambda_list();
            for (int u = 0; u < _y_lambda.size(); u++) {
                if (u == j) {
                    stl::CSet s;
                    s.AddElement(j);
                    _y_lambda[j] = pair(compute_cut(*_g, *_arcMap, s), INFINITY);
                }
                else {
                    int i = -1;
                    std::list<double>::iterator lambda_it = lambda_list.begin();
                    for (Set& s : s_list) {
                        if (s.HasElement(u)) {
                            i++;
                            lambda_it++;
                        }
                    }
                    if (i != -1) {
                        lambda_it--;
                        if (*lambda_it > (_y_lambda[u].first - _y_lambda[u].second) / 2)
                            _y_lambda[u] = std::make_pair(_y_lambda[u].first, _y_lambda[u].first - 2 * (*lambda_it));
                    }
                }
            }
            std::list<Partition> partition_list_apostrophe;
            std::list<double> Lambda_list_apostrophe;
            lambda_list.push_back(INFINITY);
            int i = 0, k = 0;
            std::list<Partition>::iterator p_it = partition_list.begin();
            std::list<Set>::iterator s_it = s_list.begin();
            std::list<double>::iterator d_i = Lambda_list.begin(), d_k = lambda_list.begin();
            Partition Q;
            while (i < Lambda_list.size() && k < lambda_list.size()) {
                Partition Q_apostrophe = p_it->expand(*s_it);
                if (Q_apostrophe != Q) {
                    Q = Q_apostrophe;
                    partition_list_apostrophe.push_back(Q);
                    Lambda_list_apostrophe.push_back(std::min(*d_i, *d_k));
                }
                else {
                    std::reverse_iterator<std::list<double>::iterator> last_it = Lambda_list_apostrophe.rbegin();
                    *last_it = std::min(*d_i, *d_k);
                }
                if (*d_i < *d_k) {
                    i++;
                    d_i++;
                    p_it++;
                }
                else if (*d_i >= *d_k) {
                    k++;
                    d_k++;
                    s_it++;
                }
                else {
                    i++;
                    d_i++;
                    p_it++;
                }
            }
            Lambda_list = Lambda_list_apostrophe;
            partition_list = partition_list_apostrophe;
        }
    }
}
int remain(){
    // set elevator and flow maps before running the algorithm.
    lemon::ListDigraph g;
    std::vector<lemon::ListDigraph::Node> node_list;
    int graph_size = 4;
    //sink node id is graph_size, source node id is graph_size+1
    g.reserveNode(4);
    g.reserveArc(5);
    lemon::ListDigraph::Node source_node = g.addNode();
    lemon::ListDigraph::Node sink_node = g.addNode();
    lemon::ListDigraph::Node node_1 = g.addNode();
    lemon::ListDigraph::Node node_2 = g.addNode();
    // construct edge cost map
    typedef lemon::ListDigraph::ArcMap<double> ArcMap;
    ArcMap edge_cost_map(g);
    lemon::ListDigraph::Arc arc1 = g.addArc(source_node, node_1);
    edge_cost_map[arc1] = 2;
    lemon::ListDigraph::Arc arc2 = g.addArc(node_1, sink_node);
    edge_cost_map[arc2] = 1;
    lemon::ListDigraph::Arc arc3 = g.addArc(source_node, node_2);
    edge_cost_map[arc3] = 4;
    lemon::ListDigraph::Arc arc4 = g.addArc(node_2, sink_node);
    edge_cost_map[arc4] = 5;
    lemon::ListDigraph::Arc arc5 = g.addArc(node_1, node_2);
    edge_cost_map[arc5] = 3;

    lemon::Preflow<lemon::ListDigraph, ArcMap> pf(g, edge_cost_map, source_node, sink_node);
    lemon::Preflow<lemon::ListDigraph, ArcMap>::FlowMap flowMap(g);
    flowMap[arc1] = 2;
    flowMap[arc2] = 1;
    flowMap[arc3] = 4;
    flowMap[arc4] = 4;
    flowMap[arc5] = 1;
    pf.flowMap(flowMap);

    pf.init();
    pf.startFirstPhase();
    pf.startSecondPhase();
    double minimum_value = pf.flowValue();
    std::cout << minimum_value << std::endl;
    std::cout << flowMap[arc1] << std::endl;
    std::cout << flowMap[arc2] << std::endl;
    std::cout << flowMap[arc3] << std::endl;
    std::cout << flowMap[arc4] << std::endl;
    std::cout << flowMap[arc5] << std::endl;

    edge_cost_map[arc4] -= 1;
    flowMap[arc4] -= 1;
    pf.updateExcess();
    pf.startFirstPhase();
    pf.startSecondPhase();
    std::cout << "Second min value: " << pf.flowValue() << std::endl;
    return 0;
}
