#ifndef _ShapeGen_AirplaneObject_hpp_
#define _ShapeGen_AirplaneObject_hpp_

#include "Object.hpp"

#include "Implementation/Body.hpp"
#include "Implementation/Entity.hpp"
#include "Implementation/Animated.hpp"
#include "Interface/IDynamicBody.hpp"
#include "Interface/IMass.hpp"
#include "Interface/IDebugObject.h"

#include "Physic/Atmosphere/IAtmosphere.hpp"

namespace Sim
{

class Variant;

using namespace Framework;

namespace SFM
{
namespace Airplane
{
class Object;
}
}

namespace Interfaces
{
class IAirplane;
}

namespace ShapeGen
{

class Scene;

class ObjectAirplane :	public Object,
						public Interfaces::Body,
						public Interfaces::IDynamicBody,
						public Interfaces::IMass,
						public Interfaces::Entity,
						public Interfaces::IDebugObject,
						public Interfaces::Animated
{
public:
	ObjectAirplane(	const std::string & objectName, ModelTime birthTime, const TypeId & typeId, 
					const Math::Vec3 & position, const Math::Vec3 & velocity, const Math::Quat & rotation, const Math::Vec3 & angularVelocity,
					SFM::Airplane::Object & airplane, const Math::Vec3 & forceCoeffs, const Math::Vec3 & torqueCoeffs);
	~ObjectAirplane();
	virtual const ObjectClass & getClass() const;
	virtual void onEvent(ISubject * subject, Event event);
	virtual Event getEventsOfInterest() const;
	virtual Event getPotentialEvents() const;
	virtual const Math::Vec3 & getForce() const;
	virtual const Math::Vec3 & getTorque() const;
	virtual bool hasMass() const;
	virtual bool getMass(IMass::Data & mass) const;
	virtual const char * getString() const;
	void setDebugState(const std::string & str);
	void update();
private:
	virtual void onPositionChanged_(Interfaces::IPosition * position);
	virtual void onRotationChanged_(Interfaces::IRotation * rotation);
	virtual void onAirplaneChanged_(Interfaces::IAirplane * airplane);
	SFM::Airplane::Object & airplane_;
	mutable Math::Vec3 force_;
	mutable Math::Vec3 torque_;
	Math::Vec3 forceCoeffs_;
	Math::Vec3 torqueCoeffs_;
	std::string debugState_;
};

}

}

#endif _ShapeGen_AirplaneObject_hpp_