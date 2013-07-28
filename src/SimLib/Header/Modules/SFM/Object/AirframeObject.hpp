#ifndef _SFM_AirframeObject_hpp_
#define _SFM_AirframeObject_hpp_

#include "Modules/SFM/Model/Airframe.hpp"

#include "Core/ModelTime/ModelTime.hpp"

#include "Database/Entity.hpp"

namespace Sim
{

const Variant;

namespace SFM
{

namespace Airframe
{

class Object
{
public:
	Object(const Desc & desc, State & state);
	//Control
	void setRudder(float rudder) { state_.torque.rudder = rudder; }
	void setElevator(float elevator) { state_.torque.elevator = elevator; }
	void setAileron(float aileron) { state_.torque.aileron = aileron; }
	float getRudder() const { return state_.torque.rudder; }
	float getElevator() const { return state_.torque.elevator; }
	float getAirleron() const { return state_.torque.aileron; }
	//Mechanics
	void setGear(float gear) { state_.force.gear = gear; }
	void setFlaps(float flaps) { state_.force.flaps = flaps; }
	void setAirBrake(float airBrake) { state_.force.airBrake = airBrake; }
	void setDragParachute(float dragParachute) { state_.force.dragParachute = dragParachute; }

	//Simulation
	void simulate(ModelTime dt, const Env & env);
	const Math::Vec3 & getForce() const { return force_; }
	const Math::Vec3 & getTorque() const { return torque_; }

private:
	const Desc & desc_;
	State & state_;
	Cache cache_;
	Math::Vec3 force_; //local force, N
	Math::Vec3 torque_; //local torque, N*m
};

}

}

}

#endif _SFM_AirframeObject_hpp_