#include <tuple>
#include <numeric>
#include <lemon/list_graph.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include "core/pmf.h"
#if _DEBUG
#include <cassert>
#endif

namespace parametric {
    using Set = stl::CSet;
    double PMF::compute_lambda(const std::vector<pair>& parameter_list, const double target_value) {
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
            double intersept = std::accumulate(parameter_list.begin(), parameter_list.end(), -1 * target_value,
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
        double intersept = 0;
        for (const pair& p : parameter_list) {
            sum += std::min(p.first - 2 * last_tp, p.second);
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
            slope -= 2;
            last_tp = tp;
        }
        return (target_value - sum) / slope + last_tp;
    }
    PMF::PMF(lemon::ListDigraph& g, ArcMap& arcMap, std::size_t j, std::vector<pair>& y_lambda) :
        g_ptr(&g), aM(&arcMap), _j(j),
        _y_lambda(y_lambda),
        dig_aM(dig)
    {        
        sink_capacity.resize(_y_lambda.size());

    }
    Set PMF::get_min_cut_source_side(Preflow& pf) {
        Set s = Set::MakeEmpty(tilde_G_size);
        for (lemon::ListDigraph::NodeIt n(dig); n != lemon::INVALID; ++n) {
            if (pf.minCut(n))
                s.AddElement(dig.id(n));
        }
        return s;
    }
    void PMF::run() {
        //set sink_capacity
        int a = g_ptr->maxNodeId();
		std::fill(sink_capacity.begin(), sink_capacity.end(), 0);
        if (a != -1 && _j <= a) {
            for (lemon::ListDigraph::InArcIt arc(*g_ptr, g_ptr->nodeFromId(_j)); arc != lemon::INVALID; ++arc) {
                int i = g_ptr->id(g_ptr->source(arc));
                sink_capacity[i] = (*aM)[arc];
            }
        }
        else {
            throw std::range_error("invalid j is given");
        }

        dig.clear();
        // copy the graph
        for (int i = 0; i <= g_ptr->maxNodeId(); i++)
            dig.addNode();
        for (lemon::ListDigraph::ArcIt a(*g_ptr); a != lemon::INVALID; ++a) {
            lemon::ListDigraph::Node s = dig.nodeFromId(g_ptr->id(g_ptr->source(a)));
            lemon::ListDigraph::Node t = dig.nodeFromId(g_ptr->id(g_ptr->target(a)));
            lemon::ListDigraph::Arc a1 = dig.addArc(s, t);
            dig_aM[a1] = (*aM)[a];
        }
        source_node = dig.addNode();
        sink_node = dig.nodeFromId(_j);        
        tilde_G_size = dig.maxNodeId() + 1;
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

        Set S_0 = get_min_cut_source_side(pf);
        Set T_0 = S_0.Complement(tilde_G_size);
        Set T_1 = Set::MakeEmpty(tilde_G_size);
        T_1.AddElement(_j);
        set_list.push_back(T_0);
        set_list.push_back(T_1);
        slice(S_0, T_1, pf.flowMap(), init_lambda, std::numeric_limits<double>::infinity());
    }

    void PMF::reset_j(std::size_t j) { 
        _j = j; 
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
			double candidate = std::min<double>(a_i - 2 * lambda, b_i);
			if (candidate < 0)
				dig_aM[arc] = -candidate;

        }

        for (lemon::ListDigraph::InArcIt arc(dig, sink_node); arc != lemon::INVALID; ++arc) {
            int i = dig.id(dig.source(arc));
            double a_i = _y_lambda[i].first, b_i = _y_lambda[i].second;
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
    void PMF::slice(Set& S, Set& T, const FlowMap& flowMap, double lambda_1, double lambda_3) {

        // compute lambda_2
        double lambda_const = compute_lambda_eq_const(S, T);
        std::vector<pair> y_lambda_filter;
        for (int i = 0; i < tilde_G_size - 1; i++) {
            if (S.HasElement(i) || T.HasElement(i))
                continue;
            y_lambda_filter.push_back(_y_lambda[i]);
        }
        double lambda_2 = compute_lambda(y_lambda_filter, -lambda_const);
		if (!tolerance.different(lambda_2, lambda_1)) {
			insert(lambda_2);
			return;
		}
		else if (!tolerance.different(lambda_2, lambda_3)) {
			insert(lambda_2);
			return;
		}
        update_dig(lambda_2);
		if (lambda_2 < lambda_1 || lambda_2 > lambda_3) {
			std::stringstream ss;
			ss << "lambda value mismatch " << lambda_1 << ' ' << lambda_2 << ' ' << lambda_3;
#if _DEBUG
			throw std::logic_error(ss.str());
#else
			std::cout << ss.str() << std::endl;
			insert(lambda_2);
			return;
#endif
		}
		// compute original value
		double original_flow_value = compute_cut(dig, dig_aM, S);

		FlowMap newFlowMap(dig);
		modify_flow(flowMap, newFlowMap);
        // do not use graph contraction
        Preflow pf_instance(dig, dig_aM, source_node, sink_node);
		// pf_instance.init();
		// bool isValid = true;
        bool isValid = pf_instance.init(newFlowMap);
#if _DEBUG
		if (!isValid)
			throw std::logic_error("not valid flow map to init.");
#endif
        pf_instance.startFirstPhase();
        pf_instance.startSecondPhase();
		double new_flow_value = pf_instance.flowValue();
        Set S_apostrophe = get_min_cut_source_side(pf_instance);
        Set T_apostrophe = S_apostrophe.Complement(tilde_G_size);
        if(S_apostrophe != S && T_apostrophe != T && new_flow_value < original_flow_value){
            // if no graph contraction, S \subseteq S_apostrophe and T \subseteq T_apostrophe
#if _DEBUG
            assert(S.IsSubSet(S_apostrophe));
            assert(T.IsSubSet(T_apostrophe));
#endif
            Set S_Union = S.Union(S_apostrophe);
            Set T_Union = T.Union(T_apostrophe);
            insert_set(T_Union);
            slice(S, T_Union, flowMap, lambda_1, lambda_2);
            slice(S_Union, T, newFlowMap, lambda_2, lambda_3);
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
    PDT::PDT(const PDT& another_pdt): _y_lambda(another_pdt._y_lambda),
        _g(another_pdt._g),
        _arcMap(another_pdt._arcMap),
        pmf(*_g, *_arcMap, 0, _y_lambda),
        Lambda_list(another_pdt.Lambda_list),
        partition_list(another_pdt.partition_list){}
    PDT* make_pdt(std::size_t num_nodes, std::vector<std::tuple<std::size_t, std::size_t, double>>& edges) {
        lemon::ListDigraph* g = new lemon::ListDigraph();
        g->reserveNode(num_nodes);
        for(int i = 0; i < num_nodes; i++)
            g->addNode();
        lemon::ListDigraph::ArcMap<double>* aM = new lemon::ListDigraph::ArcMap<double>(*g);
        for(std::tuple<std::size_t, std::size_t, double>& edge_tuple : edges){
            lemon::ListDigraph::Node s = g->nodeFromId(std::get<0>(edge_tuple));
            lemon::ListDigraph::Node t = g->nodeFromId(std::get<1>(edge_tuple));
            lemon::ListDigraph::Arc a1 = g->addArc(s, t);
            lemon::ListDigraph::Arc a2 = g->addArc(t, s);
            (*aM)[a1] = std::get<2>(edge_tuple);
            (*aM)[a2] = std::get<2>(edge_tuple);
        }
        PDT* pdt = new PDT(*g, *aM);
        return pdt;
    }
    PDT::PDT(lemon::ListDigraph& g, ArcMap& arcMap):
        _g(&g),
        _arcMap(&arcMap),
        pmf(g, arcMap, 0, _y_lambda){
        partition_list.push_back(Partition());
        Lambda_list.push_back(INFINITY);
    }
    void PDT::run() {
        _y_lambda.resize(lemon::countNodes(*_g), pair(0, INFINITY));
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
				double d_i_v = *d_i, d_k_v = *d_k;
                if (d_i_v <= d_k_v) {
                    i++;
                    d_i++;
                    p_it++;
                }
                if (d_i_v >= d_k_v) {
                    k++;
                    d_k++;
                    s_it++;
                }
            }
            Lambda_list = Lambda_list_apostrophe;
            partition_list = partition_list_apostrophe;
        }
    }
}
