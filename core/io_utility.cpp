#include <iostream>
#include <vector>
namespace std {
    ostream& operator<<(ostream& os, const vector<int>& category) {
        os << '[';
        int i = 0;
        for (; i < category.size() - 1; i++)
            os << category[i] << ", ";
        os << category[i];
        os << ']';
        return os;
    }
    ostream& operator<<(ostream& os, const vector<double>& dArray) {
        os << '[';
        int i = 0;
        for (; i < dArray.size() - 1; i++)
            os << dArray[i] << ", ";
        os << dArray[i];
        os << ']';
        return os;
    }
}