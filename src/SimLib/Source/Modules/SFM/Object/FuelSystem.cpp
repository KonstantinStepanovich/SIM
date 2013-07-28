//#include "stdafx.h"

#include "Modules/SFM/Object/FuelSystem.hpp"

#include "Database/Variant.hpp"

#include <xutility>

using namespace Sim;
using namespace Sim::SFM;

static const FuelSystemState Sim::SFM::fuelSystemDefaultState = {
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false
};

FuelSystem::FuelSystem(const Desc & desc, State & state) :	desc_(desc),
															state_(state)
{
}

void Sim::SFM::load(const Variant & variant, FuelSystemDesc & fuelSystemDesc)
{
	fuelSystemDesc.internalFuelMax = variant.key("internalFuelMax");
	fuelSystemDesc.fuelDumping = variant.key("fuelDumping");
	fuelSystemDesc.fuelDumpRateMax = variant.key("fuelDumpRateMax");
}

void Sim::SFM::save(Variant & variant, const FuelSystemDesc & fuelSystemDesc)
{
	variant["internalFuelMax"] = fuelSystemDesc.internalFuelMax;
	variant["fuelDumping"] = fuelSystemDesc.fuelDumping;
	variant["fuelDumpRateMax"] = fuelSystemDesc.fuelDumpRateMax;
}

float FuelSystem::setInternalFuel(float internalFuel)
{
	return state_.internalFuel = std::min(std::max(-state_.internalFuel, internalFuel), desc_.internalFuelMax);
}

void FuelSystem::simulate(ModelTime dt)
{
	if(!state_.pumpFailure)
	{
		float delta = (-state_.fuelConsumption - state_.fuelDumpRate - state_.fuelLeakage) * dt;
		float externalDelta = std::max(-state_.externalFuel, delta);
		state_.externalFuel += externalDelta;
		delta -= externalDelta;
		float internalDelta = std::max(-state_.internalFuel, delta);
		state_.internalFuel += delta;
		delta -= internalDelta;			
	}
}