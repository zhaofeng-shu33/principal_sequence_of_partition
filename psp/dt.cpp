#include <sstream>
#include "dt.h"
namespace submodular {

    DilworthTruncation::DilworthTruncation(double lambda, Digraph* g, ArcMap* edge_map):
        lambda_(lambda), min_value(0), _g(g), _edge_map(edge_map)
    {
        NodeSize = lemon::countNodes(*_g);
    }
    double DilworthTruncation::get_min_value() {
        return min_value;
    }
    stl::Partition& DilworthTruncation::get_min_partition(){
        return _partition;
    }
        
    void DilworthTruncation::run() {
        _partition.clear();
        std::vector<double> xl;
        double alpha_l = 0;
        for (int i = 0; i < NodeSize; i++) {
            minimize(xl);
            alpha_l = min_value;
            Tl.AddElement(i);
            _partition = _partition.expand(Tl);
            xl.push_back(alpha_l);
        }
        min_value = 0;
        for (auto it = xl.begin(); it != xl.end(); it++) {
            min_value += *it;
        }
#ifdef _DEBUG            
        double min_value_check = evaluate(_partition);
        if (std::abs(min_value - min_value_check) > 1e-4) {
            std::stringstream ss;
            ss << "min_value_check error: " << min_value << ' ' << min_value_check;
            throw std::logic_error(ss.str());
        }
#endif
    }
    
    void DilworthTruncation::minimize(std::vector<double>& xl) {

        int graph_size = xl.size();

        lemon::ListDigraph::NodeMap<bool> node_filter(*_g);
        for (int i = 0; i <= graph_size; i++) {
            node_filter[_g->nodeFromId(i)] = true;
        }
        for (int i = graph_size + 1; i < lemon::countNodes(*_g); i++) {
            node_filter[_g->nodeFromId(i)] = false;
        }
        SubDigraph subgraph(*_g, node_filter);
        std::vector<double> sink_node_cost_map;
        sink_node_cost_map.resize(graph_size);
        for (lemon::FilterNodes<Digraph>::InArcIt a(subgraph, subgraph.nodeFromId(graph_size)); a != lemon::INVALID; ++a) {
            int node_id = subgraph.id(subgraph.source(a));
            sink_node_cost_map[node_id] = (*_edge_map)[a];
        }
        subgraph.disable(_g->nodeFromId(graph_size));

        // construct source node and sink node (inplace)
        Digraph::Node source_node = subgraph.addNode();
        Digraph::Node sink_node = subgraph.addNode();

        // construct edge cost map related with source_node and sink_node
        double const_difference = lambda_;
        for (int i = 0; i < graph_size; i++) {
            Digraph::Arc arc;
            if (xl[i] < 0) {
                arc = subgraph.addArc(source_node, subgraph.nodeFromId(i));
                (*_edge_map)[arc] = -xl[i];
            }
            double sink_cost = std::max<double>(0, xl[i]) + sink_node_cost_map[i];
            if (sink_cost > 0) {
                arc = subgraph.addArc(subgraph.nodeFromId(i), sink_node);
                (*_edge_map)[arc] = sink_cost;
            }
            const_difference += std::max<double>(0, xl[i]);
        }
        subgraph.enable(source_node);
        subgraph.enable(sink_node);
        Preflow pf(subgraph, *_edge_map, source_node, sink_node);
        pf.run();
        double minimum_value = pf.flowValue() - const_difference;
        stl::CSet X;
        for (int v = 0; v < graph_size; ++v) {
            if (!pf.minCut(subgraph.nodeFromId(v)))
                X.AddElement(v);
        }
        // house keeping, map is handled automatically

#ifdef  _DEBUG    
        subgraph.disable(source_node);
        subgraph.disable(sink_node);
        stl::CSet _X;
        std::copy(X.begin(), X.end(), std::back_inserter(_X));
        _X.AddElement(graph_size);
        subgraph.enable(_g->nodeFromId(graph_size));
        double minimum_value_2 = -lambda_ + get_cut_value(subgraph, *_edge_map, _X);
        for (int i : X.GetMembers())
            minimum_value_2 -= xl[i];
        if (std::abs(minimum_value - minimum_value_2) > 1e-5) {
            std::stringstream ss;
            ss << "maxflow value differs: " << minimum_value << " != " << minimum_value_2;
            throw std::logic_error(ss.str());
        }
#endif
        subgraph.erase(source_node);
        subgraph.erase(sink_node);
        Tl = X;
        min_value = minimum_value;
    }

    double DilworthTruncation::evaluate(stl::Partition& partition) {
        double result = get_partition_value(*_g, *_edge_map, partition);
        return result - lambda_ * partition.Cardinality();
    }    
    
    DT::DT(Digraph* g, ArcMap* edge_map ) : _g(g), _edge_map(edge_map)
    {
        NodeSize = lemon::countNodes(*_g);
    }    
    
    stl::Partition DT::split(stl::Partition& Q, stl::Partition& P, int partition_num){
        if (Q.Cardinality() == P.Cardinality()) {
            throw std::logic_error("Q and P have the same size");
        }
        double gamma_apostrophe = (evaluate(P) - evaluate(Q)) / (P.Cardinality() - Q.Cardinality());
        double h_apostrophe = (P.Cardinality() * evaluate(Q) - Q.Cardinality() * evaluate(P)) / (P.Cardinality() - Q.Cardinality());
        DilworthTruncation dt(gamma_apostrophe, _g, _edge_map);
        dt.run();
        double min_value = dt.get_min_value();
        stl::Partition P_apostrophe = dt.get_min_partition();
        if (min_value > h_apostrophe - _tolerance.epsilon()) {
            return P;
        }
        else {
            if (P_apostrophe.Cardinality() == partition_num) {
                return P_apostrophe;
            }
            else if (P_apostrophe.Cardinality() < partition_num) {
                return split(P_apostrophe, P, partition_num);
            }
            else {
                return split(Q, P_apostrophe, partition_num);
            }
        }
    }
        //! |Q| < |P|
    void DT::split(stl::Partition& Q, stl::Partition& P) {
        double gamma_apostrophe = (evaluate(P) - evaluate(Q)) / (P.Cardinality() - Q.Cardinality());
        double h_apostrophe = (P.Cardinality() * evaluate(Q) - Q.Cardinality() * evaluate(P)) / (P.Cardinality() - Q.Cardinality());
        DilworthTruncation dt(gamma_apostrophe, _g, _edge_map);
        dt.run();
        double min_value = dt.get_min_value();
        stl::Partition P_apostrophe = dt.get_min_partition();
        if (min_value > h_apostrophe - _tolerance.epsilon() || Q.Cardinality() == P_apostrophe.Cardinality() || P.Cardinality() == P_apostrophe.Cardinality()) {
            critical_values.push_back(gamma_apostrophe);
        }
        else {                
            psp.push_back(P_apostrophe);
            split(Q, P_apostrophe);
            split(P_apostrophe, P);
        }
    }

    stl::Partition DT::run(int partition_num) {
        stl::CSet V = stl::CSet::MakeDense(NodeSize);
        stl::Partition Q, P;
        Q.AddElement(V);
        P = stl::Partition::makeFine(NodeSize);
        return split(Q, P, partition_num);
    }
    
    void DT::run() {
        stl::CSet V = stl::CSet::MakeDense(NodeSize);
        stl::Partition Q, P;
        Q.AddElement(V);
        P = stl::Partition::makeFine(NodeSize);
        psp.push_back(Q);
        psp.push_back(P);
        split(Q, P);
        critical_values.sort();
        auto partition_compare = [](const stl::Partition & p1, const stl::Partition & p2)
            {return p1.Cardinality() < p2.Cardinality(); };
        psp.sort(partition_compare);
    }    
    
    std::list<double>& DT::get_critical_values() {
        return critical_values;
    }
    
    std::list<stl::Partition>& DT::get_psp() {
        return psp;
    }    
    
    double DT::evaluate(const stl::Partition& P) {
        return get_partition_value(*_g, *_edge_map, P);
    }    
}