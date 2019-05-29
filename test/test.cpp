#include "config.h"
#include "gtest/gtest.h"

// Graph tests
#include "graph/test_graph.h"
// Gaussian test
#include "graph/test_gaussian.h"

// Dilworth truncation tests
#include "test_dt.h"

#include "test_pmf.h"

// maxflow test in dt case
#if USE_LEMON
#include "algorithms/test_sfm_mf.h"
#endif