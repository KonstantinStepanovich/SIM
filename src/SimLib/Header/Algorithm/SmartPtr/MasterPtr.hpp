#ifndef _MasterPtr_hpp_
#define _MasterPtr_hpp_

template<typename T, class Allocator>
class MasterPtr
{
public:
	MasterPtr(const Allocator & allocator) : allocator_(allocator)
	{
	}
	template<typename Arg1>
	MasterPtr(const Arg1 & arg1, const Allocator & allocator) : allocator_(allocator)
	{
	}
	template<typename Arg1, typename Arg2>
	MasterPtr(const Arg1 & arg1, const Arg2 & arg2, const Allocator & allocator) : allocator_(allocator)
	{
	}
	template<typename Arg1, typename Arg2, typename Arg3>
	MasterPtr(const Arg1 & arg1, const Arg2 & arg2, const Arg3 & arg3, const Allocator & allocator) : allocator_(allocator)
	{
	}
	template<typename Arg1, typename Arg2, typename Arg3, typename Arg4>
	MasterPtr(const Arg1 & arg1, const Arg2 & arg2, const Arg3 & arg3, const Arg4 & arg4, const Allocator & allocator) : allocator_(allocator)
	{
	}
	template<typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
	MasterPtr(const Arg1 & arg1, const Arg2 & arg2, const Arg3 & arg3, const Arg4 & arg4, const Arg5 & arg5, const Allocator & allocator) : allocator_(allocator)
	{
	}
	T * operator->()
	{
		return ptr_;
	}
	const T * operator->() const
	{
		return ptr_;
	}
private:
	T * ptr_;
	Allocator allocator_;
};

#endif _MasterPtr_hpp_