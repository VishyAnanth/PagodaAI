#include "Swiften/Tensor.h"
#include <iostream>

using namespace Pagoda::Swiften::Core;
using namespace Pagoda::Swiften::Util;

int main() {

    Tensor<uint8_t, uint8_t, uint64_t> new_ten;
    Tensor_Three<uint8_t, uint64_t> x(smallest_cast<3>, smallest_cast<4>, smallest_cast<2>);
    Tensor_Three<uint8_t, uint64_t> y(x);

    char s = 'a';

    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            for(int k = 0; k < 2; k++)
            {
                y(i, j, k) = s++;
            }
        }
    }

    std::cout << y;
    
    Tensor_N<4, uint8_t, uint8_t, uint64_t> four_ten(smallest_cast<3>, smallest_cast<4>, smallest_cast<2>, smallest_cast<5>);

    uint64_t xx = 0;

    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            for(int k = 0; k < 2; k++)
            {
                for(int l = 0; l < 5; l++)
                {
                    four_ten(i, j, k, l) = xx++;
                }
            }
        }
    }

    std::cout << four_ten;

    x = y;
    new_ten = x;

    // for(int i = 0; i < 3; i++)
    // {
    //     for(int j = 0; j < 4; j++)
    //     {
    //         for(int k = 0; k < 2; k++)
    //         {
    //             std::cout << (char)new_ten(i, j, k) << std::endl;
    //         }
    //     }
    // }
    // std::cout << std::endl;

    // uint8_t dim = 24;
    
    // Dimension<uint8_t, uint8_t> new_dims(1, &dim);
    // new_ten.reshape(new_dims);

    // for(int i = 0; i < 24; i++)
    // {
    //     std::cout << (char)new_ten(i) << std::endl;
    // }
    // std::cout << std::endl;

    uint8_t COL = upper_bound<uint8_t>;

    std::cout << new_ten;
    std::cout << new_ten.slice((uint8_t)0, (uint8_t)3, COL);

    Dimension<uint8_t, uint8_t> new_dims2(static_cast<uint8_t>(12), static_cast<uint8_t>(2));
    new_ten.reshape(new_dims2);

    std::cout << new_ten;

    std::cout << new_ten.slice((uint8_t)2, (uint8_t)1);

    
}