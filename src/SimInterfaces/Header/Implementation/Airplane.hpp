#ifndef _Airplane_hpp_
#define _Airplane_hpp_

#include "Interface/IAirplane.hpp"

namespace Sim
{

namespace Interfaces
{

class Airplane : public IAirplane
{
public:
	Airplane();
	inline void setElevatorAxisValue(float elevatorAxis) { elevatorAxis_ = elevatorAxis; }
	inline void setAileronAxisValue(float aileronAxis) { aileronAxis_ = aileronAxis; }
	inline void setRudderAxisValue(float rudderAxis) { rudderAxis_ = rudderAxis; }
	inline void setAfterburner(bool afterburner) { afterburner_ = afterburner; }
	inline void setThrottleAxisValue(float throttleAxis) { throttleAxis_ = throttleAxis; }
	inline void setFlaps(float flaps) { flaps_ = flaps; }
	virtual float getElevatorAxisValue() const;
	virtual float getAileronAxisValue() const;
	virtual float getRudderAxisValue() const;
	virtual bool isAfterburnerEngaged() const;
	virtual float getThrottleAxisValue() const;
	virtual float getFlaps() const;
	virtual bool getGear() const;
protected:
	float elevatorAxis_;
	float aileronAxis_;
	float rudderAxis_;
	bool afterburner_;
	float throttleAxis_;
	float flaps_;
	bool gear_;
};

}

}

#endif _IAirplane_hpp_