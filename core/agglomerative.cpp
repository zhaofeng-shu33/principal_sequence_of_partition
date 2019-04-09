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
        Set union_set = Set::MakeEmpty(partition[0].n_);
        for (int i = start_index; i < k; i++) {
            if (i > start_index && !X.HasElement(i))
                continue;
            sum -= submodular_function->Call(partition[i]);
            union_set = union_set.Union(partition[i]);
        }
        sum += submodular_function->Call(union_set);
        return sum;
    }
    std::pair<double, std::vector<Set>> agglomerate(std::vector<Set>& p, SubmodularOracle<double>* sf) {
        std::vector<std::vector<double>> x_data_list;
        int k = p.size();
        FWRobust<double> solver;
        double gamma_minus = 0;
        for (int j = 0; j < k - 1; j++) {
            MNBFunction mnb(p, j, sf);
            solver.Minimize(mnb);
            std::vector<double> x_data = solver.get_x_data();
            x_data_list.push_back(x_data);
            for (int i = 0; i < (k - j - 1); i++)
                gamma_minus = std::min(gamma_minus, x_data[i]);
        }
        std::vector<Set> P_apostrophe;
        Set P_apostrophe_union = Set::MakeEmpty(p[0].n_);
        for (int j = 0; j < k; j++) {
            if (P_apostrophe_union.Union(p[j]).Cardinality() > P_apostrophe_union.Cardinality()) {
                Set C_j_candidate = p[j];
                for (int i = j + 1; i < k; i++) {
                    if (x_data_list[j][i - (j+1)] <= gamma_minus + 1e-3)
                        C_j_candidate = C_j_candidate.Union(p[i]);
                }
                P_apostrophe.push_back(C_j_candidate);
                P_apostrophe_union = P_apostrophe_union.Union(C_j_candidate);
            }
        }
        return std::pair<double, std::vector<Set>>(-gamma_minus, P_apostrophe);
    }
}