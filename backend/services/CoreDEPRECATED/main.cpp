#include <iostream>
#include <typeinfo>
#include <vector>
#include <chrono>
#include <cmath>
#include <numeric>

#include "App.h"

// using namespace torch::nn;

// template<typename Iter_T>
// long double vectorNorm(Iter_T first, Iter_T last) {
//     return sqrt(std::inner_product(first, last, first, 0.0L));
// }

int main() {
    Pagoda::App app;
    app.setup();
    app.run();

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

    // auto net = std::make_shared<Pagoda::GeneralDefinitions::GeneralNet>();
    // auto f = std::make_shared<Pagoda::ModuleDefinitions::FullyConnectedModule>(10,11,12,1,1);
    // torch::Tensor ten1 = torch::randn({3,4,5});
    // std::cout << ten1 << std::endl;

    // float* begin = ten1.data_ptr<float>();
    // float* end = ten1.data_ptr<float>() + ten1.numel();
    // uint32_t temp;

    // std::vector<uint8_t> vec;
    
    // while(begin != end) {
    //     temp = *(uint32_t*)&(*begin);
    //     vec.emplace_back(temp);
    //     vec.emplace_back(temp >> 8);
    //     vec.emplace_back(temp >> 16);
    //     vec.emplace_back(temp >> 24);
    //     begin++;
    // }
    // std::vector<float> x;
    // int n = vec.size();

    // for(int i = 0; i < n; i += 4) {
    //     temp = vec[i+3];
    //     temp <<= 8;
    //     temp |= vec[i+2];
    //     temp <<= 8;
    //     temp |= vec[i+1];
    //     temp <<= 8;
    //     temp |= vec[i];
    //     x.emplace_back(*(float*)&temp);
    // }
    

    // torch::Tensor t = torch::from_blob(x.data(), {3,4,5});
    // std::cout << t << std::endl;

    // std::vector<uint64_t> sizeVec;
    // torch::Tensor tempTensor, newCore, U, S, V;
    // std::vector<torch::Tensor> cores;
    // std::vector<torch::Tensor> tempCores;
    // std::vector<std::vector<torch::Tensor> > coresOfCores;
    // double frobNorm = 0, truncationParam = 0, el = 0;
    // int64_t rk = 0, rkminus1 = 0;
    // uint64_t d, counter = 0;
    // int64_t tempReshape, nk;
    // float* ptr;
    // float* endPtr;

    // sizeVec.clear();
    // sizeVec.insert(sizeVec.begin(), ten1.sizes().begin(), ten1.sizes().end());
    // d = sizeVec.size();

    // ptr = ten1.data_ptr<float>();
    // endPtr =  ptr + ten1.numel();
    // while(ptr != endPtr) {
    //     el = *ptr++;
    //     frobNorm += el*el;
    // }
    // truncationParam = 0.001*frobNorm / (d-1);
    // truncationParam = (truncationParam == 0)?1:truncationParam;

    // tempTensor = ten1;
    // rkminus1 = 1;
    // for(uint64_t k = 1; k < d; k++) {
    //     nk = sizeVec[k - 1];
    //     tempReshape = (int64_t)rkminus1 * nk;
    //     tempTensor = tempTensor.reshape({tempReshape, tempTensor.numel() / tempReshape});
    //     auto svd = torch::linalg::svd(tempTensor, false);
    //     U = std::get<0>(svd), S = std::get<1>(svd), V = std::get<2>(svd);

    //     ptr = S.data_ptr<float>();
    //     endPtr = ptr + S.numel();
    //     counter = 0;
    //     frobNorm = 0;
    //     while(endPtr != ptr && frobNorm < truncationParam) {
    //         el = *--endPtr;
    //         frobNorm += el*el;
    //         counter++;
    //     }
    //     rk = (endPtr == ptr)?1:std::min(S.sizes()[0], (int64_t)(S.numel() - counter + 1));

    //     U = U.slice(1, 0, rk);
    //     S = S.slice(0, 0, rk);
    //     V = V.slice(0, 0, rk);
        
    //     S = S.diag();
    //     newCore = U.reshape({(int64_t)rkminus1, nk, (int64_t)rk});
    //     tempTensor = S.matmul(V);
    //     cores.emplace_back(newCore);
    //     rkminus1 = rk;
    // }
    // cores.emplace_back(tempTensor);


    // auto start = std::chrono::high_resolution_clock::now();
    // double res1 = 0;
    // double el;
    // auto it = ten1.data_ptr<float>();
    // auto firstPtr = it;
    // auto endPtr =  ten1.data_ptr<float>() + ten1.numel();
    // while(firstPtr != endPtr) {
    //     el = *firstPtr++;
    //     res1 += el*el;
    // }
    // std::cout << sqrt(res1) << std::endl;
    // auto end = std::chrono::high_resolution_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    // std::cout << duration.count() << std::endl; 

    // net->insertModule(f);
    // // f->replaceModule(2,2);
    // for(auto& p : f->parameters()) {
    //     std::vector<uint64_t> x(p.sizes().begin(), p.sizes().end());
    //     for(auto itr = p.sizes().begin(); itr != p.sizes().end(); itr++) {
    //         std::cout << *itr << std::endl;
    //     }
    //     std::cout << p << std::endl;
    //     std::cout << "=========================" << std::endl;
    // }

    // f->replaceModule(1,1,1);

    // for(auto& p : f->parameters()) {
    //     std::cout << p << std::endl;
    // }
    
    // net->forward();
}
