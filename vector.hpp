#ifndef OMEGA_VECTOR_HPP
#define OMEGA_VECTOR_HPP

#include "vector_helpers/random_access_iterator.hpp"
#include "vector_helpers/vector_helper.hpp"
#include <memory>
#include <initializer_list>
#include <stdexcept>

namespace omega
{
    template<typename T, typename Allocator = std::allocator<T>>
    class vector
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
        using const_iterator = random_access_iterator<T>;
        using iterator = random_access_iterator<T, false>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>; 
        using reverse_iterator = std::reverse_iterator<iterator>; 

        explicit vector(const allocator_type& alloc = allocator_type{})
            : m_data{ nullptr }
            , m_size{ 0 }
            , m_capacity{ 0 }
            , m_allocator{ alloc }
        {
        }

        template<typename It>
        vector(It first, It last, const allocator_type& alloc = allocator_type{})
            : vector{ alloc }
        {
            vector_helper<T, allocator_type> temp{ m_allocator };
            temp.allocate(std::distance(first, last));
            for (auto it = first; it != last; ++it)
            {
                temp.construct(*it);
            }
            swap_data(*this, temp);
        }

        template<typename It>
        void assign(It first, It last)
        {
            vector_helper<T, allocator_type> temp{ m_allocator };
            temp.allocate(std::distance(first, last));
            for (auto it = first; it != last; ++it)
            {
                temp.construct(*it);
            }
            swap_data(*this, temp);
        }

        vector(std::initializer_list<T> list, const allocator_type& alloc = allocator_type{})
            : vector{ alloc }
        {
            vector_helper<T, allocator_type> temp{ m_allocator };
            temp.allocate(list.size());
            for (auto& item : list)
            {
                temp.construct(item);
            }
            swap_data(*this, temp);
        }

        void assign(std::initializer_list<T> list)
        {
            vector_helper<T, allocator_type> temp{ m_allocator };
            temp.allocate(list.size());
            for (auto& item : list)
            {
                temp.construct(item);
            }
            swap_data(*this, temp);
        }

        vector(const vector<T, allocator_type>& rhs)
            : m_data{ nullptr }
            , m_size{ 0 }
            , m_capacity{ 0 }
            , m_allocator{ alloc_traits::select_on_container_copy_construction(rhs.m_allocator) }
        {
            vector_helper<T, allocator_type> temp{ m_allocator };
            temp.allocate(rhs.m_size);
            for (size_type i = 0; i < rhs.m_size; i++)
            {
                temp.construct(rhs.m_data[i]);
            }
            swap_data(*this, temp);
        }

        vector(const vector<T, allocator_type>& rhs, const allocator_type& alloc)
            : m_data{ nullptr }
            , m_size{ 0 }
            , m_capacity{ 0 }
            , m_allocator{ alloc }
        {
            vector_helper<T, allocator_type> temp{ m_allocator };
            temp.allocate(rhs.m_size);
            for (size_type i = 0; i < rhs.m_size; i++)
            {
                temp.construct(rhs.m_data[i]);
            }
            swap_data(*this, temp);
        }

        vector(vector<T, allocator_type>&& rhs)
            : m_data{ rhs.m_data }
            , m_size{ rhs.m_size }
            , m_capacity{ rhs.m_capacity }
            , m_allocator{ std::move(rhs.m_allocator) }
        {
            rhs.m_size = 0;
            rhs.m_capacity = 0;
            rhs.m_data = nullptr;
        }

        vector(vector<T, allocator_type>&& rhs, const allocator_type& alloc)
            : m_data{ rhs.m_data }
            , m_size{ rhs.m_size }
            , m_capacity{ rhs.m_capacity }
            , m_allocator{ alloc }
        {
            rhs.m_size = 0;
            rhs.m_capacity = 0;
            rhs.m_data = nullptr;
        }

        vector<T, allocator_type>& operator = (const vector<T, allocator_type>& rhs)
        {
            const bool copy_storage = alloc_traits::propagate_on_container_copy_assignment::value
                                      && m_allocator != rhs.m_allocator;
            if (copy_storage)
            {
                //clearing container in case vector_helper throws
                clear_capacity();
                m_allocator = rhs.m_allocator;
            }
            
            copy_assign(rhs);

            return *this; 
        }

        vector<T, allocator_type>& operator = (vector<T, allocator_type>&& rhs)
        {
            const bool move_storage = alloc_traits::propagate_on_container_move_assignment::value
                                      || m_allocator == rhs.m_allocator; 
 
            if (move_storage)
            {
                move_assign(std::move(rhs)); 
            }
            else
            {
                copy_assign(rhs);
            }

            return *this;
        }

        void push_back(const_reference value)
        {
            push_back_internal(value);
        }

        void push_back(value_type&& value)
        {
            push_back_internal(std::move(value));
        }

        template<typename... Args>
        void emplace_back(Args&&... args)
        {
           push_back_internal(std::forward<Args>(args)...); 
        }

        template<typename It>
        iterator insert(It pos, const_reference value)
        {
            return insert_values_internal(pos, 1, value);
        }

        template<typename It>
        iterator insert(It pos, size_type count, const_reference value)
        {
            return !count ? iterator{ m_data + (pos - cbegin()) } : insert_values_internal(pos, count, value);
        }

        template<typename It>
        iterator insert(It pos, T&& value)
        {
            return insert_values_internal(pos, 1, std::move(value));
        }

        template<typename It, typename InputIt>
        iterator insert(It pos, InputIt first, InputIt last)
        {
            return first == last ? iterator{ m_data + (pos - cbegin()) } : insert_internal(pos, first, last);
        }

        template<typename It>
        iterator insert(It pos, std::initializer_list<T> list) 
        {
            return !list.size() ? iterator{ m_data + (pos - cend()) }
                        : insert_internal(pos, std::begin(list), std::end(list));
        }

        template<typename It, typename... Args>
        iterator emplace(It pos, Args&&... args)
        {
            return insert_values_internal(pos, 1, std::forward<Args>(args)...);
        }

        void pop_back()
        {
            alloc_traits::destroy(m_allocator, &m_data[m_size - 1]);
            --m_size;
        }

        void reserve(size_type new_capacity)
        {
            if (new_capacity <= m_capacity)
            {
                return;
            }
            
            vector_helper<T, allocator_type> temp{ m_allocator };
            temp.allocate(new_capacity);

            for (size_type i = 0; i < m_size; i++)
            {
                temp.construct(std::move_if_noexcept<T>(m_data[i]));
            }

            swap_data(*this, temp);
        }

        void resize(size_type count)
        {
            if (count <= m_size)
            {
                for (size_type i = 0; i < m_size - count; i++)
                {
                    alloc_traits::destroy(m_allocator, &m_data[count + i]);
                } 
                m_size = count;
                return;
            }
                
            resize_to_bigger_size(count, value_type{});
        }

        void resize(size_type count, const_reference value)
        {
            if (count <= m_size)
            {
                for (size_type i = 0; i < m_size - count; i++)
                {
                    alloc_traits::destroy(m_allocator, &m_data[count + i]);
                } 
                m_size = count;
                return;
            }

            resize_to_bigger_size(count, value);
        }

        void assign(size_type count, const_reference value)
        {
            vector_helper<T, allocator_type> temp{ m_allocator };
            temp.allocate(count);

            for (size_type i = 0; i < count; i++)
            {
                temp.construct(value);
            }

            swap_data(*this, temp);
        }

        void shrink_to_fit()
        {
            if (m_capacity == m_size)
            {
                return;
            }

            vector_helper<T, allocator_type> temp{ m_allocator };
            temp.allocate(m_size);

            for (size_type i = 0; i < m_size; i++)
            {
                temp.construct(std::move_if_noexcept<T>(m_data[i]));
            }
            
            swap_data(*this, temp);
        }

        reference operator[](size_type index)
        {
            return m_data[index];
        }

        const_reference operator[](size_type index) const
        {
            return m_data[index];
        }

        reference at(size_type index)
        {
            if (index >= m_size)
            {
                throw std::out_of_range("index out of range");
            }

            return m_data[index];
        }

        const_reference at(size_type index) const
        {
            if (index >= m_size)
            {
                throw std::out_of_range("index out of range");
            }

            return m_data[index];
        }

        pointer data () noexcept
        {
            return m_data;
        } 

        const_pointer data() const noexcept
        {
            return m_data;
        }

        reference front()
        {
            return m_data[0];
        }

        const_reference front() const
        {
            return m_data[0];
        }

        reference back()
        {
            return m_data[m_size - 1];
        }

        const_reference back() const
        {
            return m_data[m_size - 1];
        }

        iterator begin() noexcept
        {
            return m_size ? iterator{ m_data } : iterator{ nullptr };
        } 

        iterator end() noexcept
        {
            return m_size ? iterator{ m_data + m_size } : iterator{ nullptr };
        } 

        const_iterator begin() const noexcept
        {
            return m_size ? const_iterator{ m_data } : const_iterator{ nullptr };
        }

        const_iterator end() const noexcept
        {
            return m_size ? const_iterator{ m_data + m_size } : const_iterator{ nullptr };
        }

        const_iterator cbegin() const noexcept
        {
            return m_size ? const_iterator{ m_data } : const_iterator{ nullptr };
        }

        const_iterator cend() const noexcept
        {
            return m_size ? const_iterator{ m_data + m_size } : const_iterator{ nullptr };
        }

        reverse_iterator rbegin() noexcept
        {
            const auto iter = m_size ? iterator{ m_data + m_size } : iterator{ nullptr };
            return reverse_iterator{ iter };
        } 

        reverse_iterator rend() noexcept
        {
            const auto iter = m_size ? iterator{ m_data } : iterator{ nullptr };
            return reverse_iterator{ iter };
        }

        const_reverse_iterator rbegin() const noexcept
        {
            const auto iter = m_size ? const_iterator{ m_data + m_size } : const_iterator{ nullptr };
            return const_reverse_iterator{ iter };
        }

        const_reverse_iterator rend() const noexcept
        {
            const auto iter = m_size ? const_iterator{ m_data } : const_iterator{ nullptr };
            return const_reverse_iterator{ iter };
        } 

        const_reverse_iterator rcbegin() const noexcept
        {
            const auto iter = m_size ? const_iterator{ m_data + m_size } : const_iterator{ nullptr };
            return const_reverse_iterator{ iter };
        } 

        const_reverse_iterator rcend() const noexcept
        {
            const auto iter = m_size ? const_iterator{ m_data } : const_iterator{ nullptr };
            return const_reverse_iterator{ iter };
        } 

        size_type size() const noexcept
        {
            return m_size;
        }

        bool empty() const noexcept
        {
            return m_size == 0;
        }

        size_type capacity() const noexcept
        {
            return m_capacity;
        }
    
        template <typename It>
        iterator erase(It iter)
        {
            return erase(iter, It{ iter + 1 });
        }

        template <typename It>
        iterator erase(It first, It last)
        {
            if (last.getPointer() >= (m_data + m_size))
            {
                auto pointer = first.getPointer();
                const auto end = m_data + m_size;
                while(pointer < end)
                {
                    alloc_traits::destroy(m_allocator, pointer);
                    ++pointer;
                    --m_size;
                }
                return m_size ? iterator{ m_data + m_size } : iterator{ nullptr };
            }

            vector_helper<T, allocator_type> temp{ m_allocator };
            temp.allocate(m_capacity);

            iterator result(nullptr);
            for (size_type i = 0; i < m_size; i++)
            {
                if ((&m_data[i] < first.getPointer()) || (&m_data[i] >= last.getPointer())) 
                {
                    const auto pointer = temp.construct(std::move_if_noexcept<T>(m_data[i]));
                    if (!result.getPointer() && (&m_data[i] == last.getPointer()))
                        result = iterator{ pointer };
                }
            }

            swap_data(*this, temp);

            return result; 
        }

        void swap(vector<T, allocator_type>& rhs) noexcept
        {
            const bool swap_storage = alloc_traits::propagate_on_container_swap::value
                && m_allocator != rhs.m_allocator;

            using std::swap;
            if (swap_storage)
            {
                swap(m_allocator, rhs.m_allocator);
            }

            std::swap(m_data, rhs.m_data);
            std::swap(m_size, rhs.m_size);
            std::swap(m_capacity, rhs.m_capacity);
        }

        void clear()
        {
            for (size_type i = 0; i < m_size; i++)
            {
                alloc_traits::destroy(m_allocator, &m_data[i]);
            }

            m_size = 0;
        }

        ~vector()
        {
            clear_capacity();
        }

    private:
        void move_assign(vector<T, allocator_type>&& rhs) noexcept
        {
            // Constant-time move assignment when source object's memory can be
            // moved, either because the source's allocator will move too
            // or because the allocators are equal.

            clear_capacity();

            if (m_allocator != rhs.m_allocator)
            {
                m_allocator = std::move(rhs.m_allocator); 
            }

            m_size = rhs.m_size;
            m_capacity = rhs.m_capacity;
            m_data = rhs.m_data;

            rhs.m_size = 0;
            rhs.m_capacity = 0;
            rhs.m_data = nullptr;
        }
 
        void copy_assign(const vector<T, allocator_type>& rhs)
        {
            // Do move assignment when it might not be possible to move source
            // object's memory, resulting in a linear-time operation.

            vector_helper<T, allocator_type> temp{ m_allocator };
            temp.allocate(rhs.m_size);
            for (size_type i = 0; i < rhs.m_size; i++)
            {
                temp.construct(rhs.m_data[i]);
            }
            swap_data(*this, temp);
        }

        void clear_capacity()
        {
            for (size_type i = 0; i < m_size; i++)
            {
                alloc_traits::destroy(m_allocator, &m_data[i]);
            }

            m_size = 0;

            alloc_traits::deallocate(m_allocator, m_data, ITEM_SIZE * m_capacity);
            m_capacity = 0;
        }

        template <typename... Args>
        pointer push(vector<T, allocator_type>& vec, Args&&... args)
        {
            alloc_traits::construct(vec.m_allocator, &vec[vec.m_size], std::forward<Args>(args)...); 
            ++vec.m_size;
            return &vec[vec.m_size - 1];
        }

        template <typename... Args>
        void push_back_internal(Args&&... args)
        {
            if (m_size == m_capacity)
            {
                vector_helper<T, allocator_type> temp{ m_allocator };
                temp.allocate(m_capacity * 2 + 1);
                for (size_type i = 0; i < m_size; i++)
                {
                    temp.construct(std::move_if_noexcept<T>(m_data[i]));
                }
                temp.construct(std::forward<Args>(args)...);
                swap_data(*this, temp);
                return;
            }

            push(*this, std::forward<Args>(args)...);
        }

        template <typename It, typename... Args>
        iterator insert_values_internal(It pos, size_t count, Args&&... args)
        {
            const auto new_size = m_size + count;
            const auto new_capacity = new_size <= m_capacity ? m_capacity : new_size;
            vector_helper<T, allocator_type> temp{ m_allocator };
            temp.allocate(new_capacity);
            const auto copy_index = pos.getPointer() - &m_data[0];

            for (std::ptrdiff_t i = 0; i < copy_index; i++)
            {
                temp.construct(std::move_if_noexcept<T>(m_data[i]));
            }

            const auto result = iterator{ temp.construct(std::forward<Args>(args)...) };

            for (size_type i = 0; i < count - 1; i++)
            {
                temp.construct(std::forward<Args>(args)...);
            }

            for (size_type i = copy_index; i < m_size; i++)
            {
                temp.construct(std::move_if_noexcept<T>(m_data[i]));
            }

            swap_data(*this, temp);
            return result;
        }

        template <typename It, typename InputIt>
        iterator insert_internal(It pos, InputIt first, InputIt last)
        {
            const auto new_size = static_cast<size_t>(m_size + std::distance(first, last));
            const auto new_capacity = new_size <= m_capacity ? m_capacity : new_size;
            vector_helper<T, allocator_type> temp{ m_allocator };
            temp.allocate(new_capacity);
            const auto copy_index = pos.getPointer() - &m_data[0];

            for (std::ptrdiff_t i = 0; i < copy_index; i++)
            {
                temp.construct(std::move_if_noexcept<T>(m_data[i]));
            }

            const auto result = iterator{ temp.construct(*first) };

            auto iter = first;
            ++iter;
            for (; iter != last; ++iter)
            {
                temp.construct(*iter);
            }

            for (size_type i = copy_index; i < m_size; i++)
            {
                temp.construct(std::move_if_noexcept<T>(m_data[i]));
            }

            swap_data(*this, temp);
            return result;
        }

        void resize_to_bigger_size(size_type count, const_reference value)
        {
            size_type new_capacity = count > m_capacity ? count : m_capacity;
            vector_helper<T, allocator_type> temp{ m_allocator };
            temp.allocate(new_capacity);

            for (size_type i = 0; i < m_size; i++)
            {
                temp.construct(std::move_if_noexcept<T>(m_data[i]));
            }

            for (size_type i = 0; i < count - m_size; i++)
            {
                temp.construct(value);
            }

            swap_data(*this, temp);
        }

        void swap_data(vector<T, allocator_type>& first, vector_helper<T, allocator_type>& second) noexcept
        {
            std::swap(first.m_data, second.m_data);
            std::swap(first.m_size, second.m_size);
            std::swap(first.m_capacity, second.m_capacity);
        }

        static constexpr size_type ITEM_SIZE = sizeof(T);
        pointer m_data;
        size_type m_size;
        size_type m_capacity;
        allocator_type m_allocator;
    };
}

#endif //OMEGA_VECTOR_HPP
