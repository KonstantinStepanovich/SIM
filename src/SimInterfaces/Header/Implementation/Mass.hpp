#ifndef _Mass_hpp_
#define _Mass_hpp_

#include "Interface/IMass.hpp"

namespace Sim
{

namespace Interfaces
{

extern const float NoMass;
extern const float NoDensity;

class Mass : public IMass
{
public:
	Mass(const Data & data);
	virtual bool hasMass() const;
	virtual bool getMass(Data & data) const;
private:
	Data data_;
};

IMass::Data makeNoMass();

}

}

#endif _Mass_hpp_