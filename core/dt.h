#pragma once
/**
*   Example from Literature "Minimum Average Cost Clustering"
**/
#include "core/algorithms/brute_force.h"
#if USE_LEMON
#include "core/algorithms/sfm_mf.h"
#include "core/graph.h"
#include <lemon/list_graph.h>
#include "set/set_stl.h"
#endif
#include "core/oracles/modular.h"
namespace submodular {

    template <typename ValueType>
    class SampleFunctionPartial : public SubmodularOracle<ValueType> {
    public:
        using value_type = typename ValueTraits<ValueType>::value_type;
        std::string GetName() { return "Sample Function Partial"; }
        SampleFunctionPartial(std::vector<value_type>& xl, SubmodularOracle<ValueType>* sf, value_type lambda = 0) :
            XL(xl),
            lambda_(lambda),
            submodular_function(sf)
        {
            this->SetDomain(Set::MakeDense(xl.size()));
        }
        value_type Call(const Set& X) { // add the constraint X.extend(1)
            Set X_new = X.Extend(1);
            return submodular_function->Call(X_new) - XL.Call(X) - lambda_;
        }
    private:
        value_type lambda_;
        ModularOracle<ValueType> XL;
        SubmodularOracle<ValueType> *submodular_function;
    };
 
    /**
    *   compute the solution to \min_{P} h_{\gamma}(P)
    */
    template <typename ValueType>
    class DilworthTruncation {
    public:
        using value_type = typename ValueTraits<ValueType>::value_type;
        DilworthTruncation(SubmodularOracle<ValueType>* sf, value_type lambda, lemon::ListGraph* g, lemon::ListGraph::EdgeMap<ValueType>* edge_map):
            submodular_function(sf),
            lambda_(lambda), min_value(0), _g(g), _edge_map(edge_map)
        {
            NodeSize = submodular_function->GetN();
        }
        value_type Get_min_value() {
            return min_value;
        }
        stl::Partition& Get_min_partition(){
            return _partition;
        }
        value_type evaluate(stl::Partition& partition) {
			value_type result = get_partition_value(*_g, *_edge_map, partition);
            return result - lambda_ * partition.Cardinality();
        }
		stl::CSet convert_set(const Set& _s) {
			stl::CSet s;
			for (int i : _s.GetMembers())
				s.AddElement(i);
			return s;
		}
        void Run(bool bruteForce = false) {
            min_value = 0;
			_partition.clear();
            std::vector<value_type> xl;
            value_type alpha_l = 0;
            SFMAlgorithm<ValueType>* solver2;
            if(bruteForce)
                solver2 = new BruteForce<ValueType>;
            else{
#if USE_LEMON
                solver2 = new MF<value_type>;
#else
#pragma message("No lemon lib used, only BruteForce algorithm provided.")
                solver2 = new BruteForce<ValueType>;
#endif
            }
            for (int i = 0; i < NodeSize; i++) {
                if(bruteForce){
                    SampleFunctionPartial<ValueType> F1(xl, submodular_function, lambda_);
                    solver2->Minimize(F1);
                }
                else {
#if USE_LEMON                    
                    solver2->Minimize(submodular_function, xl, lambda_, _g, _edge_map);
#else
                    SampleFunctionPartial<ValueType> F1(xl, submodular_function, lambda_);
                    solver2->Minimize(F1);
#endif
                }
                alpha_l = solver2->GetMinimumValue();
#ifdef _DEBUG
                SampleFunctionPartial<ValueType> F1(xl, submodular_function, lambda_);
                Set T2 = solver2->GetMinimizer();
                value_type alpha_2 = F1.Call(T2);
                if (std::abs(alpha_2 - alpha_l) > 1e-5) {
                    std::cout << "error occurs, min value differs from function evaluated at " << T2 << std::endl;
                    std::cout << alpha_l << " != " << alpha_2 << std::endl;
                    std::cout << "lambda = " << lambda_ << std::endl;
                    std::cout << "xl.size = " << xl.size() << std::endl;
                    for (value_type a : xl) {
                        std::cout << a << ',';
                    }
                    std::cout << std::endl;
                    exit(-1);
                }
				
#endif
                stl::CSet Tl = convert_set(solver2->GetMinimizer());
				Tl.AddElement(i);
				_partition = _partition.expand(Tl);
                xl.push_back(alpha_l);
            }
            for (auto it = xl.begin(); it != xl.end(); it++) {
                min_value += *it;
            }
#ifdef _DEBUG            
			value_type min_value_check = evaluate(_partition);
            if (std::abs(min_value - min_value_check) > 1e-4) {
                std::cout << "min_value_check error: " << std::endl;
                std::cout << min_value << std::endl;
                std::cout << min_value_check << std::endl;
                exit(-1);
            }
#endif
            delete solver2;
        }
    private:
        SubmodularOracle<ValueType> *submodular_function;
		lemon::ListGraph* _g;
		lemon::ListGraph::EdgeMap<ValueType>* _edge_map;
        value_type min_value;
        stl::Partition _partition;
        value_type lambda_;
        int NodeSize;
    };
	// converter function (temporary use).
	stl::Partition convert_partition(const std::vector<Set>& _old_p) {
		stl::Partition _p;
		for (const Set& s : _old_p) {
			stl::CSet _s;
			for (int a : s.GetMembers())
				_s.AddElement(a);
			_p.AddElement(_s);
		}
		return _p;
	}
    /**
    * computing principal sequence of partition for given submodular function
    */
    template <typename ValueType>
    class PSP {
    public:
        using value_type = typename ValueTraits<ValueType>::value_type;
        PSP(SubmodularOracle<ValueType>* sf, lemon::ListGraph* g, lemon::ListGraph::EdgeMap<ValueType>* edge_map ) :submodular_function(sf), _g(g), _edge_map(edge_map)
        {
            NodeSize = submodular_function->GetN();
            critical_values.resize(NodeSize);
            psp.resize(NodeSize);
        }
        //! evaluate and find the finest partition with $\abs{\P} > \texttt{partition_num}$
		stl::Partition split(stl::Partition& Q, stl::Partition& P, int partition_num, bool bruteForce = false)
        {
            if (Q.Cardinality() == P.Cardinality()) {
                throw std::logic_error("Q and P have the same size");
            }
            value_type gamma_apostrophe = (evaluate(P) - evaluate(Q)) / (P.Cardinality() - Q.Cardinality());
            value_type h_apostrophe = (P.Cardinality() * evaluate(Q) - Q.Cardinality() * evaluate(P)) / (P.Cardinality() - Q.Cardinality());
            DilworthTruncation<value_type> dt(submodular_function, gamma_apostrophe, _g, _edge_map);
            dt.Run(bruteForce);
            value_type min_value = dt.Get_min_value();
			stl::Partition P_apostrophe = dt.Get_min_partition();
            if (min_value > h_apostrophe - 1e-4) {
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
        void split(stl::Partition& Q, stl::Partition& P, bool bruteForce = false) {
            if (Q.Cardinality() == P.Cardinality()) {
                throw std::logic_error("Q and P have the same size");
            }
            value_type gamma_apostrophe = (evaluate(P) - evaluate(Q)) / (P.Cardinality() - Q.Cardinality());
            value_type h_apostrophe = (P.Cardinality() * evaluate(Q) - Q.Cardinality() * evaluate(P)) / (P.Cardinality() - Q.Cardinality());
            DilworthTruncation<value_type> dt(submodular_function, gamma_apostrophe, _g, _edge_map);
            dt.Run(bruteForce);
            value_type min_value = dt.Get_min_value();
			stl::Partition P_apostrophe = dt.Get_min_partition();
            if (min_value > h_apostrophe-1e-4) {
                critical_values[Q.Cardinality() - 1] = gamma_apostrophe;
            }
            else {                
                psp[P_apostrophe.Cardinality() - 1] = P_apostrophe;
                try{
                    split(Q, P_apostrophe, bruteForce);
                    split(P_apostrophe, P, bruteForce);
                }
                catch (std::exception e) {
                    std::cout << "h: " << h_apostrophe << std::endl;
                    std::cout << "min_value: " << min_value << std::endl;
                    exit(-1);
                }
            }
        }

		stl::Partition run(int partition_num, bool bruteForce = false) {
			stl::CSet V = stl::CSet::MakeDense(NodeSize);
			stl::Partition Q, P;
            Q.AddElement(V);
			P = stl::Partition::makeFine(NodeSize);
			return split(Q, P, partition_num, bruteForce);
        }
        void run(bool bruteForce = false) {
			stl::CSet V = stl::CSet::MakeDense(NodeSize);
            stl::Partition Q, P;
            Q.AddElement(V);
            P = stl::Partition::makeFine(NodeSize);
            psp[0] = Q;
            psp[P.Cardinality()-1] = P;
            split(Q, P, bruteForce);
        }
        std::vector<value_type>& Get_critical_values() {
            return critical_values;
        }
        std::vector<stl::Partition>& Get_psp() {
            return psp;
        }
    private:
        //! evalute the submodular function at the given partition
        value_type evaluate(const stl::Partition& P) {
			return get_partition_value(*_g, *_edge_map, P);
        }

        SubmodularOracle<ValueType> *submodular_function;
        int NodeSize;
        std::vector<value_type> critical_values;
        std::vector<stl::Partition> psp;
		lemon::ListGraph* _g;
		lemon::ListGraph::EdgeMap<ValueType>* _edge_map;
    };
}
