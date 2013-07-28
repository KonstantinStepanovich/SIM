#ifndef _IUnit_hpp_
#define _IUnit_hpp_

namespace Sim
{

namespace Interfaces
{

enum Coalition
{
	COALITION_UNKNOWN = -1,
	COALITION_NEUTRAL,
	COALITION_RED,
	COALITION_BLUE,
	COALITION_MAX
};

typedef std::string Country;

class IUnit
{
public:
	virtual ~IUnit() {;}
	virtual bool alive() const = 0;
	virtual bool active() const = 0;
	virtual Coalition getCoalition() const = 0;
	virtual const Country & getCountry() const = 0;
	
};

}

}

#endif _IUnit_hpp_