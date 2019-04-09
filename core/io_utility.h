#pragma once
#include <iostream>
#include <vector>
namespace std {
    ostream& operator<<(ostream& os, const vector<int>& category);
    ostream& operator<<(ostream& os, const vector<double>& dArray);
}