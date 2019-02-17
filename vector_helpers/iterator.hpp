#ifndef OMEGA_VECTOR_ITERATOR_HPP
#define OMEGA_VECTOR_ITERATOR_HPP

#include <utility>
#include <iterator>

namespace omega
{
    template<typename T, bool is_const_iter = true>
    class random_access_iterator
    {
        typedef typename std::conditional<is_const_iter, const T*
                            , T*>::type ValuePointerType;
        typedef typename std::conditional<is_const_iter, const T&
                            , T&>::type ValueReferenceType;

    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = ValuePointerType;
        using reference = ValueReferenceType;

        explicit random_access_iterator(ValuePointerType pointer)
            : m_pointer{ pointer }
        {
        }

        random_access_iterator(const random_access_iterator<T, false>& rhs)
            : m_pointer{ rhs.m_pointer }
        {
        }

        ValueReferenceType operator * ()
        {
            return *m_pointer;
        }

        ValueReferenceType operator [] (difference_type n) 
        {
            return *(m_pointer + n);
        }

        random_access_iterator &operator -- () noexcept
        {
            --m_pointer;
            return *this;
        }

        random_access_iterator operator -- (int) noexcept
        {
            auto old{ *this };
            --(*this);
            return old;
        }

        random_access_iterator &operator ++ () noexcept
        {
            ++m_pointer;
            return *this;
        }

        random_access_iterator operator ++ (int) noexcept
        {
            auto old{ *this };
            ++(*this);
            return old;
        }

        random_access_iterator& operator += (difference_type n) noexcept
        {
            m_pointer += n;
            return *this;
        }

        random_access_iterator& operator -= (difference_type n) noexcept
        {
            m_pointer -= n;
            return *this;
        }

        void swap(random_access_iterator& rhs) noexcept
        {
            std::swap(m_pointer, rhs.m_pointer);
        }

        ValuePointerType getPointer() const noexcept
        {
            return m_pointer;
        }

    private:

        friend random_access_iterator<T>;

        friend bool operator == (const random_access_iterator& lhs, const random_access_iterator& rhs) noexcept
        {
            return lhs.m_pointer == rhs.m_pointer;
        }

        friend bool operator != (const random_access_iterator& lhs, const random_access_iterator& rhs) noexcept
        {
            return !(lhs == rhs);
        }

        friend bool operator < (const random_access_iterator& lhs, const random_access_iterator& rhs) noexcept
        {
            return lhs.m_pointer < rhs.m_pointer;
        }

        friend bool operator <= (const random_access_iterator& lhs, const random_access_iterator& rhs) noexcept
        {
            return lhs.m_pointer <= rhs.m_pointer;
        }

        friend bool operator > (const random_access_iterator& lhs, const random_access_iterator& rhs) noexcept
        {
            return lhs.m_pointer > rhs.m_pointer;
        }

        friend bool operator >= (const random_access_iterator& lhs, const random_access_iterator& rhs) noexcept
        {
            return lhs.m_pointer >= rhs.m_pointer;
        }

        friend random_access_iterator operator + (const random_access_iterator& iter, difference_type n) noexcept
        {
            return random_access_iterator{ iter.m_pointer + n };
        }

        friend random_access_iterator operator + (difference_type n, const random_access_iterator& iter) noexcept
        {
            return random_access_iterator{ iter.m_pointer + n };
        }

        friend random_access_iterator operator - (const random_access_iterator& iter, difference_type n) noexcept
        {
            return random_access_iterator{ iter.m_pointer - n };
        }

        friend difference_type operator - (const random_access_iterator& lhs, const random_access_iterator& rhs) noexcept
        {
            return lhs.m_pointer > rhs.m_pointer ? lhs.m_pointer - rhs.m_pointer : -(rhs.m_pointer - lhs.m_pointer);
        }

        ValuePointerType m_pointer;
    };
}

#endif //OMEGA_VECTOR_ITERATOR_HPP
