#include "io_utility.h"
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
}