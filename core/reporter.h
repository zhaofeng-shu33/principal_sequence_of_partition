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

#ifndef REPORTER_H
#define REPORTER_H

#include <string>
#include <unordered_map>
#include <chrono>
#include <iostream>
#include <utility>
#include "core/set_utils.h"

namespace submodular {

struct SimpleTimer {
  static auto Now() { return std::chrono::system_clock::now(); }
};

enum class ReportKind {
  ORACLE,
  BASE,
  TOTAL,
  ITERATION,
  PREPROCESSING
};

class SFMReporter;

std::ostream& operator << (std::ostream& stream, const SFMReporter& reporter);

class SFMReporter {
public:
  SFMReporter() = default;
  SFMReporter(const SFMReporter&) = default;
  SFMReporter(SFMReporter&&) = default;
  SFMReporter& operator=(const SFMReporter&) = default;
  SFMReporter& operator=(SFMReporter&&) = default;

  using duration_type = typename std::chrono::milliseconds;
  std::string duration_suffix_ = "msec";

  std::unordered_map<ReportKind, unsigned int> counts_;
  std::unordered_map<ReportKind, duration_type> times_;
  std::vector<std::string> msg_;

  std::string algorithm_name_;
  std::string oracle_name_;
  double minimum_value_;
  Set minimizer_;

  void Clear();

  void EntryCounter(ReportKind kind);
  void EntryTimer(ReportKind kind);
  void IncreaseCount(ReportKind kind, unsigned int num = 1);
  void TimerStart(ReportKind kind);
  void TimerStop(ReportKind kind);

  void SetNames(std::string alg_name, std::string oracle_name);
  void SetResults(double minimum_value, const Set& minimizer);
  void SetResults(double minimum_value, Set&& minimizer);
  void SetMessage(std::string message) { msg_.push_back(message); }

  friend std::ostream& operator << (std::ostream&, const SFMReporter&);
private:
  std::unordered_map<ReportKind, bool> start_flags_;
  std::unordered_map<ReportKind, decltype(SimpleTimer::Now())> start_times_;
};



}

#endif
