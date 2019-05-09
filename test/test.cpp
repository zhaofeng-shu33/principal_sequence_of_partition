#include "config.h"
#include "gtest/gtest.h"


// Algorithm tests
#include "algorithms/test_brute_force.h"

// Graph tests
#include "graph/test_graph.h"

// Dilworth truncation tests
#include "test_dt.h"

// Gaussian test
#include "test_gaussian.h"

// maxflow test in dt case
#if USE_LEMON
#include "algorithms/test_sfm_mf.h"
#endif