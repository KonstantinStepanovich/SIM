#ifndef _SFM_JetEngineObject_hpp_
#define _SFM_JetEngineObject_hpp_

#include "Modules/SFM/Model/JetEngine.hpp"
#include "Math/Control.hpp"

#include "Core/ModelTime/ModelTime.hpp"

namespace Sim
{

namespace SFM
{

namespace JetEngine
{

class Object
{
public:
	Object(const Desc & desc);
	void setThrustReq(float thrustReq) { thrustReq_ = thrustReq; state_.afterburner = false; }
	void setAfterburnerThrustReq(float thrustReq) { thrustReq_ = thrustReq; state_.afterburner = true; }
	inline float getThrust() const { return thrust_.get(); }
	inline float getConsumption() const { return consumption_; }
	void simulate(ModelTime dt, const Env & env);
	inline float getThrustRelative() const { return thrustRelative_; }
	inline bool isAfterburnerEngaged() const { return state_.afterburner; }
private:
	const Desc & desc_;
	State state_;
	Cache cache_;
	float thrustReq_;
	Math::FirstOrderFilter<float, ModelTime> thrust_;
	float consumption_; //fuel consumption, kg/N/h	
	float thrustRelative_;
};

}

}

}

#endif _SFM_JetEngineObject_hpp_