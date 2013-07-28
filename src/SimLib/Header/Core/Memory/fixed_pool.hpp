#ifndef _POOL_ALLOCATOR_
#define _POOL_ALLOCATOR_

#include <xmemory>
#include <assert.h>

template
    <
        class T,
        std::size_t stack_based_capacity = 256,
        class SecondaryAllocator = std::allocator<T>
    >
    class fixed_pool;

template <std::size_t capacity, class Alloc>
    class fixed_pool<void, capacity, Alloc>
{
public:
    typedef void*       pointer;
    typedef void const* const_pointer;
    typedef void value_type;
    template <class Tx>
        struct rebind
            { typedef fixed_pool<Tx> other; };
};

template <class T, std::size_t capacity, class Alloc>
    class fixed_pool : private Alloc
{
public:
    typedef std::size_t     size_type;
    typedef std::ptrdiff_t  difference_type;
    typedef T*              pointer;
    typedef T const*        const_pointer;
    typedef T&              reference;
    typedef T const&        const_reference;
    typedef T               value_type;
    template <class Tx>
        struct rebind
            { typedef fixed_pool<Tx, capacity, Alloc> other; };

    fixed_pool() : top(0) {}
    fixed_pool(fixed_pool const& v) : Alloc(v), top(0) {}
    template <class Tx, std::size_t capacity_x, class Alloc_x>
        fixed_pool(fixed_pool<Tx, capacity_x, Alloc_x> const& v) : Alloc(v), top(0) {}

    pointer address(reference x) const
        { return &x; }

    const_pointer address(const_reference x) const
        { return &x; }

    pointer allocate(size_type n)
		{
			if(top + n <= capacity)
			{			
				pointer p = reinterpret_cast<pointer>(&(m_stack_based_storage[top * sizeof(T)]));
				top = top + n;
				return p;
			}
			else
			{
				return Alloc::allocate(n);
			}		
		}

    pointer allocate(size_type size, void *)
        { return allocate(size); }

    void deallocate(pointer p, size_type n)
		{
			char * pC = reinterpret_cast<char*>(p);
			if(	pC >= m_stack_based_storage &&
				pC <= m_stack_based_storage + (capacity - 1)  * sizeof(T))
			{
				assert(pC + n * sizeof(T) <= m_stack_based_storage + (capacity - 1) * sizeof(T));
				if(pC + n * sizeof(T) == m_stack_based_storage + top * sizeof(T))
				{
					top = top - n;
				}
			}
			else
			{
				Alloc::deallocate(p, n);
			}
		}

    size_type max_size() const
        { return Alloc::max_size(); }

    void construct(pointer p, T const& val)
        { new(p) T(val); }

    void destroy(pointer p)
        { p->~T(); }

private:
	char	m_stack_based_storage[capacity * sizeof(T)];
	size_t	top;
    
    template <class Tx, std::size_t capacity_x, class Alloc_x>
        friend class fixed_pool;
};

#endif _POOL_ALLOCATOR_