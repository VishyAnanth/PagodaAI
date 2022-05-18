#include <iostream>
#include <typeinfo>
#include <vector>

#include "GeneralDefinitions.h"
#include "ModuleDefinitions.h"
#include "CompressionDefinitions.h"

using namespace torch::nn;

int main() {
    // torch::Tensor ten = torch::rand({3, 2}, torch::TensorOptions(torch::kCPU).dtype(at::kFloat)); 
    // std::vector<float> v(ten.data_ptr<float>(), ten.data_ptr<float>() + ten.numel());

    // for (auto a : v) std::cout << a << std::endl;
    // torch::Tensor r1 = torch::randn({232, 232, 232});
    // std::cout << "r1" << std::endl;
    // torch::Tensor r2 = torch::randn({232, 232, 232});
    // std::cout << "r2" << std::endl;
    // Transformer trans(TransformerOptions(232, 8));
    // std::cout << "transformer" << std::endl;
    // torch::Tensor output = trans(r1, r2);
    // std::cout << "out" << std::endl;

    auto net = std::make_shared<Pagoda::GeneralDefinitions::GeneralNet>();
    
    auto f = std::make_shared<Pagoda::ModuleDefinitions::FullyConnectedModule>(10,11,12, 1, 2);

    net->insertModule(f);

    // f->replaceModule(2,2);
    for(auto& p : f->parameters()) {
        std::cout << p.sizes().size() << std::endl;
        
        std::cout << p << std::endl;
    }

    // f->replaceModule(1,1,1);

    // for(auto& p : f->parameters()) {
    //     std::cout << p << std::endl;
    // }
    
    // net->forward();
}
