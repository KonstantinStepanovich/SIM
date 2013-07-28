#ifndef _PhysicsODE_MultiGeometryBody_hpp_
#define _PhysicsODE_MultiGeometryBody_hpp_

#include "Utils/Body.hpp"
#include "Utils/Geometry.hpp"

#include "Database/Entity.hpp"

#include <vector>

#include <ode/common.h>

namespace Sim
{

class Variant;

namespace Interfaces
{
class IEntity;
}

namespace PhysicsODE
{

class MultiGeometryBody
{
public:
	MultiGeometryBody();
	~MultiGeometryBody();
	void setParentSpace(dSpaceID parentSpaceID);
	void load(const Variant & variant);
	inline const Body & getBody() const { return body_; }
	inline Body & getBody() { return body_; }
	inline void update() { body_.update(); }
private:
	Body body_;
	dSpaceID spaceID_;
	typedef std::vector<Geometry> Geoms;
	Geoms geoms_;
};

}

}

#endif _PhysicsODE_MultiGeometryBody_hpp_