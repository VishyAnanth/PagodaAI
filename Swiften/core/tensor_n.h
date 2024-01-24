#pragma once

#include "tensor_base.h"

#include <cstdarg>

namespace Pagoda {
namespace Swiften {
namespace Core {

    template<uint64_t NUM_DIMS, typename Num_Dims_Type, typename Dims_Type, typename Data_Type>
    class Tensor_N : public Tensor<Num_Dims_Type, Dims_Type, Data_Type>
    {
    public:
        SWIFTEN_I Tensor_N() = delete;

        SWIFTEN_I Tensor_N(Dimension<Num_Dims_Type, Dims_Type> p_dimension) : Tensor<Num_Dims_Type, Dims_Type, Data_Type>(p_dimension)
        {
            if(p_dimension.d() != NUM_DIMS)
                throw std::invalid_argument(SWIFTEN_TENSOR_INTIALIZATION_DIMS);
        }

        SWIFTEN_I Tensor_N(Dimension<Num_Dims_Type, Dims_Type> p_dimension, Data_Type *p_data) : Tensor<Num_Dims_Type, Dims_Type, Data_Type>(p_dimension, p_data)
        {
            if(p_dimension.d() != NUM_DIMS)
                throw std::invalid_argument(SWIFTEN_TENSOR_INTIALIZATION_DIMS);
        }

        SWIFTEN_I Tensor_N(const Tensor<Num_Dims_Type, Dims_Type, Data_Type>& other) : Tensor<Num_Dims_Type, Dims_Type, Data_Type>(other)
        {
            if(other.m_dimension.d() != NUM_DIMS)
                throw std::invalid_argument(SWIFTEN_TENSOR_INTIALIZATION_DIMS);
        }

        template<typename... n, typename std::enable_if<all_same<Dims_Type, n...>::value>::type* = nullptr>
        SWIFTEN_I Tensor_N(const Dims_Type& first_dim, const n&... p_dims) : Tensor<Num_Dims_Type, Dims_Type, Data_Type>(first_dim, p_dims...)
        {
            if(sizeof...(n) + 1 != NUM_DIMS)    
                throw std::invalid_argument(SWIFTEN_TENSOR_INTIALIZATION_DIMS);
        }

        SWIFTEN_I Tensor_N& 
        operator=(const Tensor_N<NUM_DIMS, Num_Dims_Type, Dims_Type, Data_Type>& other) 
        {
            Tensor<Num_Dims_Type, Dims_Type, Data_Type>::operator=(other);
            return *this;
        }

        template<typename num_dims_type, typename dims_type>
        SWIFTEN_I Tensor_N& 
        reshape(const Dimension<num_dims_type, dims_type>& p_dims)
        {
            if(p_dims.d() != NUM_DIMS)
                throw std::invalid_argument(SWIFTEN_TENSOR_RESHAPE_DIMS);
            Tensor<Num_Dims_Type, Dims_Type, Data_Type>::reshape(p_dims);
            return *this;
        }
    }; //class Tensor_N

    template<typename Dims_Type, typename Data_Type>
    using Tensor_One = Tensor_N<1, uint8_t, Dims_Type, Data_Type>;

    template<typename Dims_Type, typename Data_Type>
    using Tensor_Two = Tensor_N<2, uint8_t, Dims_Type, Data_Type>;

    template<typename Dims_Type, typename Data_Type>
    using Tensor_Three = Tensor_N<3, uint8_t, Dims_Type, Data_Type>;

}
}
}