#ifndef OMEGA_VECTOR_ITERATOR_HPP
#define OMEGA_VECTOR_ITERATOR_HPP

#include <utility>
#include <iterator>

namespace omega
{
    template<typename T, bool is_const_iter>
    class base_iterator;

    template<typename value_type>
    base_iterator<value_type, false> operator + (const base_iterator<value_type, false>& iter, std::ptrdiff_t n) noexcept;

    template<typename value_type>
    base_iterator<value_type, false> operator + (std::ptrdiff_t n, const base_iterator<value_type, false>& iter) noexcept;

    template<typename value_type>
    base_iterator<value_type, true> operator + (const base_iterator<value_type, true>& iter, std::ptrdiff_t n) noexcept;

    template<typename value_type>
    base_iterator<value_type, true> operator + (std::ptrdiff_t n, const base_iterator<value_type, true>& iter) noexcept;

    template<typename T, bool is_const_iter = true>
    class base_iterator
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

        explicit base_iterator(ValuePointerType pointer)
            : m_pointer(pointer)
        {
        }

        base_iterator(const base_iterator<T, false>& rhs)
            : m_pointer(rhs.m_pointer)
        {
        }

        bool operator == (const base_iterator& rhs) const noexcept
        {
            return m_pointer == rhs.m_pointer;
        }

        bool operator != (const base_iterator& rhs) const noexcept
        {
            return !(*this == rhs);
        }

        bool operator < (const base_iterator& rhs) const noexcept
        {
            return m_pointer < rhs.m_pointer;
        }

        bool operator <= (const base_iterator& rhs) const noexcept
        {
            return m_pointer <= rhs.m_pointer;
        }

        bool operator > (const base_iterator& rhs) const noexcept
        {
            return m_pointer > rhs.m_pointer;
        }

        bool operator >= (const base_iterator& rhs) const noexcept
        {
            return m_pointer >= rhs.m_pointer;
        }

        ValueReferenceType operator * ()
        {
            return *m_pointer;
        }

        ValueReferenceType operator [] (difference_type n) 
        {
            return *(m_pointer + n);
        }

        base_iterator &operator -- () noexcept
        {
            --m_pointer;
            return *this;
        }

        base_iterator operator -- (int) noexcept
        {
            auto old(*this);
            --(*this);
            return old;
        }

        base_iterator &operator ++ () noexcept
        {
            ++m_pointer;
            return *this;
        }

        base_iterator operator ++ (int) noexcept
        {
            auto old(*this);
            ++(*this);
            return old;
        }

        base_iterator& operator += (difference_type n) noexcept
        {
            m_pointer += n;
            return *this;
        }

        base_iterator& operator -= (difference_type n) noexcept
        {
            m_pointer -= n;
            return *this;
        }

        difference_type operator - (const base_iterator& rhs) const noexcept
        {
            return m_pointer - rhs.m_pointer;
        }

        void swap(base_iterator& rhs) const noexcept
        {
            std::swap(m_pointer, rhs.m_pointer);
        }

        ValuePointerType getPointer() const noexcept
        {
            return m_pointer;
        }

    private:
        friend class base_iterator<T, true>;
        friend class base_iterator<T, false>;
        friend base_iterator<T, false> operator + <T> (const base_iterator<T, false>&, difference_type) noexcept;
        friend base_iterator<T, false> operator + <T> (difference_type, const base_iterator<T, false>&) noexcept;
        friend base_iterator<T, true> operator + <T> (const base_iterator<T>&, difference_type) noexcept;
        friend base_iterator<T, true> operator + <T> (difference_type, const base_iterator<T>&) noexcept;

        friend base_iterator operator - (const base_iterator& iter, std::ptrdiff_t n) noexcept
        {
            return base_iterator(iter.m_pointer - n);
        }

        ValuePointerType m_pointer;
    };

    template<typename value_type>
    base_iterator<value_type, false> operator + (const base_iterator<value_type, false>& iter, std::ptrdiff_t n) noexcept
    {
        return base_iterator<value_type, false>(iter.m_pointer + n);
    }

    template<typename value_type>
    base_iterator<value_type, false> operator + (std::ptrdiff_t n, const base_iterator<value_type, false>& iter) noexcept
    {
        return base_iterator<value_type, false>(iter.m_pointer + n);
    }

    template<typename value_type>
    base_iterator<value_type> operator + (const base_iterator<value_type>& iter, std::ptrdiff_t n) noexcept
    {
        return base_iterator<value_type>(iter.m_pointer + n);
    }

    template<typename value_type>
    base_iterator<value_type> operator + (std::ptrdiff_t n, const base_iterator<value_type>& iter) noexcept
    {
        return base_iterator<value_type>(iter.m_pointer + n);
    }
}

#endif //OMEGA_VECTOR_ITERATOR_HPP
