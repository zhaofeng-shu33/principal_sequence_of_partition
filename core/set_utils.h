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

#ifndef SET_UTILS_H
#define SET_UTILS_H

#include <vector>
#include <utility>
#include <string>
#include <stdexcept>
#include <limits>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <iostream>
//#include <initializer_list>

namespace submodular {

class Set;
class Partition;
bool operator == (const Set& lhs, const Set& rhs);
bool operator != (const Set& lhs, const Set& rhs);
std::ostream& operator << (std::ostream& stream, const Set& X);

using element_type = std::size_t;

class Set {
public:
  // Default constructor
  Set(): n_(0), bits_() {}

  Set(const Set&) = default;
  Set(Set&&) = default;

  // Constructor (a): Construct a Set object just from its size.
  // All elements of bits_ are initialized by 0.
  explicit Set(std::size_t n): n_(n), bits_(n, 0) {}

  // Constructor (b): Construct a Set object from its size and a set of indices
  // in which bits_[i] == 1. If vec contains an index that is larger than n - 1,
  // it throws std::out_of_range.
  //explicit Set(std::size_t n, std::vector<std::size_t> vec);
  //explicit Set(std::size_t n, std::initializer_list<std::size_t> vec);

  // Constructor (c): Construct a Set object from a string object consists of
  // 0-1 characters. If any characters examined in str are not 0 or 1, it throws
  // std::invalid_argument.
  template<class CharT>
  explicit Set(const std::basic_string<CharT>& str);

  // Constructor (d): Construct a Set from a bitmask.
  // The first M bits are initialized by the corresponding bit values of val,
  // where M is the number of radix digits of unsigned long type (typically 32 or 64).
  // If n > M, the remainders are filled by 0.
  explicit Set(std::size_t n, unsigned long val);

  Set& operator=(const Set&) = default;
  Set& operator=(Set&&) = default;

  // static factory methods
  static Set MakeDense(std::size_t n);
  static Set MakeEmpty(std::size_t n);
  static Set FromIndices(std::size_t n, const std::vector<element_type>& indices);
  //! static method to generate the finest partition {{0}, {1}, ..., {n-1}}
  static std::vector<Set> MakeFine(std::size_t n);
  bool HasElement(element_type i) const;
  bool IsSubSet(const Set& s);
  bool operator[] (element_type i) const { return HasElement(i); }

  std::vector<element_type> GetMembers() const;
  std::vector<std::size_t> GetInverseMap() const;
  std::size_t Cardinality() const;
  void AddElement(element_type i);
  void RemoveElement(element_type i);

  // Returns a new copy
  Set Copy() const;

  // Returns a new copy of the complement
  Set Complement() const;
  Set C() const { return Complement(); }
  //Set operator~() const;

  Set Union(const Set& X) const;
  Set Intersection(const Set& X) const;
  Set Extend(int value = 0) const {
      Set X_new(*this);
      X_new.n_++;
      X_new.bits_.push_back(value);
      return X_new;
  }
  std::size_t n_;
  std::vector<char> bits_;
  friend std::ostream& operator << (std::ostream&, const Set&);
};
class Partition {
public:
  Partition() = default;
  Partition(const Partition&) = default;
  Partition(Partition&&) = default;
  Partition& operator=(const Partition&) = default;
  Partition& operator=(Partition&&) = default;

  // static method to generate the finest partition {{0}, {1}, ..., {n-1}}
  static Partition MakeFine(std::size_t n);
  // static method to generate the coarsest partition {{0, 1, ..., n-1}}
  //static Partition MakeCoarse(std::size_t n);

  bool HasCell(std::size_t cell) const;
  std::vector<std::size_t> GetCellIndices() const;
  std::size_t Cardinality() const;

  // Return a Set object placed at the given cell.
  // If cell is not found, it returns an empty
  Set GetCellAsSet(std::size_t cell) const;

  void RemoveCell(std::size_t cell);

  // Merge cells
  // If merge operation successed, then returns the new cell index.
  // Otherwise, just returns the minimum existing cell index or n_
  std::size_t MergeCells(std::size_t cell_1, std::size_t cell_2);
  std::size_t MergeCells(const std::vector<std::size_t>& cells);

  Set Expand() const;
  Set Expand(const Set& X) const;

  // (possible largest index of elements) + 1. Do not confuse with Cardinality().
  std::size_t n_;

  // pairs of {representable elements, cell members}
  // Cell members are maintained to be sorted as long as change operations are performed
  // only through the member functions.
  std::unordered_map<std::size_t, std::vector<std::size_t>> cells_;
};


template<class CharT>
Set::Set(const std::basic_string<CharT>& str)
  : n_(str.size()), bits_(str.size(), 0)
{
  for (std::size_t i = 0; i < n_; ++i) {
    if (str[i] == CharT('0')) {
      bits_[i] = 0;
    }
    else if (str[i] == CharT('1')) {
      bits_[i] = 1;
    }
    else {
      throw std::invalid_argument("Set::constructor_c");
    }
  }
}

std::ostream& operator<<(std::ostream& os, std::vector<submodular::Set> partition);
}//namespace submodular
#endif
