#pragma once
/**
*   Example from Literature "Minimum Average Cost Clustering"
**/
#include "core/oracles/iwata_test_function.h"
#include "core/algorithms/brute_force.h"
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
        void Run() {
            std::vector<value_type> xl;
            value_type alpha_l = 0;
            BruteForce<value_type> solver2;
            for (int i = 0; i < NodeSize; i++) {
                SampleFunctionPartial<ValueType> F1(xl, submodular_function, lambda_);
                solver2.Minimize(F1);
                alpha_l = solver2.GetMinimumValue();
                Set Tl = solver2.GetMinimizer().Extend(1);
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
        PSP(SubmodularOracle<ValueType>* sf) :submodular_function(sf)
        {
            NodeSize = submodular_function->GetN();
            critical_values.resize(NodeSize);
            psp.resize(NodeSize);
        }
        void split(std::vector<Set>& Q, std::vector<Set>& P) {
            value_type gamma_apostrophe = (evaluate(P) - evaluate(Q)) / (P.size() - Q.size());
            value_type h_apostrophe = (P.size() * evaluate(Q) - Q.size() * evaluate(P)) / (P.size() - Q.size());
            DilworthTruncation<value_type> dt(submodular_function, gamma_apostrophe);
            dt.Run();
            value_type min_value = dt.Get_min_value();
            std::vector<Set> P_apostrophe = dt.Get_min_partition();
            if ( abs(min_value-h_apostrophe)<1e-4) {
                critical_values[Q.size() - 1] = gamma_apostrophe;
                psp[P_apostrophe.size() - 1] = P_apostrophe;
            }
            else {                
                split(Q, P_apostrophe);
                split(P_apostrophe, P);
            }
        }
        void run() {
            Set V = Set::MakeDense(NodeSize);
            Set Empt;
            std::vector<Set> Q, P;
            Q.push_back(V);
            for (int i = 0; i < NodeSize; i++) {
                Set EmptyExceptOne(NodeSize);
                EmptyExceptOne.AddElement(i);
                P.push_back(EmptyExceptOne);
            }
            psp[0] = Q;
            split(Q, P);
        }
        std::vector<value_type>& Get_critical_values() {
            return critical_values;
        }
        std::vector<std::vector<Set>>& Get_psp() {
            return psp;
        }
    private:
        //! evalute the submodular function at the given partition
        value_type evaluate(std::vector<Set>& P) {
            value_type sum = 0;
            for (Set& s : P)
                sum += submodular_function->Call(s);
            return sum;
        }

        SubmodularOracle<ValueType> *submodular_function;
        int NodeSize;
        std::vector<value_type> critical_values;
        std::vector<std::vector<Set>> psp;
    };
}
