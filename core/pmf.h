#pragma once
#include <tuple>
#include <numeric>
#include <lemon/list_graph.h>
#include "core/parametric_preflow.h"
#include <iostream>
typedef std::pair<double, double> pair;
double compute_lambda(const std::vector<pair>& parameter_list, const double target_value);