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
        DirectedGraphCutOracle(SimpleGraph<ValueType>& sg_parameter) : sg(sg_parameter){}
        value_type Call(const Set& X) {
            auto members = X.GetMembers();
            value_type val(0);
            if (!graph_.HasAuxiliaryNodes()) {
                val = sg.GetCutValueByNames(members);
            }
            return value;
        }


    private:
        SimpleGraph<ValueType> sg;
    };
}