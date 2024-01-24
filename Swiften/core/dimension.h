#pragma once

#include "../config/config.h"
#include "../util/util.h"

#include <stdexcept>
#include <iostream>

namespace Pagoda {
namespace Swiften {
namespace Core {

    using namespace Util;

    template<typename Num_Dims_Type, typename Dims_Type>
    class Dimension
    {
    private:
        Num_Dims_Type m_d;
        Num_Dims_Type m_size;
        Dims_Type *m_dimensions;
        Num_Dims_Type m_n;

        SWIFTEN_I Dims_Type 
        calc_n() const 
        {
            Dims_Type res = 1;
            f(this->m_d) res *= this->m_dimensions[i];
            return res;
        }
    public:
        SWIFTEN_I Dimension() : m_d{0}, m_dimensions{new Dims_Type[1]}, m_size{1}, m_n{1}
        {
            this->m_dimensions[0] = 1;
        }

        SWIFTEN_I Dimension(Num_Dims_Type p_d, const Dims_Type *p_dimensions) 
        {
            this->m_size = 1;
            this->m_d = p_d;
            while(p_d >>= 1)
                this->m_size <<= 1;
            this->m_size <<= 1;
            this->m_dimensions = new Dims_Type[this->m_size];
            f(this->m_d) this->m_dimensions[i] = p_dimensions[i];
            this->m_n = this->calc_n();
        }

        SWIFTEN_I Dimension(const Dimension<Num_Dims_Type, Dims_Type>& other) 
        {
            this->m_size = 1;
            this->m_d = other.m_d;
            Num_Dims_Type temp = other.m_d;
            while(temp >>= 1)
                this->m_size <<= 1;
            this->m_size <<= 1;
            this->m_dimensions = new Dims_Type[this->m_size];
            f(this->m_d) this->m_dimensions[i] = other.m_dimensions[i];
            this->m_n = this->calc_n();
        }

        template<typename... n, typename std::enable_if<all_same<Dims_Type, n...>::value>::type* = nullptr>
        SWIFTEN_I Dimension(const Dims_Type& first_dim, const n&... p_dims)
        {
            this->m_size = 1;
            Num_Dims_Type d = sizeof...(n) + 1;
            this->m_d = d;
            while(d >>= 1)
                this->m_size <<= 1;
            this->m_size <<= 1;
            this->m_dimensions = new Dims_Type[this->m_size];
            this->m_dimensions[0] = first_dim;

            Num_Dims_Type j = 1;
            for(const auto& elem : {p_dims...})
            {
                this->m_dimensions[j++] = elem;
            }
            this->m_n = this->calc_n();
        }

        SWIFTEN_I Dimension& 
        operator=(const Dimension<Num_Dims_Type, Dims_Type>& other) 
        {
            if(this->m_size < other.m_d) 
            {
                delete[] this->m_dimensions;
                while(this->m_size < other.m_d)
                    this->m_size <<= 1;
                this->m_dimensions = new Dims_Type[this->m_size];
            }
            this->m_d = other.m_d;
            f(this->m_d) this->m_dimensions[i] = other.m_dimensions[i];
            this->m_n = other.m_n;
            return *this;
        }

        SWIFTEN_I bool 
        operator<(const Dimension& other) const 
        {
            f(this->m_d) if(this->m_dimensions[i] >= other.m_dimensions[i]) return false;
            return true;
        }

        SWIFTEN_I bool 
        operator<=(const Dimension& other) const 
        {
            f(this->m_d) if(this->m_dimensions[i] > other.m_dimensions[i]) return false;
            return true;
        }

        SWIFTEN_I bool 
        operator>(const Dimension& other) const 
        {
            f(this->m_d) if(this->m_dimensions[i] <= other.m_dimensions[i]) return false;
            return true;
        }

        SWIFTEN_I bool 
        operator>=(const Dimension& other) const 
        {
            f(this->m_d) if(this->m_dimensions[i] < other.m_dimensions[i]) return false;
            return true;
        }

        SWIFTEN_I bool
        operator==(const Dimension& other)
        {
            f(this->m_d) if(this->m_dimensions[i] != other.m_dimensions[i]) return false;
            return true;
        }

        SWIFTEN_I bool
        operator!=(const Dimension& other)
        {
            f(this->m_d) if(this->m_dimensions[i] != other.m_dimensions[i]) return true;
            return false;
        }

        SWIFTEN_I Dims_Type&
        operator[](const Num_Dims_Type& i) const
        {
            if(i >= this->m_d || i < 0)
                throw std::invalid_argument(SWIFTEN_DIMENSION_IND_OUT_OF_RANGE);
            return this->m_dimensions[i];
        }

        SWIFTEN_I void 
        push(const Dims_Type& x) 
        {
            if(this->m_d < this->m_size) 
            {
                this->m_dimensions[this->m_d] = x;
            } 
            else
            {
                this->m_size *= 2;
                Dims_Type* temp = new Dims_Type[this->m_size];
                f(this->m_d)
                    temp[i] = this->m_dimensions[i];
                temp[this->m_d] = x;
                delete[] this->m_dimensions;
                this->m_dimensions = temp;
            }
            this->m_d++;
            this->m_n *= x;
        }

        SWIFTEN_I void
        pop()
        {
            this->m_d--;
            if(this->m_dimensions[this->m_d] > 0)
                this->m_n /= this->m_dimensions[this->m_d];
        }

        SWIFTEN_I Dimension(Dimension&& other) 
        {
            *this = other;
        }

        SWIFTEN_I Num_Dims_Type 
        d() const 
        {
            return this->m_d;
        }

        SWIFTEN_I Dims_Type 
        n() const 
        {
            return this->m_n;
        }

        ~Dimension() 
        {
            if(this->m_dimensions != nullptr)
                delete[] this->m_dimensions;
        }

        friend std::ostream& operator<<(std::ostream& os, const Dimension& dim)
        {
            os << MAKE_BLUE("DIMENSION") << std::endl;
            os << "D: " << (uint64_t)dim.m_d << std::endl << "| ";
            if(dim.m_d > 0)
            {
                f(dim.m_d - 1) os << (uint64_t)dim.m_dimensions[i] << " | ";
                os << (uint64_t)dim.m_dimensions[dim.m_d - 1] << " |" << std::endl;
            }
            os << "N: " << (uint64_t)dim.n() << std::endl;
            return os;    
        }

    }; //class Dimension

} //namespace Core
} //namespace Swiften
} //namespace Pagoda
