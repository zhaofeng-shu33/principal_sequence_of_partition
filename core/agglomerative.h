#pragma once
#include "core/algorithms/sfm_fw.h"
#include "core/oracles/modular.h"
namespace submodular {
    class MNBFunction : public SubmodularOracle<double> {
    public:
        std::string GetName() { return "Min Norm Base Function"; }
        MNBFunction(std::vector<Set>& p, int index, SubmodularOracle<double>* sf);
        double Call(const Set& X);
    private:
        int start_index;
        std::vector<Set> partition;
        SubmodularOracle<double> *submodular_function;
    };
    //! get the critical value and its corresponding partation from the last critical partation
    std::pair<double, std::vector<Set>> agglomerate(std::vector<Set>& p, SubmodularOracle<double>* sf);
}
