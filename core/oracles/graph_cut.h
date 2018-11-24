#pragma once
#include "core/oracles/modular.h"
#include "core/graph.h"
namespace submodular {
   /**
    * \brief directed graph cut oracle class
    */
    template <typename ValueType>
    class DirectedGraphCutOracle : public SubmodularOracle<ValueType> {
    public:
        using value_type = typename ValueTraits<ValueType>::value_type;
        std::string GetName() { return " Directed GraphCut Oracle"; }
        DirectedGraphCutOracle(SimpleGraph<ValueType>& sg_parameter) : sg(sg_parameter)
        {
            this->SetDomain(Set::MakeDense(sg_parameter.GetNodeNumber()));
        }
        value_type Call(const Set& X) {
            auto members = X.GetMembers();
            value_type val = sg.GetCutValueByNames(members);
            return val;
        }


    private:
        SimpleGraph<ValueType> sg;
    };
}