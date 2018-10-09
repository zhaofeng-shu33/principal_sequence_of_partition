/**
*   Example from Literature "Minimum Average Cost Clustering"
**/
#include "core/oracles/iwata_test_function.h"
#include "core/algorithms/sfm_fw.h"
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
                        Ul = Ul.Union(*it);
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
            std::cout << "optimal value is " << min_value << std::endl;
            std::cout << "optimal partition is ";
            for (std::vector<Set>::iterator it = lastPartition.begin(); it != lastPartition.end(); it++) {
                std::cout << *it << ',';
            }
            std::cout << std::endl;
        }
    private:
        SubmodularOracle<ValueType> *submodular_function;
        value_type min_value;
        std::vector<Set> lastPartition, currentPartition;
        value_type lambda_;
        int NodeSize;
    };

}
