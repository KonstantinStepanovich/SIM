#ifndef _ShapeGen_TargetStorage_hpp_
#define _ShapeGen_TargetStorage_hpp_

#include <list>
#include <algorithm>

namespace Sim
{

namespace ShapeGen
{

class BodyObject;

class TargetStorage
{
	TargetStorage();
public:	
	static TargetStorage & instance();
	inline void add(BodyObject * target)
	{
		targets.push_back(target);
	}
	inline void remove(BodyObject * target)
	{
		targets.remove(target);
	}
	template<class Func>
	inline void iterate(const Func & func)
	{
		std::for_each(targets_.begin(), targets_.end(), func);
	}
private:
	typedef std::list<BodyObject*> Targets;
	Targets targets_;
};

}

}

#endif _ShapeGen_TargetStorage_hpp_