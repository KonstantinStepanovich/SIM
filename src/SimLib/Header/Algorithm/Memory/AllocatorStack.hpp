#ifndef _AllocatorStack_hpp_
#define _AllocatorStack_hpp_

#include <assert.h>

namespace Sim
{

template<size_t size>
class AllocatorStack
{
public:
	enum { SIZE = size };
	AllocatorStack() : top_(0)
	{		
	}
	~AllocatorStack()
	{
		assert(top_ == 0);
	}
	void * allocate(size_t blockSize)
	{
		void * block = &buff_[top_];
		assert(top_ + blockSize < size);
		top_ += blockSize;
		return block;
	}
	void deallocate(void * ptr, size_t blockSize)
	{
		assert((char*)ptr + blockSize == buff_ + top_);
		top_ -= blockSize;
	}
private:
	char	buff_[size];
	size_t	top_;
};

}

#endif _AllocatorStack_hpp_