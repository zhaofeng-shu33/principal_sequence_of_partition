#include "core/base.h"
namespace submodular {
OrderType LinearOrder(std::size_t n) {
  OrderType order(n);
  std::iota(order.begin(), order.end(), 0);
  return order;
}

OrderType LinearOrder(const Set& X) {
  return X.GetMembers();
}
}