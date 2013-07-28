#ifndef _TestAirplane_hpp_
#define _TestAirplane_hpp_

#include "Iteration.hpp"

#include "Math/BilinearInterpolation.hpp"
#include "Physic/Gas.hpp"
#include "Utils/Units.hpp"

#include "Modules/SFM/Model/Airplane.hpp"
#include "Modules/SFM/Model/Airframe.hpp"
#include "Modules/SFM/Model/JetEngine.hpp"
#include "Modules/Satm1976/Satm1976.hpp"

using namespace Sim;
using namespace Sim::SFM;

struct TestAirplane
{
	Airplane::Desc desc;
	IterationRange<float> altitude; //altitude, km
	IterationRange<float> TAS; //TAS, km/h
	Airplane::State state;
};

struct FuncInstTurnNyMax : std::unary_function<Airplane::Env, float>
{
	FuncInstTurnNyMax(const Airplane::Desc & desc, const Airplane::State & state);
	float operator()(const Airplane::Env & env) const;
private:
	const Airplane::Desc & desc_;
	Airplane::State state_;
	mutable Airplane::Cache cache_;
};

struct FuncInstTurnRateMax : std::unary_function<Airplane::Env, float>
{
	FuncInstTurnRateMax(const Airplane::Desc & desc, const Airplane::State & state);
	float operator()(const Airplane::Env & env) const;
private:
	const Airplane::Desc & desc_;
	Airplane::State state_;
	mutable Airplane::Cache cache_;
};

template<class Func>
struct FuncAltTAS : std::binary_function<float, float, typename Func::result_type>
{
	FuncAltTAS(Func & func) : func_(func), level_(0) {;}
	inline typename Func::result_type operator()(float alt, float TAS) const
	{
		const float altKm = alt * 1.0E-3;
		level_ = Satm1976::updateLevelByAltKm(level_, altKm);
		Satm1976::AtmParam atmParam;
		Satm1976::getAtmParamByAltKm(altKm, level_, atmParam);
		Airplane::Env env;
		env.P = atmParam.Pr * Satm1976::P0;
		env.T = atmParam.Tr * Satm1976::T0;
		env.M = TAS / getSoundVel(atmParam.Tr * Satm1976::T0);
		env.rho = atmParam.rhor * Satm1976::rho0;
		env.TAS = TAS;
		return func_(env);
	}
private:
	Func func_;
	mutable unsigned int level_;
};

typedef TestAirplane TestTurnRate;

typedef TestAirplane TestVyMax;

struct FuncVy : std::unary_function<Airplane::Env, float>
{
	FuncVy(const Airplane::Desc & desc, const Airplane::State & state);
	float operator()(const Airplane::Env & env) const;
private:
	const Airplane::Desc & desc_;
	Airplane::State state_;
	mutable Airplane::Cache cache_;
};

typedef TestAirplane TestMaxNy;

struct FuncNyMax : std::unary_function<Airplane::Env, float>
{
	FuncNyMax(const Airplane::Desc & desc, const Airplane::State & state);
	float operator()(const Airplane::Env & env) const;
private:
	const Airplane::Desc & desc_;
	Airplane::State state_;
	mutable Airplane::Cache cache_;
};

struct FuncTurnRateMax : std::unary_function<Airplane::Env, float>
{
	FuncTurnRateMax(const Airplane::Desc & desc, const Airplane::State & state);
	float operator()(const Airplane::Env & env) const;
private:
	const Airplane::Desc & desc_;
	Airplane::State state_;
	mutable Airplane::Cache cache_;
};

typedef TestAirplane TestAoAReq;

struct FuncAoA : std::unary_function<Airplane::Env, float>
{
	FuncAoA(const Airplane::Desc & desc, const Airplane::State & state);
	float operator()(const Airplane::Env & env) const;
private:
	const Airplane::Desc & desc_;
	Airplane::State state_;
	mutable Airplane::Cache cache_;
};

typedef TestAirplane TestThrustReq;

struct FuncThrust : std::unary_function<Airplane::Env, Airplane::Thrust>
{
	FuncThrust(const Airplane::Desc & desc, const Airplane::State & state);
	Airplane::Thrust operator()(const Airplane::Env & env) const;
private:
	const Airplane::Desc & desc_;
	Airplane::State state_;
	mutable Airplane::Cache cache_;
};

typedef TestAirplane TestConsumption;

struct FuncConsumption : std::unary_function<Airplane::Env, float>
{
	FuncConsumption(const Airplane::Desc & desc, const Airplane::State & state);
	float operator()(const Airplane::Env & env) const;
private:
	const Airplane::Desc & desc_;
	Airplane::State state_;
	mutable Airplane::Cache cache_;
};

typedef TestAirplane TestRange;

struct FuncRange : std::unary_function<Airplane::Env, float>
{
	FuncRange(const Airplane::Desc & desc, const Airplane::State & state);
	float operator()(const Airplane::Env & env) const;
private:
	const Airplane::Desc & desc_;
	Airplane::State state_;
	mutable Airplane::Cache cache_;
};

typedef TestAirplane TestMaxNx;

struct FuncMaxNx : std::unary_function<Airplane::Env, float>
{
	FuncMaxNx(const Airplane::Desc & desc, const Airplane::State & state);
	float operator()(const Airplane::Env & env) const;
private:
	const Airplane::Desc & desc_;
	Airplane::State state_;
	mutable Airplane::Cache cache_;
};

struct TestTAS
{
	Airplane::Desc desc;
	IterationRange<float> alt;
	Airplane::State state;
};

template<class Func>
struct FuncAlt : std::unary_function<float, typename Func::result_type>
{
	FuncAlt(const Func & func) : func_(func), level_(0) {;}
	inline typename Func::result_type operator()(float alt) const
	{
		const float altKm = alt * 1.0E-3;
		level_ = Satm1976::updateLevelByAltKm(level_, altKm);
		Satm1976::AtmParam atmParam;
		Satm1976::getAtmParamByAltKm(altKm, level_, atmParam);
		Airplane::Env env;
		env.P = atmParam.Pr * Satm1976::P0;
		env.T = atmParam.Tr * Satm1976::T0;
		env.rho = atmParam.rhor * Satm1976::rho0;
		return func_(env);
	}
private:
	Func func_;
	mutable unsigned int level_;
};

typedef TestTAS TestTASMin;

struct TasMin : public std::unary_function<Airplane::Env, float>
{
	TasMin(const Airplane::Desc & desc, const Airplane::State & state);
	float operator()(const Airplane::Env & env) const;
private:
	const Airplane::Desc & desc_;
	Airplane::State state_;
	mutable Airplane::Cache cache_;
};

typedef TestTAS TestTASMax;

struct TasMax : public std::unary_function<Airplane::Env, float>
{
	TasMax(const Airplane::Desc & desc, const Airplane::State & state);
	float operator()(const Airplane::Env & env) const;
private:
	const Airplane::Desc & desc_;
	Airplane::State state_;
	mutable Airplane::Cache cache_;
};

typedef TestTAS TestTASEndurance;

struct FuncEnduranceSpeed : std::unary_function<Airplane::Env, float>
{
	FuncEnduranceSpeed(const Airplane::Desc & desc, const Airplane::State & state);
	float operator()(const Airplane::Env & env) const;
private:
	const Airplane::Desc & desc_;
	Airplane::State state_;
	mutable Airplane::Cache cache_;
};

typedef TestTAS TestTASCruise;

struct FuncCruiseSpeed : std::unary_function<Airplane::Env, float>
{
	FuncCruiseSpeed(const Airplane::Desc & desc, const Airplane::State & state);
	float operator()(const Airplane::Env & env) const;
private:
	const Airplane::Desc & desc_;
	Airplane::State state_;
	mutable Airplane::Cache cache_;
};

typedef TestTAS TestMaxVyTAS;

struct FuncMaxVyTAS : std::unary_function<Airplane::Env, float>
{
	FuncMaxVyTAS(const Airplane::Desc & desc, const Airplane::State & state);
	float operator()(const Airplane::Env & env) const;
private:
	const Airplane::Desc & desc_;
	Airplane::State state_;
	mutable Airplane::Cache cache_;
};

#endif _TestAirplane_hpp_