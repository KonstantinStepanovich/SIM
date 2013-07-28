#ifndef _AllocatorDefault_hpp_
#define _AllocatorDefault_hpp_

namespace Sim
{

class AllocatorDefault
{
public:
	AllocatorDefault()
	{
	}
	void * allocate(size_t size)
	{
		return malloc(size);
	}
	void deallocate(void * ptr, size_t size)
	{
		free(ptr);
	}
};

}

#endif _AllocatorDefault_hpp_