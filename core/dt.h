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
#if USE_EIGEN3
#include "core/agglomerative.h"
#endif
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
        DilworthTruncation(SubmodularOracle<ValueType>* sf, value_type lambda) :
            submodular_function(sf),
            lambda_(lambda), min_value(0) 
        {
            NodeSize = submodular_function->GetN();
        }
        value_type Get_min_value() {
            return min_value;
        }
        std::vector<Set>& Get_min_partition(){
            return lastPartition;
        }
        value_type evaluate(std::vector<Set>& partition) {
            value_type result = 0;
            for (const Set& i : partition) {
                result += submodular_function->Call(i);
            }
            return result - lambda_ * partition.size();
        }
        void Run(bool bruteForce = false) {
            min_value = 0;
            lastPartition.resize(0);
            currentPartition.resize(0);
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
                    solver2->Minimize(submodular_function, xl, lambda_);
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
                Set Tl = solver2->GetMinimizer().Extend(1);
                Set Ul = Tl;
                for (std::vector<Set>::iterator it = lastPartition.begin(); it != lastPartition.end(); it++) {
                    Set intersect = Tl.Intersection(it->Extend());
                    if (intersect.Cardinality() > 0)
                        Ul = Ul.Union(it->Extend());
                }
                for (std::vector<Set>::iterator it = lastPartition.begin(); it != lastPartition.end(); it++) {
                    Set intersect = Tl.Intersection(it->Extend());
                    if (intersect.Cardinality() == 0) {
                        currentPartition.push_back(it->Extend());
                    }
                }
                currentPartition.push_back(Ul);
                lastPartition = currentPartition;
                currentPartition.clear();
                xl.push_back(alpha_l);
            }
            for (auto it = xl.begin(); it != xl.end(); it++) {
                min_value += *it;
            }
            value_type min_value_check = evaluate(lastPartition);
            if (std::abs(min_value - min_value_check) > 1e-4) {
                std::cout << "min_value_check error: " << std::endl;
                std::cout << min_value << std::endl;
                std::cout << min_value_check << std::endl;
                exit(-1);
            }
            delete solver2;
        }
    private:
        SubmodularOracle<ValueType> *submodular_function;
        value_type min_value;
        std::vector<Set> lastPartition, currentPartition;
        value_type lambda_;
        int NodeSize;
    };
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
        std::vector<Set> split(std::vector<Set>& Q, std::vector<Set>& P, int partition_num, bool bruteForce = false)
        {
            if (Q.size() == P.size()) {
                throw std::logic_error("Q and P have the same size");
            }
            value_type gamma_apostrophe = (evaluate(P) - evaluate(Q)) / (P.size() - Q.size());
            value_type h_apostrophe = (P.size() * evaluate(Q) - Q.size() * evaluate(P)) / (P.size() - Q.size());
            DilworthTruncation<value_type> dt(submodular_function, gamma_apostrophe);
            dt.Run(bruteForce);
            value_type min_value = dt.Get_min_value();
            std::vector<Set> P_apostrophe = dt.Get_min_partition();
            if (min_value>h_apostrophe - 1e-4) {
                return P;
            }
            else {
                if (P_apostrophe.size() == partition_num) {
                    return P_apostrophe;
                }
                else if (P_apostrophe.size() < partition_num) {
                    return split(P_apostrophe, P, partition_num, bruteForce);
                }
                else {
                    return split(Q, P_apostrophe, partition_num, bruteForce);
                }
            }
        }
        //! |Q| < |P|
        void split(std::vector<Set>& Q, std::vector<Set>& P, bool bruteForce = false) {
            if (Q.size() == P.size()) {
                throw std::logic_error("Q and P have the same size");
            }
            value_type gamma_apostrophe = (evaluate(P) - evaluate(Q)) / (P.size() - Q.size());
            value_type h_apostrophe = (P.size() * evaluate(Q) - Q.size() * evaluate(P)) / (P.size() - Q.size());
            DilworthTruncation<value_type> dt(submodular_function, gamma_apostrophe);
            dt.Run(bruteForce);
            value_type min_value = dt.Get_min_value();
            std::vector<Set> P_apostrophe = dt.Get_min_partition();
            if (min_value>h_apostrophe-1e-4) {
                critical_values[Q.size() - 1] = gamma_apostrophe;
            }
            else {                
                psp[P_apostrophe.size() - 1] = P_apostrophe;
                try{
                    split(Q, P_apostrophe, bruteForce);
                    split(P_apostrophe, P, bruteForce);
                }
                catch (std::exception e) {
                    value_type q_value = dt.evaluate(Q);
                    value_type p_a_value = dt.evaluate(P_apostrophe);
                    value_type p_value = dt.evaluate(P);
                    std::cout << Q.size() << " at " << Q << " = " << q_value << std::endl;
                    std::cout << P_apostrophe.size() << " at " << P_apostrophe << " = " << p_a_value << std::endl;
                    std::cout << P.size() << " at " << P << " = " << p_value << std::endl;
                    std::cout << "h: " << h_apostrophe << std::endl;
                    std::cout << "min_value: " << min_value << std::endl;
                    exit(-1);
                }
            }
        }
        std::vector<Set> run(int partition_num, bool bruteForce = false) {
            Set V = Set::MakeDense(NodeSize);
            Set Empt;
            std::vector<Set> Q, P;
            Q.push_back(V);
            for (int i = 0; i < NodeSize; i++) {
                Set EmptyExceptOne(NodeSize);
                EmptyExceptOne.AddElement(i);
                P.push_back(EmptyExceptOne);
            }
            return split(Q, P, partition_num, bruteForce);
        }
        void run(bool bruteForce = false) {
            Set V = Set::MakeDense(NodeSize);
            std::vector<Set> Q, P;
            Q.push_back(V);
            P = Set::MakeFine(NodeSize);
            psp[0] = Q;
            psp[P.size()-1] = P;
            split(Q, P, bruteForce);
        }
#if USE_EIGEN3
        void agglomerative_run() {
            std::vector<Set> P;
            P = Set::MakeFine(NodeSize);
            int s = P.size() - 1;
            psp[s] = P;
            while (s > 0) {
                std::pair<double, std::vector<Set>> result;
                result = agglomerate(psp[s], submodular_function);
                s = result.second.size() - 1;
                critical_values[s] = result.first;
                psp[s] = result.second;
            }
        }
#endif
        std::vector<value_type>& Get_critical_values() {
            return critical_values;
        }
        std::vector<std::vector<Set>>& Get_psp() {
            return psp;
        }
    private:
        //! evalute the submodular function at the given partition
        value_type evaluate(const std::vector<Set>& P) {
            value_type sum = 0;
            for (const Set& s : P)
                sum += submodular_function->Call(s);
            return sum;
        }

        SubmodularOracle<ValueType> *submodular_function;
        int NodeSize;
        std::vector<value_type> critical_values;
        std::vector<std::vector<Set>> psp;
		lemon::ListGraph* _g;
		lemon::ListGraph::EdgeMap<ValueType>* _edge_map;
    };
}
