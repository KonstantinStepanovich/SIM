#ifndef _TestAirframe_hpp_
#define _TestAirframe_hpp_

#include "Iteration.hpp"

#include "Database/Entity.hpp"
#include "Utils/Units.hpp"

#include "Modules/SFM/Model/Airframe.hpp"

using namespace Sim;
using namespace Sim::SFM;

struct TestAirframe
{
	AirframeForce::Desc desc;

	IterationRange<float> M;
	IterationRange<float> AoA;
};

typedef TestAirframe TestLD;

struct FuncLD : std::binary_function<float, float, float>
{
	FuncLD(const AirframeForce::Desc & desc);
	float operator()(float M, float AoA) const;
private:
	const AirframeForce::Desc & desc_;
	mutable Math::TableIndex tableIndex_;
};

struct TestAoAOptimal
{
	AirframeForce::Desc desc;
	IterationRange<float> M;
};

struct FuncAoAOptimal : std::unary_function<float, float>
{
	FuncAoAOptimal(const AirframeForce::Desc & desc);
	float operator()(float M) const;
private:
	const AirframeForce::Desc & desc_;
};

typedef TestAoAOptimal TestMaxLD;

struct FuncMaxLD : std::unary_function<float, float>
{
	FuncMaxLD(const AirframeForce::Desc & desc);
	float operator()(float M) const;
private:
	const AirframeForce::Desc & desc_;
};

struct TestGlideM
{
	AirframeForce::Desc desc;
};

struct FuncMxAoS : std::unary_function<float, float>
{
	FuncMxAoS(const AirframeTorque::DescMxMy & desc, const AirframeTorque::State & state, double Cy);
	float operator()(float AoS) const;
private:
	const AirframeTorque::DescMxMy & desc_;
	AirframeTorque::State state_;
	double Cy_;
};

struct FuncOmegaX : std::unary_function<double, double>
{
	FuncOmegaX(const AirframeTorque::Desc & desc, double AoA, double AoS, double Cy);
	double operator()(double TAS) const;
private:
	const AirframeTorque::Desc & desc_;
	double alpha_;
	double betta_;
	double Cy_;
};

struct FuncMyAoS : std::unary_function<float, float>
{
	FuncMyAoS(const AirframeTorque::DescMxMy & desc, const AirframeTorque::State & state, double Cy);
	float operator()(float AoS) const;
private:
	const AirframeTorque::DescMxMy & desc_;
	AirframeTorque::State state_;
	double Cy_;
};

struct FuncMzAoA : std::unary_function<float, float>
{
	FuncMzAoA(const AirframeTorque::DescMz & desc, const AirframeTorque::State & state);
	float operator()(float AoA) const;
private:
	const AirframeTorque::DescMz & desc_;
	AirframeTorque::State state_;
};


#endif _TestAirframe_hpp_