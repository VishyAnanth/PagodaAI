#pragma once

#include "tensor_base.h"

#include <iostream>

namespace Pagoda {
namespace Swiften {
namespace Core {

    template<typename Num_Dims_Type, typename Dims_Type, typename Data_Type, typename Rank_Type>
    class Tensor_Train
    {
    private:
        Rank_Type m_rank;
        Tensor_Two<Dims_Type, Data_Type> *m_head = nullptr;
        Tensor_Three<Dims_Type, Data_Type> *m_tensor_train = nullptr;
        Tensor_Two<Dims_Type, Data_Type> *m_tail = nullptr;
    public:
        SWIFTEN_I Tensor_Train()
        {}

        SWIFTEN_I Tensor_Train(Tensor<Num_Dims_Type, Dims_Type, Data_Type>& raw_tensor)
        {
            Dimension<Num_Dims_Type, Dims_Type>& dims = raw_tensor.get_dimension();

            Dims_Type n_bar = dims.n();
            Num_Dims_Type d = dims.d();
            Rank_Type r_d = 1;
            
            Dimension<Num_Dims_Type, Dims_Type> new_dims(static_cast<Num_Dims_Type>(n_bar / dims[d - 1]), static_cast<Num_Dims_Type>(dims[d - 1]));
            Tensor<Num_Dims_Type, Dims_Type, Data_Type> temp_tensor = raw_tensor.reshape(new_dims);

            for(; d > 1; --d)
            {
                
            }
        }

        SWIFTEN_I Tensor_Train(const Tensor_Train<Num_Dims_Type, Dims_Type, Data_Type, Rank_Type>& other)
        {
            this->m_head = new Tensor_Two<Dims_Type, Data_Type>(other.m_head);
            this->m_tail = new Tensor_Two<Dims_Type, Data_Type>(other.m_tail);
            this->m_rank = other.m_rank;

            //TODO

        }

    }; //class Tensor_Train

} //namespace Core
} //namespace Swiften
} //namespace Pagoda