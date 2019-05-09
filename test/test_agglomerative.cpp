#include <sstream>

#include <gtest/gtest.h>
#include "config.h"
#include "core/agglomerative.h"
#include "test/test_extreme.h"
#include "core/io_utility.h"
#include "core/dt.h"
#include "core/oracles/graph_cut.h"
#include "core/gaussian2Dcase.h"
namespace submodular{
//compare the two methods
TEST(Gaussian2D, Agglomerative) {
    double a[8][2] = { {3.1, 3.2},
                       {4.0, 4.0 },
                       {1.1, -2.2},
                       {3.9, -2.0},
                       {-3.9, -2.0},
                       {-2.2, -3.5},
                       {-3.9, 2.4},
                       {-3.1, 2.6}
    };
    demo::Gaussian2DGraph g2g(8, 1.0, a);
    g2g.agglomerative_run();
    std::vector<double> gamma_list = g2g.get_gamma_list();
    std::vector<std::vector<submodular::Set>> psp_list = g2g.get_psp_list();
    g2g.run();
    std::vector<double> gamma_list_2 = g2g.get_gamma_list();
    std::vector<std::vector<submodular::Set>> psp_list_2 = g2g.get_psp_list();

    for (int i = 0; i < gamma_list.size(); i++) {
        if (psp_list[i].size() == 0)
            continue;
        EXPECT_DOUBLE_EQ(gamma_list[i], gamma_list_2[i]);
        EXPECT_EQ(psp_list[i], psp_list_2[i]);
    }

}
}