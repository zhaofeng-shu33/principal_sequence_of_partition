#include <tuple>
#include <numeric>
#include <lemon/list_graph.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include "psp/graph.h"
#include "psp/pmf.h"
namespace parametric {
    using Set = stl::CSet;
    double PMF::compute_lambda(const std::vector<pair>& parameter_list, const double target_value) {
        // get all breakpoints from parameter_list and sort them from smallest to largest
        std::vector<double> turning_points;
        int infinity_count = 0;
        for (const pair& p : parameter_list) {
            if (p.second != INFINITY)
                turning_points.push_back(p.first - p.second);
            else
                infinity_count++;
        }
        if (turning_points.size() == 0) {
            double intersept = std::accumulate(parameter_list.begin(), parameter_list.end(), -1 * target_value,
                [](double a, pair b) {
                return (a + b.first);
            }
            );
            return intersept / parameter_list.size();
        }
        std::sort(turning_points.begin(), turning_points.end());
        // compute values at the first breakpoint
        double last_tp = turning_points[0];
        int slope = -infinity_count ;

        double sum = 0;
        double intersept = 0;
        for (const pair& p : parameter_list) {
            sum += std::min(p.first - last_tp, p.second);
            if (p.second == INFINITY)
                intersept += p.first;
            else
                intersept += p.second;
        }
        if (tolerance.less(sum, target_value)) {
            if (slope == 0)
                throw std::range_error("no solution");
            else
                return (target_value - intersept) / (slope);
        }

        if (!tolerance.different(target_value, sum)) {
            return last_tp;
        }
        // compute values at the other breakpoints
        for (double& tp : turning_points) {
            sum += slope * (tp - last_tp);
            if (sum <= target_value) {
                return (target_value - sum) / slope + tp;
            }
            slope -= 1;
            last_tp = tp;
        }
        return (target_value - sum) / slope + last_tp;
    }

    PMF::PMF(lemon::ListDigraph* g, ArcMap* arcMap, std::size_t j, std::vector<pair>& y_lambda) :
        g_ptr(g), sub_digraph(*g_ptr, node_filter), node_filter(*g_ptr), aM(arcMap), 
        dig_aM(dig), _y_lambda(y_lambda),
        tilde_G_size(-1), _j(j){
        sink_capacity.resize(_y_lambda.size());
        node_filter[g_ptr->nodeFromId(0)] = true;
    }

    void PMF::set_node_filter(bool value) {
        for (lemon::ListDigraph::NodeIt n(*g_ptr); n != lemon::INVALID; ++n) {
            node_filter[n] = value;
        }
        node_filter[g_ptr->nodeFromId(0)] = true;
    }

    Set PMF::get_min_cut_sink_side(Preflow& pf) {
        Set t = Set::MakeEmpty(tilde_G_size);
        for (lemon::ListDigraph::NodeIt n(dig); n != lemon::INVALID; ++n) {
            if (!pf.minCut(n))
                t.AddElement(dig.id(n));
        }
        return t;
    }
    
    void PMF::run() {
        //set sink_capacity
        int a = g_ptr->maxNodeId();
        std::fill(sink_capacity.begin(), sink_capacity.end(), 0);
        if (a != -1 && _j <= a) {
            for (SubDigraph::InArcIt arc(sub_digraph, sub_digraph.nodeFromId(_j)); arc != lemon::INVALID; ++arc) {
                int i = sub_digraph.id(sub_digraph.source(arc));
                sink_capacity[i] = (*aM)[arc];
            }
        }
        else {
            throw std::range_error("invalid j is given");
        }

        dig.clear();
        // copy the graph
        for (int i = 0; i <= _j; i++)
            dig.addNode();
        for (SubDigraph::ArcIt a(sub_digraph); a != lemon::INVALID; ++a) {
            lemon::ListDigraph::Node s = sub_digraph.source(a);
            lemon::ListDigraph::Node t = sub_digraph.target(a);
            lemon::ListDigraph::Arc a1 = dig.addArc(s, t);
            dig_aM[a1] = (*aM)[a];
        }
        source_node = dig.addNode();
        sink_node = dig.nodeFromId(_j);
        tilde_G_size = a + 1;
        for (lemon::ListDigraph::NodeIt n(dig); n != lemon::INVALID; ++n) {
            if (n == sink_node || n == source_node)
                continue;
            dig.addArc(source_node, n);  
            if (sink_capacity[dig.id(n)] < tolerance.epsilon()) {
                dig.addArc(n, sink_node);
            }
        }
        double init_lambda = -0.1;
        Preflow pf(dig, dig_aM, source_node, sink_node);
        //find S_0 and T_0
        update_dig(init_lambda);
        pf.init();
        pf.startFirstPhase();
        pf.startSecondPhase();

        Set T_0 = get_min_cut_sink_side(pf);
        Set T_1 = Set::MakeEmpty(tilde_G_size);
        T_1.AddElement(_j);
        set_list.push_back(T_0);
        set_list.push_back(T_1);
        slice(T_0, T_1, pf.flowMap(), init_lambda, std::numeric_limits<double>::infinity());
        lambda_list.sort();
        auto is_superset = [](const Set& A, const Set& B){return B.IsSubSet(A);};
        set_list.sort(is_superset);
    }

    void PMF::reset_j(std::size_t j) { 
        _j = j; 
        node_filter[g_ptr->nodeFromId(_j)] = true;
        set_list.clear();
        lambda_list.clear();    
    }

    void PMF::modify_flow(const FlowMap& flowMap, FlowMap& newFlowMap){
        for (lemon::ListDigraph::ArcIt arc(dig); arc != lemon::INVALID; ++arc) {
            newFlowMap[arc] = flowMap[arc];
        }
        for (lemon::ListDigraph::OutArcIt e(dig, source_node); e != lemon::INVALID; ++e) {
            newFlowMap[e] = dig_aM[e];
        }
        for (lemon::ListDigraph::InArcIt e(dig, sink_node); e != lemon::INVALID; ++e) {
            if(flowMap[e] > dig_aM[e])
                newFlowMap[e] = dig_aM[e];
        }
    }

    void PMF::update_dig(double lambda) {
        for (lemon::ListDigraph::OutArcIt arc(dig, source_node); arc != lemon::INVALID; ++arc) {
            // get the next node id
            int i = dig.id(dig.target(arc));
            double a_i = _y_lambda[i].first, b_i = _y_lambda[i].second;
            double candidate = std::min<double>(a_i - lambda, b_i);
            dig_aM[arc] = 0;
            if (candidate < 0)
                dig_aM[arc] = -candidate;

        }

        for (lemon::ListDigraph::InArcIt arc(dig, sink_node); arc != lemon::INVALID; ++arc) {
            int i = dig.id(dig.source(arc));
            double a_i = _y_lambda[i].first, b_i = _y_lambda[i].second;
            dig_aM[arc] = sink_capacity[i] + std::max<double>(0, std::min<double>(a_i - lambda, b_i));
        }
    }

    void PMF::slice(Set& T_l, Set& T_r, const FlowMap& flowMap, double lambda_1, double lambda_3) {
#if _DEBUG
            update_dig(lambda_1);
            double value_1 = submodular::get_cut_value(dig, dig_aM, T_r) - submodular::get_cut_value(dig, dig_aM, T_l);
            if (value_1 < -1 * tolerance.epsilon()) {
                throw std::logic_error("value 1");
            }
            update_dig(lambda_3);
            double value_2 = submodular::get_cut_value(dig, dig_aM, T_r) - submodular::get_cut_value(dig, dig_aM, T_l);
            if (value_2 > tolerance.epsilon()) {
                throw std::logic_error("value 2");
            }
#endif
        // compute lambda_2
        double lambda_const = compute_lambda_eq_const(T_l, T_r);
        std::vector<pair> y_lambda_filter;
        for (int i = 0; i < tilde_G_size - 1; i++) {
            if (T_l.HasElement(i) && !T_r.HasElement(i))
                y_lambda_filter.push_back(_y_lambda[i]);
        }
        double lambda_2 = compute_lambda(y_lambda_filter, lambda_const);
        if (!tolerance.different(lambda_2, lambda_1) || !tolerance.different(lambda_2, lambda_3)) {
            lambda_list.push_back(lambda_2);
            return;
        }
        update_dig(lambda_2);
        if (lambda_2 < lambda_1 || lambda_2 > lambda_3) {
            std::stringstream ss;
            ss << "lambda value mismatch " << lambda_1 << ' ' << lambda_2 << ' ' << lambda_3;
            throw std::logic_error(ss.str());
        }
        // compute original value
        double original_flow_value = submodular::get_cut_value(dig, dig_aM, T_r);

        FlowMap newFlowMap(dig);
        modify_flow(flowMap, newFlowMap);
        Preflow pf_instance(dig, dig_aM, source_node, sink_node);
        bool isValid = pf_instance.init(newFlowMap);
#if _DEBUG
        if (!isValid)
            throw std::logic_error("not valid flow map to init.");
#endif
        pf_instance.startFirstPhase();
        pf_instance.startSecondPhase();
        double new_flow_value = pf_instance.flowValue();
        Set T_apostrophe = get_min_cut_sink_side(pf_instance);
#if _DEBUG
        if (!T_apostrophe.IsSubSet(T_l) || !T_r.IsSubSet(T_apostrophe)) {
            throw std::logic_error("not subset");
        }
#endif
        if(T_apostrophe != T_r && T_apostrophe != T_l && new_flow_value < original_flow_value - 10 * tolerance.epsilon()){
            // if no graph contraction, S \subseteq S_apostrophe and T \subseteq T_apostrophe
            set_list.push_back(T_apostrophe);
            slice(T_l, T_apostrophe, flowMap, lambda_1, lambda_2);
            slice(T_apostrophe, T_r, newFlowMap, lambda_2, lambda_3);
        }
        else {
            lambda_list.push_back(lambda_2);
        }
    }
    double PMF::compute_lambda_eq_const(Set& T_l, Set& T_r) {
        // compute the target value from original graph
        double target_value = submodular::get_cut_value(*g_ptr, *aM, T_l);
        target_value -= submodular::get_cut_value(*g_ptr, *aM, T_r);
        return target_value;
    }
    std::list<Set> PMF::get_set_list() { return set_list; }
    std::list<double> PMF::get_lambda_list() { return lambda_list; }
    void PMF::reset_y_lambda(std::vector<pair> parameter_list) {
        _y_lambda = parameter_list;
        sink_capacity.resize(_y_lambda.size());
    }

    PDT::PDT(lemon::ListDigraph* g, ArcMap* arcMap):
        _g(g),
        _arcMap(arcMap),
        pmf(g, arcMap, 0, _y_lambda){
    }

    std::list<double> PDT::get_critical_values() { return Lambda_list; }

    std::list<Partition> PDT::get_psp() {
        return partition_list;
    }

    void PDT::run() {
        // handle multiple run
        if (partition_list.size() > 0) {
            partition_list.clear();
            Lambda_list.clear();
            pmf.set_node_filter(false);            
        }
        Partition initial_partition;
        initial_partition.AddElement(Set("1"));
        partition_list.push_back(initial_partition);
        Lambda_list.push_back(INFINITY);
        _y_lambda.resize(lemon::countNodes(*_g), pair(0, INFINITY));
        for (int j = 1; j < _y_lambda.size(); j++) {
            pmf.reset_y_lambda(_y_lambda);
            pmf.reset_j(j);
            pmf.run();
            std::list<Set> t_list = pmf.get_set_list();
            std::list<double> lambda_list = pmf.get_lambda_list();
            for (int u = 0; u <= j; u++) {
                if (u == j) {
                    stl::CSet t;
                    t.AddElement(j);
                    _y_lambda[j] = pair(submodular::get_cut_value(*_g, *_arcMap, t), INFINITY);
                }
                else {
                    int i = -1;
                    std::list<double>::iterator lambda_it = lambda_list.begin();
                    for (Set& s : t_list) {
                        if (s.HasElement(u)) {
                            i++;
                            lambda_it++;
                        }
                    }
                    if (i != -1) {
                        lambda_it--;
                        if (*lambda_it > (_y_lambda[u].first - _y_lambda[u].second) )
                            _y_lambda[u] = std::make_pair(_y_lambda[u].first, _y_lambda[u].first - (*lambda_it));
                    }
                }
            }
            std::list<Partition> partition_list_apostrophe;
            std::list<double> Lambda_list_apostrophe;
            lambda_list.push_back(INFINITY);
            int i = 0, k = 0;
            std::list<Partition>::iterator p_it = partition_list.begin();
            std::list<Set>::iterator t_it = t_list.begin();
            std::list<double>::iterator d_i = Lambda_list.begin(), d_k = lambda_list.begin();
            Partition Q;
            while (i < Lambda_list.size() && k < lambda_list.size()) {
                Partition Q_apostrophe = p_it->expand(*t_it);
                if (Q_apostrophe != Q) {
                    Q = Q_apostrophe;
                    partition_list_apostrophe.push_back(Q);
                    Lambda_list_apostrophe.push_back(std::min(*d_i, *d_k));
                }
                else {
                    std::reverse_iterator<std::list<double>::iterator> last_it = Lambda_list_apostrophe.rbegin();
                    *last_it = std::min(*d_i, *d_k);
                }
                double d_i_v = *d_i, d_k_v = *d_k;
                if (d_i_v <= d_k_v) {
                    i++;
                    d_i++;
                    p_it++;
                }
                if (d_i_v >= d_k_v) {
                    k++;
                    d_k++;
                    t_it++;
                }
            }
            Lambda_list = Lambda_list_apostrophe;
            partition_list = partition_list_apostrophe;
        }
        Lambda_list.pop_back();
    }
}
