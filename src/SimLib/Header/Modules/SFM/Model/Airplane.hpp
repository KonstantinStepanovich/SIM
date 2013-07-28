#ifndef _SFM_Airplane_hpp_
#define _SFM_Airplane_hpp_

#include "Modules/SFM/Model/Airframe.hpp"
#include "Modules/SFM/Model/JetEngine.hpp"
#include "Modules/SFM/Object/FuelSystem.hpp"
#include "Modules/SFM/Payload.hpp"

#include "Math/Vec3.hpp"
#include "Math/Mat3x3.hpp"
#include "Math/Interpolation.hpp"

#include <vector>

#include "Database/Entity.hpp"

namespace Sim {

class Variant;

namespace SFM {

namespace Airplane {

extern const float g;

struct Hardpoint
{
	float pos[3];
};

void load(const Variant & variant, Hardpoint & hardpoint);

void save(Variant & variant, const Hardpoint & hardpoint);

typedef std::vector<Hardpoint> HardPoints;

void load(const Variant & variant, HardPoints & hardPoints);

void save(Variant & variant, const HardPoints & hardPoints);

struct Descriptor : public DescBase
{
	TypeId airframeId;
	TypeId jetEngineId;
	int engineCount;
	float mass;
	Math::Vec3 centerOfMass;
	Math::Matrix3x3 relInertiaTensor;
	FuelSystemDesc fuelSystem;
};
typedef Descriptor Desc;

void load(const Variant & variant, Desc & desc);

void save(Variant & variant, const Desc & desc);

typedef Database<Desc> Database;

struct PayloadMassDesc : public DescBase
{
	float mass; //payload mass
	bool solidMass; //payload is a solid mass, not an item (fuel, napalm e.t.c.)
	bool fuel; //is it a fuel?
	HardPoints hardPoints; //hardpoints (if it is a rack)
};

void load(const Variant & variant, PayloadMassDesc & desc);

void save(Variant & variant, const PayloadMassDesc & desc);

typedef Sim::Database<PayloadMassDesc> PayloadMassDatabase;

struct Environment
{
	float P; //static pressure, Pa
	float M; //Mach number
	float TAS; //true airspeed, m/s
	float rho; //air density, kg/m^3
	float T; //temperature, K
};
typedef Environment Env;

struct State
{
	float				payloadMass; //payload mass (including mass of fuel in external tanks), kg 
	Math::Vec3			payloadCenterOfMass; //center of mass of only payload
	Math::Matrix3x3		payloadInertiaTensor; //inertia tensor of only payload
	Airframe::State		airframe;
	JetEngine::State	jetEngine;
	FuelSystemState		fuelSystem;
};
extern const State defaultState;

struct Cache
{
	unsigned int		level;
	Airframe::Cache		airframe;
	JetEngine::Cache	jetEngine;
};
extern const Cache defaultCache;

void addPayload(State & state, const PayloadTree & payloadTree, PayloadTree::const_iterator where);

void removePayload(State & state, const PayloadTree & payloadTree, PayloadTree::const_iterator where);

inline float getMass(const Desc & desc, const State & state)
{
	return desc.mass + state.fuelSystem.internalFuel + state.payloadMass;
}

inline Math::Vec3 getCenterOfMass(const Desc & desc, const State & state)
{
	return 0.5 * (desc.centerOfMass * (desc.mass + state.fuelSystem.internalFuel) + state.payloadCenterOfMass * state.payloadCenterOfMass);
}

inline Math::Matrix3x3 getInertiaTensor(const Desc & desc, const State & state)
{
	float mass = getMass(desc, state);
	return Math::Matrix3x3(	mass * desc.relInertiaTensor[0][0] + state.payloadInertiaTensor[0][0], 0.0, 0.0,
							0.0, mass * desc.relInertiaTensor[1][1] + state.payloadInertiaTensor[1][1], 0.0,
							0.0, 0.0, mass * desc.relInertiaTensor[2][2] + state.payloadInertiaTensor[2][2]);	
}

inline float getFuelMass(const Desc & desc, const State & state)
{
	return getFuelMass(desc.fuelSystem, state.fuelSystem);
}

float getInstTurnNyMax(	const Desc & desc, const Env & env, const State & state,
						Cache & cache);

inline float getInstTurnGammaMax(float instTurnNyMax)
{
	return acos(1.0f / instTurnNyMax);
}

inline float getInstTurnRadiusMin(float TAS, float instTurnNyMax)
{
	const float cosGamma = 1.0f / instTurnNyMax;
	const float tanGamma = sqrt(1.0f - cosGamma * cosGamma) / cosGamma;
	return TAS * TAS / (g * tanGamma);
}

inline float getInstTurnRateMax(float TAS, float instTurnGammaMax)
{
	return g * tan(instTurnGammaMax) / TAS;
}

float getTASbyNy(	const Desc & desc, const Env & env, const State & state, float Ny,
					Cache & cache);

inline float getTASMin(	const Desc & desc, const Env & env, const State & state,
						Cache & cache)
{
	return getTASbyNy(desc, env, state, 1.0f, cache);
}

float getTASMax(const Desc & desc, const Env & env, const State & state, 
				Cache & cache);

float getNyMax(	const Desc & desc, const Env & env, const State & state,
				Cache & cache);

inline float getTurnRateMax(float TAS, float NyMax)
{
	return (NyMax - 1.0f) * g / TAS;
}

inline float getTurnRadiusMin(float TAS, float NyMax)
{
	return TAS * TAS / ((NyMax - 1.0f) * g);
}

float getNxMax(	const Desc & desc, const Env & env, const State & state, 
				Cache & cache);

float getVyMax(	const Desc & desc, const Env & env, const State & state, 
				Cache & cache);

struct Thrust
{
	Thrust() : afterburner(false), abs(0.f), rel(0.0f) {;}
	Thrust(bool afterburnerIn, float absIn, float relIn) : afterburner(afterburnerIn), abs(absIn), rel(relIn) {;}
	bool afterburner;
	float abs;
	float rel;
};

struct LevelFlightParam
{
	LevelFlightParam() : AoA(0.0f) {;}
	LevelFlightParam(float alphaIn, const Thrust & thrustIn) : AoA(alphaIn), thrust(thrustIn) {;}
	float	AoA;
	Thrust	thrust;
};

LevelFlightParam getLevelFlightParam(	const Desc & desc, const Env & env, const State & state, 
										Cache & cache);

Thrust getThrust(	const Desc & desc, const Env & env, const State & state, 
					Cache & cache);

float getConsumption(	const Desc & desc, const Env & env, const State & state, 
						Cache & cache);

float getTASEndurance(	const Desc & desc, const Env & env, const State & state, 
						Cache & cache);

float getTASCruise(	const Desc & desc, const Env & env, const State & state, 
					Cache & cache);

float getMaxVyTAS(	const Desc & desc, const Env & env, const State & state, float minM, float maxM,
					Cache & cache);

}

}

}

#endif _SFM_Airplane_hpp_