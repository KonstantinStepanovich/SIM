#include "stdafx.h"

#include "TestAirplane.hpp"

#include "Math/Scalar.hpp"
#include "Utils/Units.hpp"

using namespace Sim::SFM::Airplane;

FuncInstTurnNyMax::FuncInstTurnNyMax(const Airplane::Desc & desc, const Airplane::State & state) : 
	desc_(desc), state_(state), cache_(Airplane::defaultCache)
{
}

float FuncInstTurnNyMax::operator()(const Airplane::Env & env) const
{
	return getInstTurnNyMax(desc_, env, state_, cache_);
}

FuncInstTurnRateMax::FuncInstTurnRateMax(const Airplane::Desc & desc, const Airplane::State & state) : 
	desc_(desc), state_(state),	cache_(Airplane::defaultCache)
{
}

float FuncInstTurnRateMax::operator()(const Airplane::Env & env) const
{
	return getInstTurnRateMax(env.TAS, getInstTurnGammaMax(getInstTurnNyMax( desc_, env, state_, cache_)));
}

FuncVy::FuncVy(const Airplane::Desc & desc, const Airplane::State & state) : desc_(desc), state_(state),
	cache_(Airplane::defaultCache)
{
}

float FuncVy::operator()(const Airplane::Env & env) const
{
	return getVyMax(desc_, env, state_, cache_);
}

FuncNyMax::FuncNyMax(const Airplane::Desc & desc, const Airplane::State & state) : desc_(desc), state_(state),
	cache_(Airplane::defaultCache)
{
}

float FuncNyMax::operator()(const Airplane::Env & env) const
{
	return getNyMax(desc_, env, state_, cache_);
}

FuncTurnRateMax::FuncTurnRateMax(const Airplane::Desc & desc, const Airplane::State & state) : desc_(desc), state_(state),
	cache_(Airplane::defaultCache)
{
}

float FuncTurnRateMax::operator()(const Airplane::Env & env) const
{
	return getTurnRateMax(env.TAS, getNyMax(desc_, env, state_, cache_));
}

FuncAoA::FuncAoA(const Airplane::Desc & desc, const Airplane::State & state) : desc_(desc), state_(state),
	cache_(Airplane::defaultCache)
{
}

float FuncAoA::operator()(const Airplane::Env & env) const
{
	return getLevelFlightParam(desc_, env, state_, cache_).AoA;
}

FuncThrust::FuncThrust(const Airplane::Desc & desc, const Airplane::State & state) : desc_(desc), state_(state),
	cache_(Airplane::defaultCache)
{
}

Airplane::Thrust FuncThrust::operator()(const Airplane::Env & env) const
{
	Airplane::Thrust thrust = getThrust(desc_, env, state_, cache_);
	thrust.abs /= g;
	return thrust;
}

FuncConsumption::FuncConsumption(const Airplane::Desc & desc, const Airplane::State & state) : desc_(desc), state_(state),
	cache_(Airplane::defaultCache)
{
}

float FuncConsumption::operator()(const Airplane::Env & env) const
{
	return getConsumption(desc_, env, state_, cache_);
}

FuncRange::FuncRange(const Airplane::Desc & desc, const Airplane::State & state) : desc_(desc), state_(state),
	cache_(Airplane::defaultCache)
{
}

float FuncRange::operator()(const Airplane::Env & env) const
{
	Airplane::State meanState = state_;
	meanState.fuelSystem.internalFuel = desc_.fuelSystem.internalFuelMax * 0.5f;
	float consumption = getConsumption(desc_, env, meanState, cache_);
	return consumption != Math::invalid<float>() ? meanState.fuelSystem.internalFuel / consumption * env.TAS : Math::invalid<float>();
}

FuncMaxNx::FuncMaxNx(const Airplane::Desc & desc, const Airplane::State & state) : desc_(desc), state_(state),
	cache_(Airplane::defaultCache)
{
}

float FuncMaxNx::operator()(const Airplane::Env & env) const
{
	return getNxMax(desc_, env, state_, cache_);
}

TasMin::TasMin(const Airplane::Desc & desc, const Airplane::State & state) : desc_(desc), state_(state), cache_(Airplane::defaultCache)
{
}

float TasMin::operator()(const Airplane::Env & env) const
{
	return getTASMin(desc_, env, state_, cache_);
}

TasMax::TasMax(const Airplane::Desc & desc, const Airplane::State & state) : desc_(desc), state_(state), cache_(Airplane::defaultCache)
{

}

float TasMax::operator()(const Airplane::Env & env) const
{
	float result = getTASMax(desc_, env, state_, cache_);
	return result;
}

FuncEnduranceSpeed::FuncEnduranceSpeed(const Airplane::Desc & desc, const Airplane::State & state) : desc_(desc), state_(state),
	cache_(Airplane::defaultCache)
{
}

float FuncEnduranceSpeed::operator()(const Airplane::Env & env) const
{
	return getTASEndurance(desc_, env, state_, cache_);
}

FuncCruiseSpeed::FuncCruiseSpeed(const Airplane::Desc & desc, const Airplane::State & state) : desc_(desc), state_(state),
	cache_(Airplane::defaultCache)
{
}

float FuncCruiseSpeed::operator()(const Airplane::Env & env) const
{
	return getTASCruise(desc_, env, state_, cache_);
}

FuncMaxVyTAS::FuncMaxVyTAS(const Airplane::Desc & desc, const Airplane::State & state) : desc_(desc), state_(state),
	cache_(Airplane::defaultCache)
{
}

float FuncMaxVyTAS::operator()(const Airplane::Env & env) const
{
	return getMaxVyTAS(desc_, env, state_, 0.0f, 1.0E10f, cache_);
}