//#include "stdafx.h"

#include "Modules/SFM/Object/AirplaneObject.hpp"

#include "Modules/Satm1976/Satm1976.hpp"
#include "Math/Equation.hpp"
#include "Physic/Gas.hpp"

using namespace Sim;
using namespace Sim::SFM::Airplane;

Object::Object(const Desc & desc) :	desc_(desc),
									state_(defaultState),
									airframe_(Airframe::Database::instance().getDesc(desc.airframeId), state_.airframe),
									levelIndex_(-1),
									gearReq_(false),
									gearActuator_(0.5f),
									flapsReq_(0.0f),
									flapsActuator_(0.5f),
									airBrakeReq_(0.0f),
									airBrakeActuator_(0.5f),
									jetEngine_(JetEngine::Database::instance().getDesc(desc.jetEngineId)),
									fuelSystem_(desc.fuelSystem, state_.fuelSystem),
									force_(0.0f, 0.0f, 0.0f),
									torque_(0.0f, 0.0f, 0.0f)
{
	
}

void Object::setFuel(float fuelMass)
{
	fuelSystem_.setInternalFuel(fuelMass);
}

void Object::setRudder(float rudder)
{
	airframe_.setRudder(rudder);
}

void Object::setElevator(float elevator)
{
	airframe_.setElevator(elevator);
}

void Object::setAileron(float aileron)
{
	airframe_.setAileron(aileron);
}

void Object::setThrust(float thrust)
{
	jetEngine_.setThrustReq(thrust);
}

void Object::setAfterburnerThrust(float aftThrust)
{
	jetEngine_.setAfterburnerThrustReq(aftThrust);
}

SFM::PayloadTree::iterator Object::addPayload(SFM::PayloadTree::iterator where, const SFM::Payload & payload)
{
	SFM::PayloadTree::iterator it = payloadTree_.insert(where, payload);
	Airplane::addPayload(state_, payloadTree_, it);
	return it;
}

SFM::PayloadTree::iterator Object::removePayload(SFM::PayloadTree::iterator where)
{
	Airplane::removePayload(state_, payloadTree_, where);
	return payloadTree_.erase(where);
}

void Object::simulate(ModelTime dt, const Environment & env)
{
	//actuators
	driveActuators_(dt);

	//Satm1976 altitude by static pressure
	levelIndex_ = (levelIndex_ == -1) ?  Satm1976::getLevelByPr(env.P / Satm1976::P0) : Satm1976::updateLevelByPr(levelIndex_, env.P / Satm1976::P0);
	float altKmSatm1976 = Satm1976::getAltKmByPr(levelIndex_, env.P / Satm1976::P0);

	Satm1976::AtmParam atmParam;
	Satm1976::getAtmParamByAltKm(altKmSatm1976, levelIndex_, atmParam);

	//engines
	float propulsion;
	if(fuelSystem_.isFeeding())
	{
		JetEngine::Environment jetEnv;
		jetEnv.altKm = altKmSatm1976;
		jetEnv.M = env.M;
		jetEnv.dT = env.T - Satm1976::T0 * atmParam.Tr;
		jetEngine_.simulate(dt, jetEnv);
		propulsion = jetEngine_.getThrust();
	}
	else
		propulsion = 0.0f;
	force_ = Math::Vec3(desc_.engineCount * propulsion, 0.0f, 0.0f);

	//fuel system
	fuelSystem_.setFuelConsumption(desc_.engineCount * jetEngine_.getConsumption());
	fuelSystem_.simulate(dt);

	//airframe
	airframe_.setGear(gearActuator_.getValue());
	airframe_.setFlaps(flapsActuator_.getValue());
	airframe_.setAirBrake(airBrakeActuator_.getValue());
	Airframe::Env airframeEnv;
	airframeEnv.force.TAS = env.TAS;
	airframeEnv.force.M = env.M;
	airframeEnv.force.rho = env.rho;
	airframeEnv.torque.TAS = env.TAS;
	airframeEnv.torque.flagTAS = true;
	airframeEnv.torque.omega = env.omega;
	airframeEnv.torque.rho = env.rho;
	airframe_.simulate(dt, airframeEnv);
	force_ += airframe_.getForce();
	torque_ = airframe_.getTorque();
}

float Object::getMass() const
{
	return ::getMass(desc_, state_);
}

Math::Matrix3x3 Object::getInertiaTensor() const
{
	return ::getInertiaTensor(desc_, state_);
}

float Object::getFuelMass() const
{
	return ::getFuelMass(desc_, state_);
}

float Object::getThrust() const
{
	return jetEngine_.getThrust() * desc_.engineCount;
}

bool Object::isAfterburnerEngaged() const
{
	return jetEngine_.isAfterburnerEngaged();
}

float Object::getThrustRelative() const
{
	return jetEngine_.getThrustRelative();
}

void Object::driveActuators_(ModelTime dt)
{
	gearActuator_.simulate(gearReq_ ? 1.0f : 0.0f, dt);
	flapsActuator_.simulate(flapsReq_, dt);
	airBrakeActuator_.simulate(airBrakeReq_, dt);
}