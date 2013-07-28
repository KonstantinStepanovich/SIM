#ifndef _IAirplane_hpp_
#define _IAirplane_hpp_

namespace Sim
{

namespace Interfaces
{

class IAirplane
{
public:
	virtual ~IAirplane() {;}
	virtual float getElevatorAxisValue() const = 0;
	virtual float getAileronAxisValue() const = 0;
	virtual float getRudderAxisValue() const = 0;
	virtual bool isAfterburnerEngaged() const = 0;
	virtual float getThrottleAxisValue() const = 0;
	virtual float getFlaps() const = 0;
	virtual bool getGear() const = 0;
};

}

}

#endif _IAirplane_hpp_