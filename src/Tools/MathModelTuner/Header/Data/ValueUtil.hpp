#ifndef _ValueUtil_hpp_
#define _ValueUtil_hpp_

#include "data/IValue.hpp"

#include <deque>

ValuePtr nagivate(IValue * src, const Path & path);

ValuePtrC nagivate(const IValue * src, const Path & path);

void copy(IValue * dst, const IValue * src);

inline void copy(ValuePtr dst, ValuePtrC src)
{
	copy(dst.get(), src.get());
}


#endif _ValueUtil_hpp_