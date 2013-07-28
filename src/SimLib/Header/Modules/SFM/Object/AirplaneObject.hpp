#ifndef _SFM_AirplaneObject_hpp_
#define _SFM_AirplaneObject_hpp_

#include "Database/Entity.hpp"

#include "Modules/SFM/Model/Airplane.hpp"
#include "Modules/SFM/Object/AirframeObject.hpp"
#include "Modules/SFM/Object/JetEngineObject.hpp"
#include "Modules/SFM/Object/FuelSystem.hpp"
#include "Modules/SFM/Actuators.hpp"

namespace Sim {

namespace SFM {

namespace Airplane {

class Object
{
public:
	Object(const Desc & desc);
	inline const Desc & getDesc() const { return desc_; }

	//Initialization
	void setFuel(float fuelMass);
	
	//Control
	void setRudder(float rudder);
	void setElevator(float elevator);
	void setAileron(float aileron);
	void setThrust(float thrust);
	void setAfterburnerThrust(float aftThrust);
	void setGear(bool gear) { gearReq_ = gear; }
	void setFlaps(float flaps) { flapsReq_ = flaps; }
	void setAirBrake(float airBrake) { airBrakeReq_ = airBrake; }
	
	float getRudder() const { return airframe_.getRudder(); }
	float getElevator() const { return airframe_.getElevator(); }
	float getAirleron() const { return airframe_.getAirleron(); }

	float getFlaps() const { return flapsActuator_.getValue(); }
	float getGear() const { return gearActuator_.getValue(); }
	PayloadTree::iterator addPayload(PayloadTree::iterator where, const Payload & payload);
	PayloadTree::iterator removePayload(PayloadTree::iterator where);
	PayloadTree::iterator getPayloadRoot() { return payloadTree_.root(); }
	//Simulation
	struct Environment
	{
		Math::Vec3 TAS; //local flow velocity, m/s
		Math::Vec3 omega; //local angular speed, rad/s
		float rho; //density, kg/m^3
		float M; //Mach number
		float P; //static pressure, Pa
		float T; //temperature, K
	};
	void simulate(ModelTime dt, const Environment & env);
	//State
	const Math::Vec3 & getForce() const { return force_; }
	const Math::Vec3 & getTorque() const { return torque_; }
	float getMass() const;
	Math::Matrix3x3 getInertiaTensor() const;
	float getFuelMass() const;
	float getThrust() const;
	bool isAfterburnerEngaged() const;
	float getThrustRelative() const;
private:
	void driveActuators_(ModelTime dt);
	const Desc & desc_;
	State		state_;
	Airframe::Object airframe_;
	unsigned int levelIndex_; //cached level in alt = f(P)
	typedef ConstRateActuator Actuator;
	bool		gearReq_;
	Actuator	gearActuator_;
	float		flapsReq_;
	Actuator	flapsActuator_;
	float		airBrakeReq_;
	Actuator	airBrakeActuator_;
	JetEngine::Object jetEngine_;
	FuelSystem	fuelSystem_;
	Math::Vec3	force_; //local force, N
	Math::Vec3	torque_; //local torque, N*m
	PayloadTree payloadTree_;
};

}

}

}

#endif _SFM_AirplaneObject_hpp_