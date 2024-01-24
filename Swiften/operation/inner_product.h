#pragma once

#include "../core/tensor.h"

namespace Pagoda {
namespace Swiften {
namespace Operation {

using namespace Core;

template<typename Dims_Type, typename Data_Type>
SWIFTEN_I Data_Type
inner_product(const Tensor_One<Dims_Type, Data_Type>& vec_one, const Tensor_One<Dims_Type, Data_Type>& vec_two)
{
    auto& vec_one_dim = vec_one.get_dimension();
    auto& vec_two_dim = vec_two.get_dimension();
    if(vec_one_dim != vec_two_dim)
        throw std::invalid_argument(SWIFTEN_DIMENSION_COMPARISON_DIMS);
    Data_Type res = 0;
    Dims_Type n = vec_one_dim[0];
    
    f(n) res += (vec_one(i) * vec_two(i));
    return res;
}

template<typename Num_Dims_Type, typename Dims_Type, typename Data_Type>
SWIFTEN_I Data_Type
inner_product(const Tensor<Num_Dims_Type, Dims_Type, Data_Type>& ten_one, const Tensor<Num_Dims_Type, Dims_Type, Data_Type>& ten_two)
{
    auto& ten_one_dim = ten_one.get_dimension();
    auto& ten_two_dim = ten_two.get_dimension();
    if(ten_one_dim.d() != 1 || ten_one_dim != ten_two_dim)
        throw std::invalid_argument(SWIFTEN_DIMENSION_COMPARISON_DIMS);
    
    Data_Type res = 0;
    Dims_Type n = ten_one_dim[0];
    
    f(n) res += (ten_one(i) * ten_two(i));
    return res;
}

template<typename Dims_Type, typename Data_Type>
SWIFTEN_I Data_Type
frobenius_inner_product(const Tensor_Two<Dims_Type, Data_Type>& mat_one, const Tensor_Two<Dims_Type, Data_Type>& mat_two)
{
    auto& mat_one_dim = mat_one.get_dimension();
    auto& mat_two_dim = mat_two.get_dimension();

    if(mat_one_dim[0] == mat_two_dim[0] || mat_one_dim[1] == mat_two_dim[1])
        throw std::invalid_argument(SWIFTEN_DIMENSION_COMPARISON_DIMS);

    Data_Type res = 0;
    Dims_Type m = mat_one_dim[0];
    Dims_Type n = mat_one_dim[1];

    //NEED TO FUCKING PARALLELIZE THIS SHIT 
    Dims_Type i = 0, j = 0;
    fi(m, i)
    fi(n, j)
    res += (mat_one(i, j)*mat_two(i, j));
    
    return res;
}

template<typename Num_Dims_Type, typename Dims_Type, typename Data_Type>
SWIFTEN_I Data_Type
frobenius_inner_product(const Tensor<Num_Dims_Type, Dims_Type, Data_Type>& ten_one, const Tensor<Num_Dims_Type, Dims_Type, Data_Type>& ten_two)
{
    auto& ten_one_dim = ten_one.get_dimension();
    auto& ten_two_dim = ten_two.get_dimension();

    if(ten_one_dim.d() != 2 || ten_two_dim.d() != 2 || ten_one_dim[0] == ten_two_dim[0] || ten_one_dim[1] == ten_two_dim[1])
        throw std::invalid_argument(SWIFTEN_DIMENSION_COMPARISON_DIMS);

    Data_Type res = 0;
    Dims_Type m = ten_one_dim[0];
    Dims_Type n = ten_one_dim[1];

    Dims_Type i = 0, j = 0;
    fi(m, i)
    fi(n, j)
    res += (mat_one(i, j)*mat_two(i, j));
}

}
}
}