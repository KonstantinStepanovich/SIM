#ifndef _Units_hpp_
#define _Units_hpp_

#include <string>

namespace Sim
{

class Variant;

enum UnitId
{
	UNIT_NO = -1,
	UNIT_DISTANCE,
	UNIT_VELOCITY,
	UNIT_SIZE,	
	VERTICAL_VELOCITY,
	UNIT_ANGLE,
	UNIT_ANGULAR_VELOCITY,
	UNIT_MASS,
	UNIT_PRESSURE,
	UNIT_THRUST,
	UNIT_MAX
};

enum UnitSystemId
{
	UNIT_SYSTEM_NULL = 0,
	UNIT_SYSTEM_METRIC = UNIT_SYSTEM_NULL,
	UNIT_SYSTEM_IMPERIAL,
	UNIT_SYSTEM_MAX
};

struct Unit
{
	std::string	name;
	double		coeff;
};

class UnitManager
{
public:
	UnitManager();
	static UnitManager & instance();
	inline void setUnitSystem(UnitSystemId unitSystem) { unitSystem_ = unitSystem; }
	void load(const Variant & variant);
	const Unit & getUnit(UnitId unitId) const;
	UnitId getUnitId(const std::string & name) const;
private:
	UnitSystemId unitSystem_;
	std::string unitsName_[UNIT_MAX];
	Unit units_[UNIT_SYSTEM_MAX][UNIT_MAX];
};

}

#endif _Units_hpp_