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
}
