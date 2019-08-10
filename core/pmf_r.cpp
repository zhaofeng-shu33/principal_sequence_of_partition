#include <tuple>
#include <numeric>
#include <lemon/list_graph.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include "core/graph/graph.h"
#include "core/pmf_r.h"
namespace parametric {
    using Set = stl::CSet;
    double PMF_R::compute_lambda(const std::vector<pair>& parameter_list, const double target_value) {
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

    PMF_R::PMF_R(lemon::ListDigraph* g, ArcMap* arcMap, std::size_t j, std::vector<pair>& y_lambda) :
        g_ptr(g), aM(arcMap), _j(j),
        _y_lambda(y_lambda),
        dig_aM(dig)
    {        
        sink_capacity.resize(_y_lambda.size());
    }
	PMF_R::PMF_R():dig_aM(dig){}
    Set PMF_R::get_min_cut_sink_side(Preflow& pf) {
        Set t = Set::MakeEmpty(tilde_G_size);
        for (lemon::ListDigraph::NodeIt n(dig); n != lemon::INVALID; ++n) {
            if (!pf.minCut(n))
                t.AddElement(dig.id(n));
        }
        return t;
    }
    
    void PMF_R::run() {
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
		source_node_id = dig.id(source_node);
		sink_node_id = dig.id(sink_node);
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

        Set T_0 = get_min_cut_sink_side(pf);
        Set T_1 = Set::MakeEmpty(tilde_G_size);
        T_1.AddElement(_j);
        set_list.push_back(T_0);
        set_list.push_back(T_1);
		FlowMap newFlowMap;
		set_flowMap(dig, pf.flowMap(), newFlowMap);
        slice(T_0, T_1, newFlowMap, init_lambda, std::numeric_limits<double>::infinity());
        lambda_list.sort();
        auto is_superset = [](const Set& A, const Set& B){return B.IsSubSet(A);};
        set_list.sort(is_superset);
    }
	
    void PMF_R::reset_j(std::size_t j) { 
        _j = j; 
        set_list.clear();
        lambda_list.clear();    
	}
	void PMF_R::modify_flow(const Set& S, const Set& T, const lemon::ListDigraph& G, const ArcMap& capMap, const FlowMap& flowMap, FlowMap& newFlowMap){
		std::map<int, std::pair<double, double>> s_capacity_map;
		std::map<int, std::pair<double, double>> t_capacity_map;
		for (lemon::ListDigraph::NodeIt n(G); n != lemon::INVALID; ++n) {
			int tmp_id = G.id(n);
			if (tmp_id == source_node_id || tmp_id == sink_node_id)
				continue;
			s_capacity_map[tmp_id] = std::make_pair(0.0, 0.0);
			t_capacity_map[tmp_id] = std::make_pair(0.0, 0.0);
		}
		double s_t_flow = 0;
		// compute flow
		for (std::pair<int, std::map<int, double>> kv_outer : flowMap) 
			for(std::pair<int, double> kv_inner: kv_outer.second){
				int u_id = kv_outer.first;
				int v_id = kv_inner.first;
				double cv = kv_inner.second; // capacity value
				if (S.HasElement(u_id)) {
					if (T.HasElement(v_id)) {
						s_t_flow += cv;
					}
					else if (!S.HasElement(v_id)) {
						s_capacity_map[v_id].first += cv;
					}
				}
				else if (T.HasElement(u_id)) {
					if (S.HasElement(v_id)) {
						s_t_flow -= cv;
					}
					else if (!T.HasElement(v_id)) {
						t_capacity_map[v_id].second += cv;
					}
				}
				else {
					if (S.HasElement(v_id)) {
						s_capacity_map[u_id].second += cv;
					}
					else if (T.HasElement(v_id)) {
						t_capacity_map[u_id].first += cv;
					}
					else {						
						newFlowMap[u_id][v_id] = cv;
					}
				}
		}
		// add necessary s-t arc
		addFlowArc(source_node_id, sink_node_id, s_t_flow, newFlowMap);
		// add source -> others
		for (std::pair<int, std::pair<double, double>> kvp : s_capacity_map) {
			addFlowArc(source_node_id, kvp.first, kvp.second.first, newFlowMap);
			addFlowArc(kvp.first, source_node_id, kvp.second.second, newFlowMap);
		}
		// add others -> sink
		for (std::pair<int, std::pair<double, double>> kvp : t_capacity_map) {
			addFlowArc(kvp.first, sink_node_id, kvp.second.first, newFlowMap);
			addFlowArc(sink_node_id, kvp.first, kvp.second.second, newFlowMap);
		}
		// add missing arc
		for (lemon::ListDigraph::ArcIt a(G); a != lemon::INVALID; ++a) {
			int u = G.id(G.source(a));
			int v = G.id(G.target(a));
			FlowMap::iterator u_iterator = newFlowMap.find(u);
			if (u_iterator == newFlowMap.end()) {
				newFlowMap[u][v] = 0;
				continue;
			}
			std::map<int, double>::iterator v_iterator = u_iterator->second.find(v);
			if (v_iterator == u_iterator->second.end()) {
				u_iterator->second[v] = 0;
			}
		}
		// not exceeds capMap
		for (lemon::ListDigraph::InArcIt a(G, sink_node); a != lemon::INVALID; ++a) {
			int u = G.id(G.source(a));			
			if (newFlowMap[u][sink_node_id] > capMap[a]) {
				newFlowMap[u][sink_node_id]= capMap[a];
			}
		}
#if _DEBUG
		// check newFlowMap has at least the number of arc with directed graph G
		for (lemon::ListDigraph::ArcIt a(G); a != lemon::INVALID; ++a) {
			int u = G.id(G.source(a));
			int v = G.id(G.target(a));
			if (newFlowMap[u][v] > capMap[a] + tolerance.epsilon())
				throw std::logic_error("newFlowMap does not conform to capacity constraint");
		}
#endif
	}

    void PMF_R::update_dig(double lambda) {
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

    void PMF_R::slice(Set& T_l, Set& T_r, const FlowMap& flowMap, double lambda_1, double lambda_3) {
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
		
		Set S = T_l.Complement(tilde_G_size);
		lemon::ListDigraph newDig;
		ArcMap newArcMap(newDig);
		contract(S, T_r,newDig, newArcMap);
		FlowMap newFlowMap;
		modify_flow(S, T_r, newDig, newArcMap, flowMap, newFlowMap);
        // do not use graph contraction
        Preflow pf_instance(newDig, newArcMap, source_node, sink_node);
		// pf_instance.init();
		// bool isValid = true;
		Preflow::FlowMap innerNewFlowMap(newDig);
		get_preflow_flowMap(newDig, newFlowMap, innerNewFlowMap);
        bool isValid = pf_instance.init(innerNewFlowMap);
#if _DEBUG
		if (!isValid)
			throw std::logic_error("not valid flow map to init.");
#endif
        pf_instance.startFirstPhase();
        pf_instance.startSecondPhase();
		double new_flow_value = pf_instance.flowValue();
        Set T_apostrophe = get_min_cut_sink_side(pf_instance);
		Set T_apostrophe_total = T_apostrophe.Union(T_r);
        if(T_apostrophe_total != T_r && T_apostrophe_total != T_l && new_flow_value < original_flow_value - tolerance.epsilon()){
            set_list.push_back(T_apostrophe_total);
            slice(T_l, T_apostrophe_total, flowMap, lambda_1, lambda_2);
            slice(T_apostrophe_total, T_r, newFlowMap, lambda_2, lambda_3);
        }
        else {
            lambda_list.push_back(lambda_2);
        }
    }
    double PMF_R::compute_lambda_eq_const(Set& T_l, Set& T_r) {
        // compute the target value from original graph
        double target_value = submodular::get_cut_value(*g_ptr, *aM, T_l);
        target_value -= submodular::get_cut_value(*g_ptr, *aM, T_r);
        return target_value;
    }
	double PMF_R::contract(const Set& S, const Set& T, lemon::ListDigraph& G, ArcMap& arcMap) {
#if _DEBUG
		// check s \in S and t \in T
		if (!S.HasElement(source_node_id) || !T.HasElement(sink_node_id)) {
			throw std::logic_error("s \in S or t \in T fails");
		}
		// G should not have nodes
		if (lemon::countNodes(G) > 0) {
			throw std::logic_error("G should not have nodes");
		}
#endif
		// add necessary nodes
		for (int i = 0; i < tilde_G_size; i++) {
			G.addNode();
		}
		// delete nodes from S
		for (int i : S) {
			if (i != source_node_id)
				G.erase(G.nodeFromId(i));
		}
		// delete nodes from T
		for (int i : T) {
			if (i != sink_node_id)
				G.erase(G.nodeFromId(i));
		}
		
		std::map<int, std::pair<double, double>> s_capacity_map;
		std::map<int, std::pair<double, double>> t_capacity_map;
		for (lemon::ListDigraph::NodeIt n(G); n != lemon::INVALID; ++n) {
			int tmp_id = G.id(n);
			if (tmp_id == source_node_id || tmp_id == sink_node_id)
				continue;
			s_capacity_map[tmp_id] = std::make_pair(0.0, 0.0);
			t_capacity_map[tmp_id] = std::make_pair(0.0, 0.0);
		}
		double s_t_cost = 0, t_s_cost = 0;
		// compute cost
		for (lemon::ListDigraph::ArcIt a(dig); a != lemon::INVALID; ++a) {
			lemon::ListDigraph::Node u = dig.source(a);
			lemon::ListDigraph::Node v = dig.target(a);
			int u_id = dig.id(u);
			int v_id = dig.id(v);
			double cv = dig_aM[a]; // capacity value
			if (S.HasElement(u_id)) {
				if (T.HasElement(v_id)) {
					s_t_cost += cv;
				}
				else if (!S.HasElement(v_id)) {
					s_capacity_map[v_id].first += cv;
				}
			}
			else if (T.HasElement(u_id)) {
				if (S.HasElement(v_id)) {
					t_s_cost += cv;
				}
				else if (!T.HasElement(v_id)) {
					t_capacity_map[v_id].second += cv;
				}
			}
			else {
				if (S.HasElement(v_id)) {
					s_capacity_map[u_id].second += cv;
				}
				else if (T.HasElement(v_id)) {
					t_capacity_map[u_id].first += cv;
				}
				else {
					// add arc to new graph
					lemon::ListDigraph::Arc newA = G.addArc(G.nodeFromId(u_id), G.nodeFromId(v_id));
					arcMap[newA] = cv;					
				}
			}
		}
		// add necessary s-t arc
		addArc(source_node_id, sink_node_id, s_t_cost, G, arcMap);
		addArc(sink_node_id, source_node_id, t_s_cost, G, arcMap);
		// add source -> others
		for (std::pair<int, std::pair<double, double>> kvp : s_capacity_map) {
			addArc(source_node_id, kvp.first, kvp.second.first, G, arcMap);
			addArc(kvp.first, source_node_id, kvp.second.second, G, arcMap);
		}
		// add others -> sink
		for (std::pair<int, std::pair<double, double>> kvp : t_capacity_map) {
			addArc(kvp.first, sink_node_id, kvp.second.first, G, arcMap);
			addArc(sink_node_id, kvp.first, kvp.second.second, G, arcMap);
		}
	}
	inline void PMF_R::addArc(int u, int v, double w, lemon::ListDigraph& G, ArcMap& arcMap) {
		if (w > tolerance.epsilon()) {
			lemon::ListDigraph::Arc newA = G.addArc(G.nodeFromId(u), G.nodeFromId(v));
			arcMap[newA] = w;
		}
	}
	void PMF_R::set_flowMap(const lemon::ListDigraph& G, const Preflow::FlowMap& pfm, FlowMap& flowMap) {
		for (lemon::ListDigraph::ArcIt a(G); a != lemon::INVALID; ++a) {
			int u = G.id(G.source(a));
			int v = G.id(G.target(a));
			flowMap[u][v] = pfm[a];
		}
	}
	void PMF_R::get_preflow_flowMap(const lemon::ListDigraph& G, const FlowMap& flowMapDic, Preflow::FlowMap& flowMap) {
		for (lemon::ListDigraph::ArcIt a(G); a != lemon::INVALID; ++a) {
			int u = G.id(G.source(a));
			int v = G.id(G.target(a));
			flowMap[a] = flowMapDic.at(u).at(v);
		}
	}
	void PMF_R::set_source_node_id(int new_id) {
		source_node_id = new_id;
		source_node = dig.nodeFromId(source_node_id);
	}
	void PMF_R::set_sink_node_id(int new_id) {
		sink_node_id = new_id;
		sink_node = dig.nodeFromId(sink_node_id);
	}
	void PMF_R::set_tilde_G_size(int new_size) {
		tilde_G_size = new_size;
	}
	inline void PMF_R::addFlowArc(int u, int v, double w, FlowMap& flowMap) {
		if (w > tolerance.epsilon()) {
			flowMap[u][v] = w;
		}
	}


	PDT_R::PDT_R(lemon::ListDigraph* g, ArcMap* arcMap):
		_g(g),
		_arcMap(arcMap),
		pmf(g, arcMap, 0, _y_lambda) {}
	void PDT_R::run() {
		partition_list.clear();
		Lambda_list.clear();
		partition_list.push_back(Partition());
		Lambda_list.push_back(INFINITY);
		_y_lambda.resize(lemon::countNodes(*_g), pair(0, INFINITY));
		for (int j = 0; j < _y_lambda.size(); j++) {
			pmf.reset_y_lambda(_y_lambda);
			pmf.reset_j(j);
			pmf.run();
			std::list<Set> t_list = pmf.get_set_list();
			std::list<double> lambda_list = pmf.get_lambda_list();
			for (int u = 0; u < _y_lambda.size(); u++) {
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
						if (*lambda_it > (_y_lambda[u].first - _y_lambda[u].second))
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
