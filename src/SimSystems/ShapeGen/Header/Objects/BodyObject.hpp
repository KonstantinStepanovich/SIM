#ifndef _ShapeGen_Body_hpp_
#define _ShapeGen_Body_hpp_

#include "ISystem.hpp"
#include "Object.hpp"

#include "Implementation/Entity.hpp"
#include "Implementation/Body.hpp"
#include "Implementation/Mass.hpp"
#include "Implementation/Animated.hpp"

#include "Math/Scalar.hpp"
#include "Math/Vec3.hpp"
#include "Math/Box3.hpp"

namespace Sim
{

namespace Interfaces
{
class IPosition;
class IMass;
}

using namespace Framework;
using namespace Interfaces;

namespace ShapeGen
{
class BodyObject : 	public Object,
					public Interfaces::Entity,
					public Interfaces::Animated,
					public Interfaces::Body,
					public Interfaces::Mass
{
public:
	BodyObject(	const ObjectName & objectName, const TypeId & typeId,
				const Math::Vec3 & position, const Math::Vec3 & velocity,
				const Math::Quat & rotation, const Math::Vec3 & angularVelocity,
				const IMass::Data & mass, ModelTime birthTime);
	~BodyObject();
	virtual const ObjectClass & getClass() const;
	virtual Math::Box3 getOBB() const;
	virtual Event getPotentialEvents() const;
	virtual Event getEventsOfInterest() const;
	virtual void onEvent(ISubject * subject, Event event);
protected:
	ModelTime birthTime_;
};

}

}

#endif _ShapeGen_Body_hpp_