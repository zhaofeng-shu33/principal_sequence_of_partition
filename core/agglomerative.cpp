#include "agglomerative.h"
namespace submodular {
    MNBFunction::MNBFunction(std::vector<Set>& p, int index, SubmodularOracle<double>* sf):
    start_index(index),
    partition(p),
    submodular_function(sf)
    {        
        int k = p.size();
        Set domain_set(k);
        for (int i = start_index + 1; i < k; i++)
            domain_set.AddElement(i);
        this->SetDomain(domain_set);
    }
    double MNBFunction::Call(const Set& X) {
        double sum = 0;
        int k = GetNGround();
        Set union_set = Set::FromIndices(k, { (size_t)start_index });
        for (int i = start_index; i < k; i++) {
            if (i > start_index && !X.HasElement(i))
                continue;
            sum -= submodular_function->Call(partition[i]);
            union_set = union_set.Union(partition[i]);
        }
        sum += submodular_function->Call(union_set);
        return 0;
    }
}