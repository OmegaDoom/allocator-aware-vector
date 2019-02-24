#ifndef OMEGA_VECTOR_HELPER_HPP
#define OMEGA_VECTOR_HELPER_HPP

#include <utility>
#include <memory>

namespace omega
{
    template<typename T, typename Allocator>
    class vector_helper
    {
        using alloc_traits = std::allocator_traits<Allocator>;
    public:
        using value_type = T;
        using allocator_type = Allocator;
        using size_type = size_t;
        using difference_type = std::ptrdiff_t;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = typename alloc_traits::pointer;
        using const_pointer = typename alloc_traits::const_pointer;

        explicit vector_helper(allocator_type& alloc)
            : m_allocator{ alloc }
        {
        }

        ~vector_helper()
        {
            for (size_type i = 0; i < m_size; ++i)
            {
                alloc_traits::destroy(m_allocator, &m_data[i]);
            }

            m_size = 0;

            alloc_traits::deallocate(m_allocator, m_data, ITEM_SIZE * m_capacity);
            m_capacity = 0;
        }

        void allocate(size_type capacity)
        {
            m_data = alloc_traits::allocate(m_allocator, ITEM_SIZE * capacity); 
            m_capacity = capacity;
        }

        template <typename... Args>
        pointer construct(Args&&... args)
        {
            alloc_traits::construct(m_allocator, &m_data[m_size], std::forward<Args>(args)...); 
            ++m_size;
            return &m_data[m_size - 1];
        }

        static constexpr size_type ITEM_SIZE = sizeof(T);
        pointer m_data = nullptr;
        size_type m_size = 0;
        size_type m_capacity = 0;
        allocator_type& m_allocator;
    };
}

#endif //OMEGA_VECTOR_HELPER_HPP
