#pragma once

#include "dimension.h"

namespace Pagoda {
namespace Swiften {
namespace Core {

    template<typename Num_Dims_Type, typename Dims_Type, typename Data_Type>
    class Tensor
    {
    protected:
        Dimension<Num_Dims_Type, Dims_Type> m_dimension;
        Data_Type *m_data;
        Dims_Type m_size;
    public:
        Dims_Type COL = upper_bound<Dims_Type>;

        using Index = Dimension<Num_Dims_Type, Dims_Type>;

        SWIFTEN_I Tensor() : m_dimension{Dimension<Num_Dims_Type, Dims_Type>()}, m_data{new Data_Type[1]}, m_size{1}
        {
            this->m_data[0] = 0;
        } 

        SWIFTEN_I Tensor(Dimension<Num_Dims_Type, Dims_Type> p_dimension) 
        {
            this->m_dimension = p_dimension;
            this->m_size = 1;
            Dims_Type temp = this->m_dimension.n();
            while(this->m_size < temp)
                this->m_size <<= 1;
            this->m_data = new Data_Type[this->m_size];
        }

        SWIFTEN_I Tensor(Dimension<Num_Dims_Type, Dims_Type> p_dimension, Data_Type *p_data) 
        {
            this->m_dimension = p_dimension;
            this->m_size = 1;
            Dims_Type temp = this->m_dimension.n();
            while(this->m_size < temp)
                this->m_size <<= 1;
            this->m_data = new Data_Type[this->m_size];
            f(temp) this->m_data[i] = p_data[i];
        }

        SWIFTEN_I Tensor(Tensor<Num_Dims_Type, Dims_Type, Data_Type>& other) 
        {
            this->m_dimension = other.m_dimension;
            this->m_size = 1;
            Dims_Type temp = this->m_dimension.n();
            while(this->m_size < temp)
                this->m_size <<= 1;
            this->m_data = new Data_Type[this->m_size];
            f(temp) this->m_data[i] = other.m_data[i];
        }

        template<typename... n, typename std::enable_if<all_same<Dims_Type, n...>::value>::type* = nullptr>
        SWIFTEN_I Tensor(const Dims_Type& first_dim, const n&... p_dims)
        {
            Dims_Type dimensions[sizeof...(n) + 1];
            dimensions[0] = first_dim;
            Num_Dims_Type j = 1;
            for(const auto& elem : {p_dims...})
            {
                dimensions[j++] = elem;
            }
            this->m_dimension = Dimension<Num_Dims_Type, Dims_Type>(sizeof...(n) + 1, dimensions);
            this->m_size = 1;
            Dims_Type temp = this->m_dimension.n();
            while(this->m_size < temp)
                this->m_size <<= 1;
            this->m_data = new Data_Type[this->m_size];
        }

        SWIFTEN_I Tensor& 
        operator=(const Tensor<Num_Dims_Type, Dims_Type, Data_Type>& other) 
        {
            this->m_dimension = other.m_dimension;
            Dims_Type temp = this->m_dimension.n();
            if(this->m_size < temp && this->m_data != nullptr)
            {
                delete[] this->m_data;

                while(this->m_size < temp)
                    this->m_size <<= 1;
                this->m_data = new Data_Type[this->m_size];
            }
            f(temp) this->m_data[i] = other.m_data[i];
            return *this;
        }
        
        SWIFTEN_I Data_Type&
        operator()(const Index& index) 
        {
            if(!(this->m_dimension >= index))
                throw std::invalid_argument(SWIFTEN_TENSOR_IND_OUT_OF_RANGE);
            Dims_Type tempOffset = 0;
            f(index.d()) 
            {
                tempOffset *= this->m_dimension[i];
                tempOffset += index[i];
            }
            return this->m_data[tempOffset];
        }

        template<typename ...dims_type>
        SWIFTEN_I Data_Type&
        operator()(const dims_type&... p_dims)
        {
            if(sizeof...(p_dims) != this->m_dimension.d())
                throw std::invalid_argument(SWIFTEN_TENSOR_ACCESS_DIMS);
            
            Index index;
            for(const auto& i : {p_dims...})
            {
                index.push(i);
            }
            return this->operator()(index);
        }

        SWIFTEN_I Tensor&
        slice(const Index& index) 
        {
            bool greater_eq = false;
            f(this->m_dimension.d()) if(index[i] != COL && this->m_dimension[i] <= index[i]) greater_eq = true;
            if(greater_eq)
            {
                throw std::invalid_argument(SWIFTEN_TENSOR_IND_OUT_OF_RANGE);
            }
            
            Dimension<Num_Dims_Type, Dims_Type> result_dims;
            f(index.d())
            {
                result_dims.push((index[i] == COL)?this->m_dimension[i]:1);
            }

            Tensor* res = new Tensor(result_dims);
            Tensor& result = *res;
            
            Dimension<Num_Dims_Type, Dims_Type> index_dim;
            Dimension<Num_Dims_Type, Dims_Type> result_index_dim;
            Dims_Type this_d = this->m_dimension.d();
            auto recurse_index = [&](auto&& recurse)
            {
                Dims_Type index_d = index_dim.d();
                if(index_d == this_d)
                {
                    result(result_index_dim) = this->operator()(index_dim);
                    return;
                }
                if(result_dims[index_d] == 1)
                {
                    result_index_dim.push(0);
                    index_dim.push(index[index_d]);
                    recurse(recurse);
                    index_dim.pop();
                    result_index_dim.pop();
                }
                else
                {
                    f(result_dims[index_d])
                    {
                        result_index_dim.push(i);
                        index_dim.push(i);
                        recurse(recurse);
                        index_dim.pop();
                        result_index_dim.pop();
                    }
                }
            };
            recurse_index(recurse_index);
            return result;
        }

        template<typename ...dims_type>
        SWIFTEN_I Tensor&
        slice(const dims_type&... p_dims)
        {
            if(sizeof...(p_dims) != this->m_dimension.d())
                throw std::invalid_argument(SWIFTEN_TENSOR_ACCESS_DIMS);

            Index index;
            for(const auto& i : {p_dims...})
            {
                index.push(i);
            }
            return this->slice(index);
        }

        template<typename num_dims_type, typename dims_type>
        SWIFTEN_I Tensor& 
        reshape(const Dimension<num_dims_type, dims_type>& p_dims)
        {
            if(this->m_dimension.n() != p_dims.n())
                throw std::invalid_argument(SWIFTEN_TENSOR_RESHAPE_DIMS);
            this->m_dimension = p_dims;
            return *this;
        }

        ~Tensor()
        {
            if(this->m_data != nullptr)
                delete[] this->m_data;
        }

        friend std::ostream& operator<<(std::ostream& os, Tensor& ten)
        {
            os << ten.m_dimension;
            os << MAKE_BLUE("TENSOR") << std::endl;
            Dimension<Num_Dims_Type, Dims_Type> index_dim;
            Dims_Type this_d = ten.m_dimension.d();

            auto recurse_index = [&](auto&& recurse)
            {
                Dims_Type index_d = index_dim.d();
                if(index_d == this_d)
                {
                    os << (uint64_t)ten(index_dim) << " ";
                    return;
                }
                f(ten.m_dimension[index_d])
                {
                    index_dim.push(i);
                    recurse(recurse);
                    index_dim.pop();
                }
                os << std::endl;
            };
            
            recurse_index(recurse_index);
            return os;
        }

    }; //class Tensor

} //namespace Core
} //namespace Swiften
} //namespace Pagoda
