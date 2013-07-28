#ifndef _PhysicsPhysX_MultiGeometryBody_hpp_
#define _PhysicsPhysX_MultiGeometryBody_hpp_

#include "Utils/Body.hpp"
#include "Utils/Geometry.hpp"

#include "Database/Entity.hpp"

#include <vector>

namespace physx
{
class PxPhysics;
class PxScene;
}

namespace Sim
{

class Variant;

namespace Interfaces
{
class IEntity;
}

namespace PhysicsPhysX
{

class MultiGeometryBody
{
public:
	MultiGeometryBody(Object * object, physx::PxPhysics * SDK, physx::PxScene * scene);
	~MultiGeometryBody();
	void load(const Variant & variant);
	inline const Body & getBody() const { return body_; }
	inline Body & getBody() { return body_; }
	inline void update() { body_.update(); }
private:
	Body body_;
};

}

}

#endif _PhysicsPhysX_MultiGeometryBody_hpp_