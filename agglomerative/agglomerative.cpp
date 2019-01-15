#include "agglomerative.h"
namespace submodular {
    MNBFunction::MNBFunction(std::vector<Set>& p, int index, SubmodularOracle<double>* sf):
    start_index(index),
    partition(p),
    submodular_function(sf)
    {        
    }
    double Call(const Set& X) {
        return 0;
    }
}