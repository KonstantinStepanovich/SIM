#include "stdafx.h"

#include "TestAirframe.hpp"
#include "Modules/Satm1976/Satm1976.hpp"

FuncLD::FuncLD(const AirframeForce::Desc & desc) : desc_(desc), tableIndex_(0)
{
}

float FuncLD::operator()(float M, float AoA) const
{
	AirframeForce::Desc::Param param;
	getAerParam(desc_, M, tableIndex_, param);
	AirframeForce::AerCoeffs aerCoeffPair =  getAerCoeffs(desc_, param, AoA);
	return aerCoeffPair.cy / aerCoeffPair.cx;
}

FuncAoAOptimal::FuncAoAOptimal(const AirframeForce::Desc & desc) : desc_(desc)
{
}

float FuncAoAOptimal::operator()(float M) const
{
	Math::TableIndex tableIndex = 0;
	AirframeForce::Desc::Param param;
	getAerParam(desc_, M, tableIndex, param);
	return getOptimalAoA(param, desc_.Cy0);
}

FuncMaxLD::FuncMaxLD(const AirframeForce::Desc & desc) : desc_(desc)
{
}

float FuncMaxLD::operator()(float M) const
{
	Math::TableIndex tableIndex = 0;
	AirframeForce::Desc::Param param;
	getAerParam(desc_, M, tableIndex, param);
	return getMaxLD(param, desc_.Cy0);
}

FuncMxAoS::FuncMxAoS(const AirframeTorque::DescMxMy & desc, const AirframeTorque::State & state, double Cy) : 
	desc_(desc), state_(state), Cy_(Cy)
{
}

float FuncMxAoS::operator()(float AoS) const
{
	double Mx, My;
	calcMxMyAoS(desc_, state_, AoS, Cy_, Mx, My);
	return Mx;
}

FuncOmegaX::FuncOmegaX(const AirframeTorque::Desc & desc, double AoA, double AoS, double Cy) :
	desc_(desc), alpha_(AoA), betta_(AoS), Cy_(Cy)
{
}

double FuncOmegaX::operator()(double TAS) const
{
	AirframeTorque::Env env;
	env.AoA = alpha_;
	env.AoS = betta_;
	env.flagTAS = false;
	env.TAS = Math::Vec3(TAS, 0.0, 0.0);
	env.Cy = Cy_;
	env.rho = Satm1976::rho0;
	return getOmegaX(desc_, env);
}

FuncMyAoS::FuncMyAoS(const AirframeTorque::DescMxMy & desc, const AirframeTorque::State & state, double Cy) :
	desc_(desc), state_(state), Cy_(Cy)
{
}

float FuncMyAoS::operator()(float AoS) const
{
	double Mx, My;
	calcMxMyAoS(desc_, state_, AoS, Cy_, Mx, My);
	return My;
}

FuncMzAoA::FuncMzAoA(const AirframeTorque::DescMz & desc, const AirframeTorque::State & state) :
	desc_(desc), state_(state)
{
}

float FuncMzAoA::operator()(float AoA) const
{
	return calcMzAoA(desc_, state_, AoA);
}