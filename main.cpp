/**
* \author : zhaofeng-shu33
* \brief : accomplish real task by info-clustering technique
*/
#include "config.h"
#include "utility/gaussian2Dcase.h"
//! To be finished
int main(){
    demo::Gaussian2DGraph<double>* g2g = new demo::Gaussian2DGraph<double>(12);
    g2g->run();
    std::vector<submodular::Set> p = g2g->get_smallest_partition(4);
    std::cout << p;
    delete g2g;
    return 0;
}