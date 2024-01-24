#pragma once

#include "../core/tensor.h"

#include "inner_product.h"

namespace Pagoda {
namespace Swiften {
namespace Operation {

using namespace Core;

template<typename Dims_Type, typename Data_Type>
SWIFTEN_I Data_Type
euclidean_norm(const Tensor_One<Dims_Type, Data_Type>& vec)
{
    Data_Type res = inner_product<Dims_Type, Data_Type>(vec, vec);
    return sqrt(res);
}

template<typename Num_Dims_Type, typename Dims_Type, typename Data_Type>
SWIFTEN_I Data_Type
euclidean_norm(const Tensor<Num_Dims_Type, Dims_Type, Data_Type>& ten)
{
    Data_Type res = inner_product<Num_Dims_Type, Dims_Type, Data_Type>(ten, ten);
    return sqrt(res);
}

template<typename Dims_Type, typename Data_Type>
SWIFTEN_I Data_Type
frobenius_norm(const Tensor_Two<Dims_Type, Data_Type>& mat)
{
    Data_Type res = frobenius_inner_product<Dims_Type, Data_Type>(mat, mat);
    return sqrt(res);
}

template<typename Num_Dims_Type, typename Dims_Type, typename Data_Type>
SWIFTEN_I Data_Type
frobenius_norm(const Tensor<Num_Dims_Type, Dims_Type, Data_Type>& ten)
{
    Data_Type res = frobenius_inner_product<Num_Dims_Type, Dims_Type, Data_Type>(ten, ten);
    return sqrt(res);
}

}
}
}