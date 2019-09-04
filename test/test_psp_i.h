#pragma once
#include "psp/psp_i.h"
#include "utility.h"
namespace demo {


    TEST(PSP_I, SIMPLE_FOUR_POINT) {
        std::vector<std::tuple<std::size_t, std::size_t, double>> elt;
        elt.push_back(std::make_tuple(0, 1, 1));
        elt.push_back(std::make_tuple(0, 2, 0.4));
        elt.push_back(std::make_tuple(1, 3, 0.5));
        elt.push_back(std::make_tuple(2, 3, 2));
        psp::PSP ic(elt, 4);
        ic.run_psp_i();
        std::list<double> cv = ic.get_critical_values();
        EXPECT_EQ(cv.size(), 3);
        std::list<double>::iterator it = cv.begin();
        EXPECT_DOUBLE_EQ(*it, 0.9);
        it++;
        EXPECT_DOUBLE_EQ(*it, 1);
        it++;
        EXPECT_DOUBLE_EQ(*it, 2);

        std::list<stl::Partition> psp_list = ic.get_psp();
        EXPECT_EQ(psp_list.size(), 4);
        std::list<stl::Partition>::iterator it_2 = psp_list.begin();
        EXPECT_EQ(*it_2, stl::Partition::makeDense(4));
        stl::Partition p2;
        p2.AddElement(stl::CSet(std::string("11")));
        p2.AddElement(stl::CSet(std::string("0011")));
        it_2++;
        EXPECT_EQ(*it_2, p2);
        stl::Partition p3;
        p3.AddElement(stl::CSet(std::string("1")));
        p3.AddElement(stl::CSet(std::string("01")));
        p3.AddElement(stl::CSet(std::string("0011")));
        it_2++;
        EXPECT_EQ(*it_2, p3);
        it_2++;
        EXPECT_EQ(*it_2, stl::Partition::makeFine(4));
    }
    TEST(PSP_I, GivenPoint8) {
        double a[8][2] = { {3.1, 3.2},
                           {4.0, 4.0 },
                           {1.1, -2.2},
                           {3.9, -2.0},
                           {-3.9, -2.0},
                           {-2.2, -3.5},
                           {-3.9, 2.4},
                           {-3.1, 2.6}
        };
        Gaussian2DGraph g2g(8, 1.0, a);
        g2g.run();
        std::list<double> gamma_list_2 = g2g.get_gamma_list();
        std::list<stl::Partition> psp_list_2 = g2g.get_psp();

        demo::EdgeListTuple et = g2g.get_edge_list_tuple();
        lemon::ListDigraph g;
        lemon::ListDigraph::ArcMap<double> edge_map(g);
        submodular::make_dgraph(8, et, g, edge_map);

        psp::PSP_I instance(&g, &edge_map);
        instance.run();
        std::list<double> gamma_list = instance.get_critical_values();
        std::list<stl::Partition> psp_list = instance.get_psp();

        std::list<double>::iterator it_2 = gamma_list_2.begin();

        EXPECT_EQ(psp_list, psp_list_2);

        for (std::list<double>::iterator it = gamma_list.begin(); it != gamma_list.end(); it++) {
            EXPECT_DOUBLE_EQ(*it, *it_2);
            it_2++;
        }

    }
}