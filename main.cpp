/**
* \author : zhaofeng-shu33
* \brief : accomplish real task by info-clustering technique
*/
#include "config.h"
#include "utility/gaussian2Dcase.h"
//! To be finished
int main(){
    demo::Gaussian2DGraph<double>* g2g = new demo::Gaussian2DGraph<double>(8);
    g2g->run();
    std::vector<double> gl = g2g->get_gamma_list();
    std::vector<std::vector<submodular::Set>> psp_l = g2g->get_psp_list();
    for (int i = 0; i < gl.size(); i++) {
        if (psp_l[i].size() == 0)
            continue;
        std::cout << "critical value: " << gl[i] << std::endl; // the last critical value is not set
        std::cout << psp_l[i] << std::endl;
    }
    delete g2g;
    return 0;
}