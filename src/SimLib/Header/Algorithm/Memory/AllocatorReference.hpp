#ifndef _AllocatorReference_hpp_
#define _AllocatorReference_hpp_

namespace Sim
{

template<typename Allocator>
class AllocatorReference
{
public:
	AllocatorReference(Allocator & ref) : ref_(ref)
	{
	}
	void * allocate(size_t size)
	{
		return ref_.allocate(size);
	}
	void deallocate(void * ptr, size_t size)
	{
		ref_.deallocate(ptr, size);
	}
private:
	Allocator & ref_;
};

}

#endif _AllocatorReference_hpp_