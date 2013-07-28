#ifndef _PhysicsODE_Geometry_hpp_
#define _PhysicsODE_Geometry_hpp_

#include <vector>

#include <ode/common.h>

#include "Database/Entity.hpp"

namespace Sim
{

class Variant;

namespace Interfaces
{
class IEntity;
class IAnimated;
}

using Interfaces::IEntity;
using Interfaces::IAnimated;

namespace Math
{
class Box3;
}

namespace PhysicsODE
{

class GeometryNode;
class GeometryAnimation;

typedef std::vector<GeometryAnimation*> AnimationGroup;
typedef std::vector<AnimationGroup> AnimationGroups;

class Geometry : public DescBase
{
public:
	static std::string path;
	Geometry(dSpaceID parentSpaceID, dBodyID bodyID);
	~Geometry();
	void load(const Variant & variant);
	//void setParentSpace(dSpaceID parentSpaceID);
	//Geometry(const Geometry & geometry);
	void setDynamic(bool flag);
	void onEntityChanged(IEntity * entity);
	void onAnimationChanged(IAnimated * animated);
	void animate(unsigned int arg, double value);
	const Math::Box3 & getOBB() const;
private:
	dSpaceID spaceID_;
	dBodyID bodyID_;
	typedef GeometryNode Node;
	std::auto_ptr<Node> root_;
	AnimationGroups animationGroups_;
};

}

}

#endif _PhysicsODE_Geometry_hpp_