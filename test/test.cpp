#include "config.h"
#include "gtest/gtest.h"

// Utility tests
#include "test_set_utils.h"
#if USE_EIGEN3
#include "test_base.h"
#include "test_linalg.h"
#endif
// Oracle tests
#include "test_oracle.h"
#include "oracles/test_modular.h"
#include "oracles/test_iwata_test_function.h"

// Algorithm tests
#include "algorithms/test_brute_force.h"
//#include "algorithms/test_sfm_iff.h"
#if USE_EIGEN3
// can be only used when the domain of submodular function is integer!
#include "algorithms/test_sfm_fw.h"
#endif
// Graph tests
#include "graph/test_maxflow.h"
#include "graph/test_divide_conquer.h"
#include "graph/test_graph_oracle.h"

// Dilworth truncation tests
#include "test_dt.h"

// Gaussian test
#include "test_gaussian.h"