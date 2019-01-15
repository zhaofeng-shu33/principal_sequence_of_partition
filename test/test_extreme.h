#pragma once

#include "core/oracles/modular.h"

namespace submodular {

class EmptyDomain : public SubmodularOracle<float> {
public:
    std::string GetName() { return "emptyDomain"; }
    EmptyDomain()
    {
        this->SetDomain(Set::MakeDense(0));
    }
    float Call(const Set& X) {
        return -1;
    }
};

/**
* $f: 2^{\{1,2,3\}} \to \mathbb{R}, f(\emptyset) = 0, f(\{3\}) = 1, \textrm{others}: f(\{V\}) = 2$ 
*/
template <typename ValueType>
class HyperGraphicalModel : public SubmodularOracle<ValueType> {
public:
    using value_type = typename ValueTraits<ValueType>::value_type;
    std::string GetName() { return ""; }
    HyperGraphicalModel()
    {
        this->SetDomain(Set::MakeDense(3));
    }
    value_type Call(const Set& X) {
        int car = X.Cardinality();
        if (car == 0) // empty set
            return 0;
        else if (car == 1 && X.n_ == 3 && X.bits_[2]==1)
            return 1;
        else
            return 2;
    }
};
//! entropy definition, maximum 6 random variables, widely used in Chen Chuan's article
template <typename ValueType>
class HyperGraphicalModel2 : public SubmodularOracle<ValueType> {
public:
    using value_type = typename ValueTraits<ValueType>::value_type;
    std::string GetName() { return ""; }
    HyperGraphicalModel2()
    {
        this->SetDomain(Set::MakeDense(6));
    }
    value_type Call(const Set& X) {
        auto members = X.GetMembers();
        bool edges[4] = { 0,0,0,0 };
        for (const auto i : members) {
            if (i == 0 || i == 1) {
                edges[0] = 1;
                edges[3] = 1;
            }
            else if (i == 2) {
                edges[0] = 1;
            }
            else if (i == 3 || i == 4) {
                edges[1] = 1;
            }
            else // i == 5
                edges[2] = 1;
        }
        int sum = 0;
        for (const auto i : edges)
            sum += i;
        return sum;
    }
};
}
