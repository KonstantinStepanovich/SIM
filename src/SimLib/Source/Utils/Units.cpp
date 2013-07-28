#include "Utils/Units.hpp"

#include <algorithm>

#include "Database/Variant.hpp"

namespace Sim
{

void load(const Variant & variant, Unit & unit)
{
	unit.name = variant.index(0).getString();
	unit.coeff = variant.index(1);
}

}

using namespace Sim;

UnitManager::UnitManager() : unitSystem_(UNIT_SYSTEM_METRIC)
{
}

UnitManager & UnitManager::instance()
{
	static UnitManager unitManager;
	return unitManager;
}

void UnitManager::load(const Variant & variant)
{
	Variant & vUnitName = variant.key("name");
	for(unsigned int unitId = UNIT_DISTANCE;
		unitId < UNIT_MAX;
		unitId++) 
	{
		unitsName_[unitId] = vUnitName.index(unitId).getString();
	}
	Variant & vUnits = variant.key("units");
	for(unsigned int unitSystemId = UNIT_SYSTEM_NULL;
		unitSystemId < UNIT_SYSTEM_MAX;
		unitSystemId++) 
	{
		Variant & vUnitSystem = vUnits.index(unitSystemId);
		for(unsigned int unitId = UNIT_DISTANCE;
			unitId < UNIT_MAX;
			unitId++) 
		{
			::load(vUnitSystem.index(unitId), units_[unitSystemId][unitId]);
		}
	}
}

const Unit & UnitManager::getUnit(UnitId unitId) const
{
	static const Unit unitNo = { "", 1.0 };
	return unitId != UNIT_NO ? units_[unitSystem_][unitId] : unitNo;
}

UnitId UnitManager::getUnitId(const std::string & name) const
{
	for(unsigned int unitSystemId = UNIT_SYSTEM_NULL;
		unitSystemId < UNIT_SYSTEM_MAX;
		unitSystemId++) 
	{
		const std::string * unitName = std::find(unitsName_, unitsName_ + UNIT_MAX, name);
		return unitName != unitsName_ + UNIT_MAX ? (UnitId)(UNIT_DISTANCE + unitName - unitsName_): UNIT_NO;
	}
	return UNIT_NO;
}