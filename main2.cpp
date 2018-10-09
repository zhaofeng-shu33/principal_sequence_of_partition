/**
* \author: zhaofeng-shu33
*/
#include "dt.h"
namespace submodular {
    template <typename ValueType>
    class SampleFunction : public SubmodularOracle<ValueType> {
    public:
        using value_type = typename ValueTraits<ValueType>::value_type;
        std::string GetName() { return ""; }
        SampleFunction()
        {
            this->SetDomain(Set::MakeDense(2));
        }
        value_type Call(const Set& X) {
            int car = X.Cardinality();
            if (car == 0)
                return 0;
            else if (car == 2)
                return 19;
            else if (X.HasElement(0))
                return 12;
            else
                return 8;            
        }
    };

    template <typename ValueType>
    class HyperGraphicalModel : public SubmodularOracle<ValueType> {
    public:
        std::string GetName() { return ""; }
        HyperGraphicalModel()
        {
            this->SetDomain(Set::MakeDense(3));
        }
        value_type Call(const Set& X) {
            int car = X.Cardinality();
            if (car == 0) // empty set
                return 0;
            else if (car == 1 && X.n_ == 3)
                return 1;
            else
                return 2;
        }
    };

  
}
#define LAMBDA 2
using namespace submodular;
int main(){
    // DilworthTruncation<float>(&SampleFunction<float>(), LAMBDA).Run();
    // optimal value is 16
    // optimal partition is { 0 }, { 1 },

    DilworthTruncation<float>(&HyperGraphicalModel<float>(), 1.5).Run();

    return 0;
}