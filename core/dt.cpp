#include <sstream>
#include "core/dt.h"

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
        
    void DilworthTruncation::run(bool bruteForce) {
        min_value = 0;
        _partition.clear();
        std::vector<double> xl;
        double alpha_l = 0;
        SFMAlgorithm* solver2;
        if(bruteForce)
            solver2 = new BruteForce;
        else{
#if USE_LEMON
            solver2 = new MF;
#else
#pragma message("No lemon lib used, only BruteForce algorithm provided.")
            solver2 = new BruteForce;
#endif
        }
        for (int i = 0; i < NodeSize; i++) {
            solver2->Minimize(xl, lambda_, _g, _edge_map);
            alpha_l = solver2->GetMinimumValue();
            stl::CSet Tl = solver2->GetMinimizer();
            Tl.AddElement(i);
            _partition = _partition.expand(Tl);
            xl.push_back(alpha_l);
        }
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
        delete solver2;
    }
    
    double DilworthTruncation::evaluate(stl::Partition& partition) {
        double result = get_partition_value(*_g, *_edge_map, partition);
        return result - lambda_ * partition.Cardinality();
    }    
    
    PSP::PSP(Digraph* g, ArcMap* edge_map ) : _g(g), _edge_map(edge_map)
    {
        NodeSize = lemon::countNodes(*_g);
        critical_values.resize(NodeSize);
        psp.resize(NodeSize);
    }    
    
    stl::Partition PSP::split(stl::Partition& Q, stl::Partition& P, int partition_num, bool bruteForce)
    {
        if (Q.Cardinality() == P.Cardinality()) {
            throw std::logic_error("Q and P have the same size");
        }
        double gamma_apostrophe = (evaluate(P) - evaluate(Q)) / (P.Cardinality() - Q.Cardinality());
        double h_apostrophe = (P.Cardinality() * evaluate(Q) - Q.Cardinality() * evaluate(P)) / (P.Cardinality() - Q.Cardinality());
        DilworthTruncation dt(gamma_apostrophe, _g, _edge_map);
        dt.run(bruteForce);
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
                return split(P_apostrophe, P, partition_num, bruteForce);
            }
            else {
                return split(Q, P_apostrophe, partition_num, bruteForce);
            }
        }
    }
        //! |Q| < |P|
    void PSP::split(stl::Partition& Q, stl::Partition& P, bool bruteForce) {
        if (Q.Cardinality() == P.Cardinality()) {
            throw std::logic_error("Q and P have the same size");
        }
        double gamma_apostrophe = (evaluate(P) - evaluate(Q)) / (P.Cardinality() - Q.Cardinality());
        double h_apostrophe = (P.Cardinality() * evaluate(Q) - Q.Cardinality() * evaluate(P)) / (P.Cardinality() - Q.Cardinality());
        DilworthTruncation dt(gamma_apostrophe, _g, _edge_map);
        dt.run(bruteForce);
        double min_value = dt.get_min_value();
        stl::Partition P_apostrophe = dt.get_min_partition();
        if (min_value > h_apostrophe - _tolerance.epsilon()) {
            critical_values[Q.Cardinality() - 1] = gamma_apostrophe;
        }
        else {                
            psp[P_apostrophe.Cardinality() - 1] = P_apostrophe;
            try{
                split(Q, P_apostrophe, bruteForce);
                split(P_apostrophe, P, bruteForce);
            }
            catch (std::exception e) {
                std::cout << e.what() << std::endl;
                double q_value = dt.evaluate(Q);
                double p_a_value = dt.evaluate(P_apostrophe);
                double p_value = dt.evaluate(P);
                std::cout << Q.Cardinality() << " at " << Q << " = " << q_value << std::endl;
                std::cout << P_apostrophe.Cardinality() << " at " << P_apostrophe << " = " << p_a_value << std::endl;
                std::cout << P.Cardinality() << " at " << P << " = " << p_value << std::endl;

                std::cout << "h: " << h_apostrophe << std::endl;
                std::cout << "min_value: " << min_value << std::endl;
                exit(-1);
            }
        }
    }

    stl::Partition PSP::run(int partition_num, bool bruteForce) {
        stl::CSet V = stl::CSet::MakeDense(NodeSize);
        stl::Partition Q, P;
        Q.AddElement(V);
        P = stl::Partition::makeFine(NodeSize);
        return split(Q, P, partition_num, bruteForce);
    }
    
    void PSP::run(bool bruteForce) {
        stl::CSet V = stl::CSet::MakeDense(NodeSize);
        stl::Partition Q, P;
        Q.AddElement(V);
        P = stl::Partition::makeFine(NodeSize);
        psp[0] = Q;
        psp[P.Cardinality()-1] = P;
        split(Q, P, bruteForce);
    }    
    
    std::vector<double>& PSP::get_critical_values() {
        return critical_values;
    }
    
    std::vector<stl::Partition>& PSP::get_psp() {
        return psp;
    }    
    
    double PSP::evaluate(const stl::Partition& P) {
        return get_partition_value(*_g, *_edge_map, P);
    }    
}