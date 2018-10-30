/**
* \author: zhaofeng-shu33
*/
#include "core/dt.h"
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
    /**
    * $f: 2^{\{1,2,3\}} \to \mathbb{R}, f(\emptyset) = 0, f(\{3\}) = 1, \textrm{others}: f(\{V\}) = 2$ 
    */
    template <typename ValueType>
    class HyperGraphicalModel : public SubmodularOracle<ValueType> {
    public:
        std::string GetName() { return ""; }
        HyperGraphicalModel()
        {
            this->SetDomain(Set::MakeDense(3));
        }
        value_type Call(const Set& X) {
            int car = X.Cardinality();
            if (car == 0) // empty set
                return 0;
            else if (car == 1 && X.n_ == 3 && X.bits_[2]==1)
                return 1;
            else
                return 2;
        }
    };
    TEST(DilworthTruncation, SampleFunction){
        DilworthTruncation<float> dt(&SampleFunction<float>(), 2);
        dt.Run();
        // optimal value is 16
        // optimal partition is { 0 }, { 1 },        
        
        EXPECT_EQ(dt.Get_min_value(), 16);
        std::vector<Set> p = dt.Get_min_partition();
        EXPECT_EQ(p.size(), 2);
        EXPECT_EQ(p[0], Set::FromIndices(2, { 0 }));
        EXPECT_EQ(p[1], Set::FromIndices(2, { 1 }));
    }
    TEST(DilworthTruncation, HyperGraphicalModel){
        DilworthTruncation<float> dt(&HyperGraphicalModel<float>(), 1.5);
        dt.Run();
        EXPECT_EQ(dt.Get_min_value(), 0);
        std::vector<Set> p = dt.Get_min_partition();
        EXPECT_EQ(p.size(), 2);
        EXPECT_EQ(p[0], Set::FromIndices(3, { 0, 1 }));
        EXPECT_EQ(p[1], Set::FromIndices(3, { 2 }));
        // optimal value is 0
        // optimal partition is{ 0, 1 }, { 2 },        
    }
    TEST(PSP, HyperGraphicalModel){
        HyperGraphicalModel<float>* hgm = new HyperGraphicalModel<float>();
        PSP<float> psp_class(hgm);
        psp_class.run();
        std::vector<float> gamma_list = psp_class.Get_critical_values();
        std::vector<std::vector<Set>> psp_list = psp_class.Get_psp();
        EXPECT_EQ(gamma_list.size(), 3);
        EXPECT_EQ(gamma_list[0], 1);
        EXPECT_EQ(gamma_list[1], 2);

        EXPECT_EQ(psp_list.size(), 3);

        EXPECT_EQ(psp_list[0].size(), 1);
        EXPECT_EQ(psp_list[0][0], Set::MakeDense(3));

        EXPECT_EQ(psp_list[1].size(), 2);
        EXPECT_EQ(psp_list[1][0], Set::FromIndices(3, { 0, 1 }));
        EXPECT_EQ(psp_list[1][1], Set::FromIndices(3, { 2 }));

        EXPECT_EQ(psp_list[2].size(), 3);
        EXPECT_EQ(psp_list[2][0], Set::FromIndices(3, { 0 }));
        EXPECT_EQ(psp_list[2][1], Set::FromIndices(3, { 1 }));
        EXPECT_EQ(psp_list[2][2], Set::FromIndices(3, { 2 }));

        delete hgm;
        // partition list is {{{0, 1, 2}}, {{0, 1}, {2}}, {{0}, {1}, {2}}}  
        // gamma list is {1, 2}
    }
}
