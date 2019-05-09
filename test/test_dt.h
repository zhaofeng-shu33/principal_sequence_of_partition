#pragma once
/**
* \author: zhaofeng-shu33
*/
#include "core/dt.h"
#if USE_EIGEN3
#include "test/algorithms/test_sfm_fw.h"
#endif
#include "test/test_extreme.h"
namespace submodular {
    /**
    * $f: 2^{\{1,2\}} \to \mathbb{R}, f(\emptyset) = 0, f(\{1\}) = 12, f(\{2\}) = 8, f(\{1,2\}) = 19$
    */
    template <typename ValueType>
    class SampleFunction : public SubmodularOracle<ValueType> {
    public:
        using value_type = typename ValueTraits<ValueType>::value_type;
        std::string GetName() { return ""; }
        SampleFunction()
        {
            this->SetDomain(Set::MakeDense(2));
        }
        value_type Call(const Set& X) {
            int car = X.Cardinality();
            if (car == 0)
                return 0;
            else if (car == 2)
                return 19;
            else if (X.HasElement(0))
                return 12;
            else
                return 8;            
        }
    };


    TEST(DilworthTruncation, SampleFunction){
        SampleFunction<double> sff;
        DilworthTruncation<double> dt(&sff, 2);
        dt.Run(true);
        // optimal value is 16
        // optimal partition is { 0 }, { 1 },        
        
        EXPECT_EQ(dt.Get_min_value(), 16);
        std::vector<Set> p = dt.Get_min_partition();
        EXPECT_EQ(p.size(), 2);
        EXPECT_EQ(p[0], Set::FromIndices(2, { 0 }));
        EXPECT_EQ(p[1], Set::FromIndices(2, { 1 }));
    }
    TEST(DilworthTruncation, SampleFunctionPartial) {
        std::vector<double> xl(1);
        SampleFunction<double>* sf = new SampleFunction<double>();
        SampleFunctionPartial<double> F1(xl, sf, 3.5);
        F1.Call(Set::MakeDense(1));
        delete sf;
    }
    TEST(DilworthTruncation, HyperGraphicalModel){
        HyperGraphicalModel<double> hgm;
        DilworthTruncation<double> dt(&hgm, 1.5);
        dt.Run(true);
        EXPECT_EQ(dt.Get_min_value(), 0);
        std::vector<Set> p = dt.Get_min_partition();
        EXPECT_EQ(p.size(), 2);
        EXPECT_EQ(p[0], Set::FromIndices(3, { 0, 1 }));
        EXPECT_EQ(p[1], Set::FromIndices(3, { 2 }));
        // optimal value is 0
        // optimal partition is{ 0, 1 }, { 2 },        
    }
    TEST(DilworthTruncation, HyperGraphicalModel2) {
        HyperGraphicalModel2<double> hgm2;
        DilworthTruncation<double> dt(&hgm2, 1.5);
        dt.Run(true);
        EXPECT_EQ(dt.Get_min_value(), -1.5);
        std::vector<Set> p = dt.Get_min_partition();
        EXPECT_EQ(p.size(), 5);
        EXPECT_EQ(p[0], Set::FromIndices(6, { 0, 1 }));
        EXPECT_EQ(p[1], Set::FromIndices(6, { 2 }));
        EXPECT_EQ(p[2], Set::FromIndices(6, { 3 }));
        EXPECT_EQ(p[3], Set::FromIndices(6, { 4 }));
        EXPECT_EQ(p[4], Set::FromIndices(6, { 5 }));
        // optimal value is -1.5
        // optimal partition is{ 0, 1 }, { 2 }, { 3 }, { 4 }, { 5 }  
    }
    TEST(PSP, HyperGraphModelDT) {
        HyperGraphicalModel2<double>* hgm = new HyperGraphicalModel2<double>();
        DilworthTruncation<double> dt(hgm, 0.0);
        dt.Run(true); //BruteForce
        double min_value = dt.Get_min_value();
        std::vector<Set> min_p = dt.Get_min_partition();
        EXPECT_DOUBLE_EQ(min_value, 4.0);
        delete hgm;
    }
    TEST(PSP, HyperGraphModelSFM) {
        HyperGraphicalModel2<double>* hgm = new HyperGraphicalModel2<double>();
        BruteForce<double> solver2;
        std::vector<double> xl({2,0,0});
        SampleFunctionPartial<double> F1(xl, hgm, 0.0);
        solver2.Minimize(F1);
        double alpha_2 = solver2.GetMinimumValue();
        Set p_2 = solver2.GetMinimizer();
        EXPECT_DOUBLE_EQ(alpha_2, 1.0);
        EXPECT_DOUBLE_EQ(F1.Call(p_2), alpha_2);
#if USE_EIGEN3        
	FWRobust<double> solver1;
        solver1.Minimize(F1);
        double alpha_l = solver1.GetMinimumValue();
        EXPECT_DOUBLE_EQ(alpha_l, 1.0);
#endif
	delete hgm;
    }

}
