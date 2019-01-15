// Copyright 2018 Kentaro Minami. All Rights Reserved.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// =============================================================================

#include "core/set_utils.h"

namespace submodular {

std::ostream& operator<<(std::ostream& os, std::vector<submodular::Set> partition) {
    os << '{';
    for (const submodular::Set& s : partition)
        os << s;
    os << '}';
    return os;
}



Set Set::FromIndices(std::size_t n, const std::vector<element_type>& indices) {
  Set X(n);
  for (const auto& i: indices) {
    if (i >= n) {
      throw std::range_error("Set::FromIndices");
    }
    else {
      X.bits_[i] = 1;
    }
  }
  return X;
}
std::vector<Set> Set::MakeFine(std::size_t n) {
    Set Empt;
    std::vector<Set> P;
    for (int i = 0; i < n; i++) {
        Set EmptyExceptOne(n);
        EmptyExceptOne.AddElement(i);
        P.push_back(EmptyExceptOne);
    }
    return P;
}


Set::Set(std::size_t n, unsigned long val)
  : n_(n), bits_(n, 0)
{
  for (std::size_t i = 0; i < n; ++i) {
    bits_[i] = (val >> i & 1) == 1 ? 1 : 0;
  }
}

bool Set::HasElement(element_type i) const {
  return static_cast<bool>(bits_[i]);
}

Set Set::Copy() const {
  Set X; X.n_ = n_; X.bits_ = bits_;
  return X;
}

Set Set::Complement() const{
  Set X = Copy();
  for (auto& b: X.bits_) {
    b = !b;
  }
  return X;
}

Set Set::Union(const Set& X) const {
  Set X_new(n_);
  for (std::size_t i = 0; i < n_; ++i) {
    if (X.HasElement(i) || this->HasElement(i)) {
      X_new.bits_[i] = 1;
    }
  }
  return X_new;
}

Set Set::Intersection(const Set& X) const {
  Set X_new(n_);
  for (std::size_t i = 0; i < n_; ++i) {
    if (X.HasElement(i) && this->HasElement(i)) {
      X_new.bits_[i] = 1;
    }
  }
  return X_new;
}

std::vector<element_type> Set::GetMembers() const {
  std::vector<element_type> members;
  for (std::size_t i = 0; i < n_; ++i) {
    if (HasElement(i)) {
      members.push_back(i);
    }
  }
  return members;
}

// NOTE: this is used as follows:
// auto members = X.GetMembers(); auto inverse = X.GetInverseMap();
// for (element_type i: members) {
//   assert(i == members[inverse[i]]);
// }
std::vector<std::size_t> Set::GetInverseMap() const {
  std::vector<std::size_t> inverse(n_, n_);
  std::size_t pos = 0;
  for (std::size_t i = 0; i < n_; ++i) {
    if (HasElement(i)) {
      inverse[i] = pos;
      pos++;
    }
  }
  return inverse;
}

std::size_t Set::Cardinality() const {
  std::size_t c = 0;
  for (const auto& b: bits_) {
    if (b) { c++; }
  }
  return c;
}

void Set::AddElement(element_type i) {
  bits_.at(i) = 1;
}

void Set::RemoveElement(element_type i) {
  bits_.at(i) = 0;
}

Set Set::MakeDense(std::size_t n) {
  Set X;
  X.n_ = n;
  X.bits_ = std::vector<char>(n, 1);
  return X;
}

Set Set::MakeEmpty(std::size_t n) {
  Set X(n);
  return X;
}

std::ostream& operator << (std::ostream& stream, const Set& X) {
  auto members = X.GetMembers();
  stream << "{";
  for (std::size_t pos = 0; pos < members.size(); ++pos) {
    stream << members[pos];
    if (pos < members.size() - 1) {
      stream << ", ";
    }
  }
  stream << "}";
  return stream;
}
bool operator == (const Set& set_1, const Set& set_2) {
    bool set_1_less = true;
    if (set_2.n_ > set_1.n_)
        set_1_less = false;
    int min_len = set_1_less ? set_2.n_ : set_1.n_;
    for(int i = 0; i < min_len; i++)
        if ((bool)set_1.bits_[i] != (bool)set_2.bits_[i]) {
            return false;
        }
    if(set_1_less){
        for(int i=min_len; i < set_2.n_; i++){
            if ((bool)set_2.bits_[i]) {
                return false;
            }
        }
    }
    else {
        for (int i = min_len; i < set_1.n_; i++) {
            if ((bool)set_1.bits_[i]) {
                return false;
            }
        }
    }
    return true;
}
//bool operator == (const Set& lhs, const Set& rhs) {
//  if (lhs.n_ != rhs.n_) {
//    return false;
//  }
//  for (std::size_t i = 0; i < lhs.n_; ++i) {
//    if ((bool)lhs.bits_[i] != (bool)rhs.bits_[i]) {
//      return false;
//    }
//  }
//  return true;
//}

bool operator != (const Set& lhs, const Set& rhs) {
  return !(lhs == rhs);
}

Partition Partition::MakeFine(std::size_t n) {
  Partition p;
  p.n_ = n;
  for (std::size_t cell = 0; cell < n; ++cell) {
    p.cells_[cell] = std::vector<std::size_t>(1, cell);
  }
  return p;
}

bool Partition::HasCell(std::size_t cell) const {
  return (cells_.count(cell) == 1);
}

std::vector<std::size_t> Partition::GetCellIndices() const {
  std::vector<std::size_t> indices;
  for (std::size_t cell = 0; cell < n_; ++cell) {
    if (cells_.count(cell) == 1) {
      indices.push_back(cell);
    }
  }
  return indices;
}

std::size_t Partition::Cardinality() const {
  std::size_t card = 0;
  for (const auto& kv: cells_) {
    card += kv.second.size();
  }
  return card;
}

Set Partition::GetCellAsSet(std::size_t cell) const {
  if (HasCell(cell)) {
    auto indices = cells_.at(cell);
    return Set::FromIndices(n_, indices);
  }
  else {
    return Set::MakeEmpty(n_);
  }
}

void Partition::RemoveCell(std::size_t cell) {
  if (HasCell(cell)) {
    cells_.erase(cells_.find(cell));
  }
}

std::size_t Partition::MergeCells(std::size_t cell_1, std::size_t cell_2) {
  auto c1 = HasCell(cell_1) ? cell_1 : n_;
  auto c2 = HasCell(cell_2) ? cell_2 : n_;
  auto cell_min = std::min(c1, c2);

  if (c1 < n_ && c2 < n_ && cell_1 != cell_2) {
    auto cell_max = std::max(cell_1, cell_2);
    auto indices_max = cells_[cell_max];
    cells_[cell_min].reserve(cells_[cell_min].size() + indices_max.size());
    for (const auto& i: indices_max) {
      cells_[cell_min].push_back(i);
    }
    std::sort(cells_[cell_min].begin(), cells_[cell_min].end());
    cells_.erase(cells_.find(cell_max));
  }

  return cell_min;
}

std::size_t Partition::MergeCells(const std::vector<std::size_t>& cells) {
  std::unordered_set<std::size_t> cells_to_merge;
  std::size_t cell_new = n_;
  for (const auto& cell: cells) {
    if (cell < cell_new) {
      cell_new = cell;
    }
    if (HasCell(cell)) {
      cells_to_merge.insert(cell);
    }
  }
  for (const auto& cell: cells_to_merge) {
    if (cell != cell_new) {
      for (const auto& i: cells_[cell]) {
        cells_[cell_new].push_back(i);
      }
      cells_.erase(cells_.find(cell));
    }
  }
  std::sort(cells_[cell_new].begin(), cells_[cell_new].end());
  return cell_new;
}

Set Partition::Expand() const {
  Set expanded = Set::MakeEmpty(n_);
  for (const auto& kv: cells_){
    auto indices = kv.second;
    for (const auto& i: indices) {
      expanded.bits_[i] = 1;
    }
  }
  return expanded;
}

Set Partition::Expand(const Set& X) const {
  Set expanded = Set::MakeEmpty(n_);
  for (const auto& kv: cells_) {
    auto cell = kv.first;
    if (cell < X.n_ && X[cell]) {
      auto indices = kv.second;
      for (const auto& i: indices) {
        expanded.bits_[i] = 1;
      }
    }
  }
  return expanded;
}

}
