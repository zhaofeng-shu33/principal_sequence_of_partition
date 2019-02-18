#include <tuple>
#include <numeric>
#include <lemon/list_graph.h>
#include "core/parametric_preflow.h"
#include <iostream>
#include "core/pmf.h"

double compute_lambda(const std::vector<pair>& parameter_list, const double target_value) {
    // get all breakpoints from parameter_list and sort them from smallest to largest
    std::vector<double> turning_points;
    for (const pair& p : parameter_list) {
        turning_points.push_back((p.first - p.second) / 2);
    }
    std::sort(turning_points.begin(), turning_points.end());
    // compute values at breakpoints
    double sum = std::accumulate(parameter_list.begin(), parameter_list.end(), 0, 
        [](double a, pair b) {return (a + b.second); }
    );
    if (target_value > sum)
        return -1;
    double slope = 0;
    double last_tp = turning_points[0];
    if (target_value == sum)
        return last_tp;
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
namespace submodular {
    Set PMF::get_min_cut_source_side() {
        Set s = Set::MakeEmpty(tilde_G_size);
        for (lemon::ListDigraph::NodeIt n(dig); n != lemon::INVALID; ++n) {
            if (pf.minCut(n))
                s.AddElement(dig.id(n));
        }
        return s;
    }
    void PMF::run() {
        lemon::DigraphCopy<lemon::ListDigraph, lemon::ListDigraph> cg(*g_ptr, dig);
        cg.arcMap(*aM, dig_aM);
        cg.run();
        sink_node = dig.nodeFromId(dig.maxNodeId());
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
        sink_capacity.resize(_y_lambda.size());
        for (lemon::ListDigraph::InArcIt arc(*g_ptr, g_ptr->nodeFromId(tilde_G_size - 2)); arc != lemon::INVALID; ++arc) {
            int i = g_ptr->id(g_ptr->source(arc));
            sink_capacity[i] = (*aM)[arc];
        }
        //find S_0 and T_0
        update_dig(0);
        pf.init();
        pf.startFirstPhase();
        pf.startSecondPhase();
        double minimum_value = pf.flowValue();
        Set S_0 = get_min_cut_source_side();
        Set T_0 = S_0.Complement();
        Set T_1 = Set::MakeEmpty(tilde_G_size);
        T_1.AddElement(tilde_G_size - 2);
        set_list.push_back(T_0);
        set_list.push_back(T_1);
        slice(S_0, T_1);
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
        for (int i = 0; i < tilde_G_size - 2; i++) {
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
        Set T_apostrophe = S_apostrophe.Complement();
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
        double target_value = 0;
        Set T_Complement = T.Complement();
        Set S_Complement = S.Complement();
        for (size_t i : T.GetMembers()) {
            for (lemon::ListDigraph::OutArcIt arc(*g_ptr, g_ptr->nodeFromId(i)); arc != lemon::INVALID; ++arc) {
                if (T_Complement.HasElement(g_ptr->id(g_ptr->target(arc))))
                    target_value += (*aM)[arc];
            }
        }
        for (size_t i : S_Complement.GetMembers()) {
            for (lemon::ListDigraph::InArcIt arc(*g_ptr, g_ptr->nodeFromId(i)); arc != lemon::INVALID; ++arc) {
                if (S.HasElement(g_ptr->id(g_ptr->source(arc))))
                    target_value -= (*aM)[arc];
            }
        }
        return target_value;
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
