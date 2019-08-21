#include <tuple>
#include <numeric>
#include <lemon/list_graph.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <lemon/adaptors.h>
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
        dig_aM(dig), node_filter(*g_ptr), sub_digraph(*g_ptr, node_filter)
    {                
        node_filter[g_ptr->nodeFromId(0)] = true;
    }
    void PMF_R::set_node_filter(bool value) {
        for (lemon::ListDigraph::NodeIt n(*g_ptr); n != lemon::INVALID; ++n) {
            node_filter[n] = value;
        }
    }
    Set PMF_R::get_min_cut_sink_side_reverse(const lemon::ReverseDigraph<lemon::ListDigraph>& digraph, Preflow_Reverse& pf) {
        Set t = Set::MakeEmpty(tilde_G_size);
        for (lemon::ReverseDigraph<lemon::ListDigraph>::NodeIt n(digraph); n != lemon::INVALID; ++n) {
            if (pf.minCutSource(n))
                t.AddElement(dig.id(n));
        }
        return t;
    }
    Set PMF_R::get_min_cut_sink_side(const lemon::ListDigraph& digraph, Preflow& pf) {
        Set t = Set::MakeEmpty(tilde_G_size);
        for (lemon::ListDigraph::NodeIt n(digraph); n != lemon::INVALID; ++n) {
            if (!pf.minCut(n))
                t.AddElement(dig.id(n));
            }
        return t;
    }
    
    void PMF_R::run() {
        //set sink_capacity
        sink_capacity.clear();
        int a = g_ptr->maxNodeId();        
        if (a != -1 && _j <= a) {
            for (SubDigraph::InArcIt arc(sub_digraph, sub_digraph.nodeFromId(_j)); arc != lemon::INVALID; ++arc) {
                int i = sub_digraph.id(sub_digraph.source(arc));
                sink_capacity[i] = std::make_pair(0, (*aM)[arc]);
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
        source_node_id = dig.id(source_node);
        sink_node_id = dig.id(sink_node);
        tilde_G_size = dig.maxNodeId() + 1;
        for (lemon::ListDigraph::NodeIt n(dig); n != lemon::INVALID; ++n) {
            if (n == sink_node || n == source_node)
                continue;
            dig.addArc(source_node, n);  
            if (sink_capacity[dig.id(n)].second < tolerance.epsilon()) {
                dig.addArc(n, sink_node);
            }
        }
        double init_lambda = -0.1;
        Preflow pf(dig, dig_aM, source_node, sink_node);
        //find S_0 and T_0
        update_dig(init_lambda, dig, dig_aM, sink_capacity);
        pf.init();
        pf.startFirstPhase();
        pf.startSecondPhase();

        Set T_0 = get_min_cut_sink_side(dig, pf);
        Set T_1 = Set::MakeEmpty(tilde_G_size);
        T_1.AddElement(_j);
        set_list.push_back(T_0);
        set_list.push_back(T_1);
        FlowMap leftFlowMap(dig);
        set_flowMap(dig, pf.flowMap(), leftFlowMap);

        double lambda_3 = 0;
        for (int i = 0; i < _y_lambda.size(); i++) {
            // get the next node id
            double a_i = _y_lambda[i].first, b_i = _y_lambda[i].second;
            if (a_i - b_i > lambda_3)
                lambda_3 = a_i - b_i;
            if (a_i > lambda_3)
                lambda_3 = a_i;
            if (sink_capacity[i].second + a_i > lambda_3)
                lambda_3 = sink_capacity[i].second + a_i;
        }
        // run the reverse Preflow at lambda_3
        update_dig(lambda_3, dig, dig_aM, sink_capacity);
        lemon::ReverseDigraph<lemon::ListDigraph> reverse_newDig(dig);
        Preflow_Reverse pf_reverse_instance(reverse_newDig, dig_aM, sink_node, source_node);
        pf_reverse_instance.run();

        // construct rightFlowMap
        FlowMap rightFlowMap(dig);
        set_flowMap(dig, pf_reverse_instance.flowMap(), rightFlowMap);

        slice(&dig, &dig_aM, sink_capacity, T_0, T_1, leftFlowMap, rightFlowMap, init_lambda, lambda_3, pf.elevator(), pf_reverse_instance.elevator(), false);
        lambda_list.sort();
        auto is_superset = [](const Set& A, const Set& B){return B.IsSubSet(A);};
        set_list.sort(is_superset);
    }
    
    void PMF_R::reset_j(std::size_t j) { 
        _j = j; 
        node_filter[g_ptr->nodeFromId(_j)] = true;
        set_list.clear();
        lambda_list.clear();    
    }
    void PMF_R::modify_flow(const Set& S, const Set& T, const lemon::ListDigraph& G, const lemon::ListDigraph& old_G, const FlowMap& flowMap, FlowMap& newFlowMap){
        std::map<int, std::pair<double, double>> s_capacity_map;
        std::map<int, std::pair<double, double>> t_capacity_map;
        std::map<int, std::map<int, double>> tmp_map;
        for (lemon::ListDigraph::NodeIt n(G); n != lemon::INVALID; ++n) {
            int tmp_id = G.id(n);
            if (tmp_id == source_node_id || tmp_id == sink_node_id)
                continue;
            s_capacity_map[tmp_id] = std::make_pair(0.0, 0.0);
            t_capacity_map[tmp_id] = std::make_pair(0.0, 0.0);
        }
        double s_t_flow = 0;
        // compute flow
        for (lemon::ListDigraph::ArcIt arc(old_G); arc != lemon::INVALID; ++arc)
        {
                int u_id = old_G.id(old_G.source(arc));
                int v_id = old_G.id(old_G.target(arc));
                double cv = flowMap[arc]; // flow value
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
                        tmp_map[u_id][v_id] = cv;
                    }
                }
        }
        for (lemon::ListDigraph::ArcIt arc(G); arc != lemon::INVALID; ++arc) {
            int u_id = G.id(G.source(arc));
            int v_id = G.id(G.target(arc));
            if (S.HasElement(u_id)) {
                if (T.HasElement(v_id)) {
                    newFlowMap[arc] = s_t_flow;
                }
                else {
                    newFlowMap[arc] = s_capacity_map[v_id].first;
                }
            }
            else if (T.HasElement(u_id)) {
                if(!S.HasElement(v_id))
                    newFlowMap[arc] = t_capacity_map[v_id].second;
            }
            else {
                if (S.HasElement(v_id))
                    newFlowMap[arc] = s_capacity_map[u_id].second;
                else if (T.HasElement(v_id))
                    newFlowMap[arc] = t_capacity_map[u_id].first;
                else
                    newFlowMap[arc] = tmp_map[u_id][v_id];
            }
        }
    }
    void PMF_R::modify_flow(const lemon::ListDigraph& G, const ArcMap& capMap, const FlowMap& flowMap, FlowMap& newFlowMap) {

        for (lemon::ListDigraph::ArcIt a(G); a != lemon::INVALID; ++a) {
                newFlowMap[a] = flowMap[a];
        }
        // not exceeds capMap
        for (lemon::ListDigraph::InArcIt a(G, sink_node); a != lemon::INVALID; ++a) {
            if (newFlowMap[a] > capMap[a]) {
                newFlowMap[a] = capMap[a];
            }
        }
        for (lemon::ListDigraph::OutArcIt a(G, source_node); a != lemon::INVALID; ++a) {
            if (newFlowMap[a] > capMap[a]) {
                newFlowMap[a] = capMap[a];
            }
        }
    }
    void PMF_R::update_dig(double lambda, lemon::ListDigraph& G, ArcMap& cap, std::map<int, std::pair<double, double>>& update_base) {
        for (lemon::ListDigraph::OutArcIt arc(G, source_node); arc != lemon::INVALID; ++arc) {
            // get the next node id
            int i = G.id(G.target(arc));
            if (i == sink_node_id)
                continue;
            double a_i = _y_lambda[i].first, b_i = _y_lambda[i].second;
            double candidate = std::min<double>(a_i - lambda, b_i);
            double cap_arc_value = candidate < 0 ? -candidate : 0;
            cap_arc_value += update_base[i].first;
            cap[arc] = cap_arc_value;
        }

        for (lemon::ListDigraph::InArcIt arc(G, sink_node); arc != lemon::INVALID; ++arc) {
            int i = G.id(G.source(arc));
            if (i == source_node_id)
                continue;
            double a_i = _y_lambda[i].first, b_i = _y_lambda[i].second;
            cap[arc] = update_base[i].second + std::max<double>(0, std::min<double>(a_i - lambda, b_i));
        }
    }
    void PMF_R::construct_new_update_base(const lemon::ListDigraph& G, const Set& S, const Set& T, std::map<int, std::pair<double, double>>& new_update_base) {
        for (lemon::ListDigraph::NodeIt n(G); n != lemon::INVALID; ++n) {
            if (n == source_node || n == sink_node)
                continue;
            double s_value = 0, t_value = 0;
            for (lemon::ListDigraph::InArcIt a(*g_ptr, n); a != lemon::INVALID; ++a) {
                int s_id = g_ptr->id(g_ptr->source(a));
                if(S.HasElement(s_id))
                    s_value += (*aM)[a];
            }
            for (lemon::ListDigraph::OutArcIt a(*g_ptr, n); a != lemon::INVALID; ++a) {
                int t_id = g_ptr->id(g_ptr->target(a));
                if (T.HasElement(t_id))
                    t_value += (*aM)[a];
            }
            int n_id = G.id(n);
            new_update_base[n_id] = std::make_pair(s_value, t_value);
        }
    }

    void PMF_R::executePreflow(ThreadArgumentPack& TAP) {
        lemon::ListDigraph& newDig = *TAP.newDig;
        FlowMap new_leftFlowMap(newDig);
        ArcMap& newArcMap = *TAP.newArcMap;
        FlowMap& leftArcMap = *TAP.flowMap;
        Set& S = *TAP.S;
        Set& T = *TAP.T;
        Set& T_apostrophe = *TAP.T_apostrophe;
        double& new_flow_value = *TAP.new_flow_value;
        FlowMap& newFlowMap = *TAP.newFlowMap;
        Elevator* ele = TAP.ele;
        bool isValid;
        try{
            modify_flow(newDig, newArcMap, leftArcMap, new_leftFlowMap);
            Preflow::FlowMap inner_new_leftFlowMap(newDig);
            get_preflow_flowMap(newDig, new_leftFlowMap, inner_new_leftFlowMap);
            Preflow pf_instance(newDig, newArcMap, source_node, sink_node);
            if (ele == NULL)
                isValid = pf_instance.init(inner_new_leftFlowMap);
            else{
                TAP.ele_out = new Elevator(*ele);
                isValid = pf_instance.init(inner_new_leftFlowMap, TAP.ele_out);
            }
#if _DEBUG
            if (!isValid)
                throw std::logic_error("not valid flow map to init.");
#endif
            pf_instance.startFirstPhase();
            pf_instance.startSecondPhase();
            if (ele == NULL)
                TAP.ele_out = pf_instance.elevator();

            set_flowMap(newDig, pf_instance.flowMap(), newFlowMap);

            new_flow_value = pf_instance.flowValue();
            T_apostrophe = get_min_cut_sink_side(newDig, pf_instance);
            cond.notify_all();
        }
        catch (boost::thread_interrupted&) {
            return;
        }
        catch (...) {
            TAP.thread_exception_ptr = std::current_exception();
        }
    }
    void PMF_R::executePreflow_reverse(ThreadArgumentPack& TAP) {
        lemon::ListDigraph& newDig = *TAP.newDig;
        ArcMap& newArcMap = *TAP.newArcMap;
        FlowMap& rightArcMap = *TAP.flowMap;
        Set& S = *TAP.S;
        Set& T = *TAP.T;
        Set& T_apostrophe = *TAP.T_apostrophe;
        double& new_flow_value = *TAP.new_flow_value;
        FlowMap& newFlowMap = *TAP.newFlowMap;
        Elevator_Reverse* ele = TAP.ele_reverse;
        lemon::ReverseDigraph<lemon::ListDigraph>& reverse_newDig = *TAP.reverse_newDig;
        bool isValid;        
        try{    
            Preflow_Reverse pf_reverse_instance(reverse_newDig, newArcMap, sink_node, source_node);
            FlowMap new_rightFlowMap(newDig);
            modify_flow(newDig, newArcMap, rightArcMap, new_rightFlowMap);
            Preflow_Reverse::FlowMap inner_new_rightFlowMap(newDig);
            get_preflow_flowMap(newDig, new_rightFlowMap, inner_new_rightFlowMap);
            if(ele == NULL)
                isValid = pf_reverse_instance.init(inner_new_rightFlowMap);
            else{
                TAP.ele_reverse_out = new Elevator_Reverse(*ele);
                isValid = pf_reverse_instance.init(inner_new_rightFlowMap, TAP.ele_reverse_out);
            }
#if _DEBUG
            if (!isValid)
                throw std::logic_error("not valid flow map to init.");
#endif
            pf_reverse_instance.startFirstPhase();
            pf_reverse_instance.startSecondPhase(true);


            set_flowMap(newDig, pf_reverse_instance.flowMap(), newFlowMap);
            if (ele == NULL)
                TAP.ele_reverse_out = pf_reverse_instance.elevator();

            new_flow_value = pf_reverse_instance.flowValue();
            T_apostrophe = get_min_cut_sink_side_reverse(reverse_newDig, pf_reverse_instance);
            cond.notify_all();
        }
        catch (boost::thread_interrupted&) {
            return;
        }
        catch (...) {
            TAP.thread_exception_ptr = std::current_exception();
        }        
    }

    void PMF_R::slice(lemon::ListDigraph* G, ArcMap* arcMap, std::map<int, std::pair<double, double>>& update_base, Set& T_l, Set& T_r, FlowMap& leftArcMap, FlowMap& rightArcMap, double lambda_1, double lambda_3, Elevator* left_ele, Elevator_Reverse* right_ele, bool is_contract) {
#if _DEBUG

            update_dig(lambda_1, dig, dig_aM, sink_capacity);
            double value_1 = submodular::get_cut_value(dig, dig_aM, T_r) - submodular::get_cut_value(dig, dig_aM, T_l);
            if (value_1 < -1 * tolerance.epsilon()) {
                throw std::logic_error("value 1");
            }
            update_dig(lambda_3, dig, dig_aM, sink_capacity);
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
        
        if (lambda_2 < lambda_1 || lambda_2 > lambda_3) {
            std::stringstream ss;
            ss << "lambda value mismatch " << lambda_1 << ' ' << lambda_2 << ' ' << lambda_3;
            throw std::logic_error(ss.str());
        }
        
        Set S = T_l.Complement(tilde_G_size);

        lemon::ListDigraph* newDig;
        lemon::ReverseDigraph<lemon::ListDigraph>* reverse_newDig = NULL;
        ArcMap* newArcMap;
        Elevator* left_inner;
        Elevator_Reverse* right_inner;
        FlowMap* leftArcMap_inner, * rightArcMap_inner;
        std::map<int, std::pair<double, double>>* new_update_base;
        if (is_contract) {
            newDig = new lemon::ListDigraph();
            newArcMap = new ArcMap(*newDig);
            contract(S, T_r, *G, *arcMap, *newDig, *newArcMap);
            new_update_base = new std::map<int, std::pair<double, double>>();
            construct_new_update_base(*newDig, S, T_r, *new_update_base);
            leftArcMap_inner = new FlowMap(*newDig);
            modify_flow(S, T_r, *newDig, *G, leftArcMap, *leftArcMap_inner);
            rightArcMap_inner = new FlowMap(*newDig);
            modify_flow(S, T_r, *newDig, *G, rightArcMap, *rightArcMap_inner);
            left_inner = NULL;
            right_inner = NULL;
        }
        else {
            newDig = G;
            newArcMap = arcMap;
            new_update_base = &update_base;
            leftArcMap_inner = &leftArcMap;
            rightArcMap_inner = &rightArcMap;
            left_inner = left_ele;
            right_inner = right_ele;
        }
        update_dig(lambda_2, *newDig, *newArcMap, *new_update_base);
        // compute original value
        double original_flow_value = submodular::get_cut_value(*newDig, *newArcMap, T_r);

        reverse_newDig = new lemon::ReverseDigraph<lemon::ListDigraph>(*newDig);
        FlowMap newFlowLeftMap(*newDig), newFlowRightMap(*newDig);
        Set T_apostrophe_left, T_apostrophe_right;
        double new_flow_value_left = -1;
        double new_flow_value_right = -1;

        boost::thread * left = NULL, * right = NULL;
        // and concurrently run execute and execute_reverse
        ThreadArgumentPack TAP_Left(*newDig, *newArcMap, *leftArcMap_inner, S, T_r, T_apostrophe_left, new_flow_value_left, newFlowLeftMap, left_inner, NULL, NULL);
        ThreadArgumentPack TAP_Right(*newDig, *newArcMap, *rightArcMap_inner, S, T_r, T_apostrophe_right, new_flow_value_right, newFlowRightMap, NULL, right_inner, reverse_newDig);

        left = new boost::thread(&PMF_R::executePreflow, this, std::ref(TAP_Left));
        right = new boost::thread(&PMF_R::executePreflow_reverse, this, std::ref(TAP_Right));

        if (new_flow_value_left < 0 || new_flow_value_right < 0) {
            boost::unique_lock<boost::mutex> lock(mutex);
            cond.wait(lock);
            if (new_flow_value_left < 0) { // right terminate first
                if (2 * T_apostrophe_right.Cardinality() <= lemon::countNodes(*newDig)) {
                    // interrupt left thread
                    left->interrupt();
                }
            }
            else { // left terminate first
                if (2 * T_apostrophe_left.Cardinality() > lemon::countNodes(*newDig)) {
                    // interrupt right thread
                    right->interrupt();
                }
            }
        }
        left->join();
        right->join();
        // check any error
        if (TAP_Left.thread_exception_ptr)
            std::rethrow_exception(TAP_Left.thread_exception_ptr);
        if (TAP_Right.thread_exception_ptr)
            std::rethrow_exception(TAP_Right.thread_exception_ptr);

        delete left, right;

        double new_flow_value;
        Set T_apostrophe_total;
#if _DEBUG
        // check s \in S and t \in T
        if (!T_apostrophe_left.IsSubSet(T_l)) {
            throw std::logic_error("T_a is not subset of T_l");
        }
#endif
        FlowMap* newFlowMap;
        bool left_contract = true, right_contract = true;

        if (T_apostrophe_left.Cardinality() == 0) {
            newFlowMap = &newFlowRightMap;
            new_flow_value = new_flow_value_right;
            T_apostrophe_total = T_apostrophe_right.Union(T_r);
            left_contract = false;
        }
        else {
            newFlowMap = &newFlowLeftMap;
            new_flow_value = new_flow_value_left;
            T_apostrophe_total = T_apostrophe_left.Union(T_r);
            right_contract = false;
        }

        if(T_apostrophe_total != T_r && T_apostrophe_total != T_l && new_flow_value < original_flow_value - tolerance.epsilon()){
            set_list.push_back(T_apostrophe_total);
            slice(newDig, newArcMap, *new_update_base, T_l, T_apostrophe_total, *leftArcMap_inner, *newFlowMap, lambda_1, lambda_2, left_ele, TAP_Right.ele_reverse_out, left_contract);
            slice(newDig, newArcMap, *new_update_base, T_apostrophe_total, T_r, *newFlowMap, *rightArcMap_inner, lambda_2, lambda_3, TAP_Left.ele_out, right_ele, right_contract);
        }
        else {
            // house keeping
            if (TAP_Left.ele_out)
                delete TAP_Left.ele_out;
            if (TAP_Right.ele_reverse_out)
                delete TAP_Right.ele_reverse_out;
            if (reverse_newDig)
                delete reverse_newDig;
            if (is_contract) {
                delete new_update_base;
                delete leftArcMap_inner;
                delete rightArcMap_inner;
                delete newArcMap;
                delete newDig;
            }
            lambda_list.push_back(lambda_2);
        }
    }
    double PMF_R::compute_lambda_eq_const(Set& T_l, Set& T_r) {
        // compute the target value from original graph
        double target_value = submodular::get_cut_value(*g_ptr, *aM, T_l);
        target_value -= submodular::get_cut_value(*g_ptr, *aM, T_r);
        return target_value;
    }
    void PMF_R::contract(const Set& S, const Set& T, const lemon::ListDigraph& old_G, const ArcMap& old_arcMap, lemon::ListDigraph& G, ArcMap& arcMap) {
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
        std::vector<std::pair<double, double>> s_capacity_map;
        std::vector<std::pair<double, double>> t_capacity_map;
        for (int i = 0; i < tilde_G_size; i++) {
            s_capacity_map.push_back(std::make_pair(0.0, 0.0));
            t_capacity_map.push_back(std::make_pair(0.0, 0.0));
        }
        double s_t_cost = 0, t_s_cost = 0;
        // compute cost
        for (lemon::ListDigraph::ArcIt a(old_G); a != lemon::INVALID; ++a) {
            lemon::ListDigraph::Node u = old_G.source(a);
            lemon::ListDigraph::Node v = old_G.target(a);
            int u_id = old_G.id(u);
            int v_id = old_G.id(v);
            double cv = old_arcMap[a]; // capacity value
            if (cv < tolerance.epsilon())
                continue;
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
        // add source -> others and add others -> sink
        for (int i = 0; i < tilde_G_size; i++) {
            if (i == sink_node_id || i == source_node_id || !G.valid(G.nodeFromId(i)))
                continue;
            addArc(source_node_id, i, s_capacity_map[i].first, G, arcMap);
            if(s_capacity_map[i].second > tolerance.epsilon())
                addArc(i, source_node_id, s_capacity_map[i].second, G, arcMap);
            addArc(i, sink_node_id, t_capacity_map[i].first, G, arcMap);
            if (t_capacity_map[i].second > tolerance.epsilon())
                addArc(sink_node_id, i, t_capacity_map[i].second, G, arcMap);
        }
    }
    inline void PMF_R::addArc(int u, int v, double w, lemon::ListDigraph& G, ArcMap& arcMap) {
        lemon::ListDigraph::Arc newA = G.addArc(G.nodeFromId(u), G.nodeFromId(v));
        arcMap[newA] = w;
    }
    void PMF_R::set_flowMap(const lemon::ListDigraph& G, const Preflow::FlowMap& pfm, FlowMap& flowMap) {
        for (lemon::ListDigraph::ArcIt a(G); a != lemon::INVALID; ++a) {
            flowMap[a] = pfm[a];
        }
    }
    void PMF_R::get_preflow_flowMap(const lemon::ListDigraph& G, const FlowMap& flowMapDic, Preflow::FlowMap& flowMap) {
        for (lemon::ListDigraph::ArcIt a(G); a != lemon::INVALID; ++a) {
            flowMap[a] = flowMapDic[a];
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
    }


    PDT_R::PDT_R(lemon::ListDigraph* g, ArcMap* arcMap):
        _g(g),
        _arcMap(arcMap),
        pmf(g, arcMap, 0, _y_lambda) {}
    void PDT_R::run() {
        partition_list.clear();
        Lambda_list.clear();
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
