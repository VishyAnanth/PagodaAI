#include <stdint.h>

template <typename ...Ts>
class Module {
private:
    uint64_t m_id;
public:
    constexpr uint64_t getId() const {
        return this->m_id;
    }
    constexpr void setId(uint64_t p_id) {
        this->m_id = p_id;
    }
};

template <class T, class ... Ts>
class Module<T, Ts...> : Module<Ts...> {
private:
    T* m_first = nullptr;
    Module<Ts...> m_rem;
public:
    constexpr T*& getFirst() const {
        return this->m_first;
    }
};