#ifndef _SFM_FuelSystem_hpp_
#define _SFM_FuelSystem_hpp_

#include "Core/ModelTime/ModelTime.hpp"

#include "Database/Entity.hpp"

namespace Sim
{

class Variant;

namespace SFM
{

struct FuelSystemDesc
{
	float internalFuelMax; //internal tanks max capacity, kg
	bool fuelDumping; //fuel dumping capability
	float fuelDumpRateMax; //fuel dump maximal rate , kg/s
};

void load(const Variant & variant, FuelSystemDesc & fuelSystemDesc);

void save(Variant & variant, const FuelSystemDesc & fuelSystemDesc);

struct FuelSystemState
{
	float internalFuel; //internal fuel mass, kg
	float externalFuel; //external fuel mass, kg

	float fuelConsumption; // fuel consumption of engines and APU, kg/s
	float fuelDumpRate; // fuel emergency dump rate, kg/s
	float fuelLeakage; // fuel leakage due the damage, kg/s
	bool pumpFailure;
};

extern const FuelSystemState fuelSystemDefaultState;

inline float getFuelMass(const FuelSystemDesc & desc, const FuelSystemState & state)
{
	return state.internalFuel + state.externalFuel;
}

class FuelSystem
{
public:
	typedef FuelSystemDesc Desc;
	typedef FuelSystemState State;
	FuelSystem(const Desc & desc, State & state);
	
	float setInternalFuel(float internalFuel);

	void setFuelConsumption(float fuelConsumption) { state_.fuelConsumption = fuelConsumption; } 
	void setFuelDumpRate(float fuelDumpRate) { state_.fuelDumpRate = fuelDumpRate; } 
	void setFuelLeakage(float fuelLeakage) { state_.fuelLeakage = fuelLeakage; }
	void setPumpFailure	(bool pumpFailure) { state_.pumpFailure = pumpFailure; }
	void simulate(ModelTime dt);
	
	bool isFeeding() const { return getFuel() > 0.0f && !state_.pumpFailure; }
	
	float getFuel() const { return state_.internalFuel + state_.externalFuel; }
	float getInternalFuel() const { return state_.internalFuel; }
	float getExternalFuel() const { return state_.externalFuel; }
private:
	const Desc & desc_;
	State & state_;
};

}

}

#endif _SFM_FuelSystem_hpp_