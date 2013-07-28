//#include "stdafx.h"

#include "Modules/SFM/Model/Airplane.hpp"
#include "Modules/SFM/Model/JetEngine.hpp"

#include "Database/Variant.hpp"
#include "Utils/VariantMathUtil.hpp"

#include "Modules/Satm1976/Satm1976.hpp"
#include "Math/Scalar.hpp"
#include "Math/Equation.hpp"
#include "Physic/Gas.hpp"

using namespace Sim;
using namespace Sim::SFM;
using namespace Sim::SFM::Airplane;

void Sim::SFM::Airplane::load(const Variant & variant, Hardpoint & hardpoint)
{
	loadFromTable(variant, "pos", hardpoint.pos);
}

void Sim::SFM::Airplane::save(Variant & variant, const Hardpoint & hardpoint)
{
	saveToTable(variant, "pos", hardpoint.pos);
}

void Sim::SFM::Airplane::load(const Variant & variant, HardPoints & hardPoints)
{
	hardPoints.resize(variant.size());
	for(size_t i = 0; i < hardPoints.size(); i++)
		loadFromTable(variant, i, hardPoints[i]);
}

void Sim::SFM::Airplane::save(Variant & variant, const HardPoints & hardPoints)
{
	for(size_t i = 0; i < hardPoints.size(); i++)
		saveToTable(variant[i], i, hardPoints[i]);
}

void Sim::SFM::Airplane::load(const Variant & variant, Desc & desc)
{
	desc.airframeId = variant.key("airframeId").getString();
	desc.jetEngineId = variant.key("jetEngineId").getString();
	desc.engineCount = variant.key("engineCount");
	desc.mass = variant.key("mass");
	loadFromTable(variant, "centerOfMass", desc.centerOfMass);
	loadFromTable(variant, "relInertiaTensor", desc.relInertiaTensor);
	load(variant.key("fuelSystem"), desc.fuelSystem);
}

void Sim::SFM::Airplane::save(Variant & variant, const Desc & desc)
{
	variant["airframeId"] = desc.airframeId;
	variant["jetEngineId"] = desc.jetEngineId;
	variant["engineCount"] = desc.engineCount;
	variant["mass"] = desc.mass;
	saveToTable(variant, "centerOfMass", desc.centerOfMass);
	saveToTable(variant, "relInertiaTensor", desc.relInertiaTensor);
	save(variant["fuelSystem"], desc.fuelSystem);
}

void Sim::SFM::Airplane::load(const Variant & variant, PayloadMassDesc & desc)
{
	desc.solidMass = variant.tryKey("solidMass") ? variant.key("solidMass") : false;
	if(!desc.solidMass)
		desc.mass = variant.key("mass");
	desc.fuel = variant.tryKey("fuel") ? variant.key("fuel") : false;
	tryLoadFromTable(variant, "hardPoints", desc.hardPoints);
}

void Sim::SFM::Airplane::save(Variant & variant, const PayloadMassDesc & desc)
{
	variant["solidMass"] = desc.solidMass;
	if(!desc.solidMass)
		variant["mass"] = desc.mass;
	variant["fuel"] = desc.fuel;
	saveToTable(variant, "hardPoints", desc.hardPoints);
}

namespace Sim { namespace SFM { namespace Airplane {
Math::Vec3 getNodePos(const PayloadTree & payloadTree, PayloadTree::const_iterator where)
{
	PayloadTree::const_iterator it = where;
	Math::Vec3 pos(0.0, 0.0, 0.0);
	if(it == payloadTree.root())
		return pos;
	unsigned int index = it->index;
	do
	{
		it.down();
		if(index != Payload::INDEX_NULL)
		{
			const HardPoints & hardPoints = PayloadMassDatabase::instance().getDesc(it->typeId).hardPoints;
			const Hardpoint & hardpoint = hardPoints[index];
			pos += Math::Vec3(hardpoint.pos[0], hardpoint.pos[1], hardpoint.pos[2]);
		}
		index = it->index;
	}
	while(it != payloadTree.root());
	return pos;
}

static const State defaultState = {
	0.0f,
	Math::Vec3(0.0, 0.0, 0.0),
	Math::Matrix3x3(),
	Airframe::defaultState,
	JetEngine::defaultState,
	fuelSystemDefaultState
};

static const Cache defaultCache = {
	0,
	Airframe::defaultCache,
	JetEngine::defaultCache
};

} } }

void Sim::SFM::Airplane::addPayload(State & state, const PayloadTree & payloadTree, PayloadTree::const_iterator where)
{
	Math::Vec3 pos = getNodePos(payloadTree, where);
	const PayloadMassDesc & desc = PayloadMassDatabase::instance().getDesc(where->typeId);
	const Real mass = desc.solidMass ? where->ammount.mass :  where->ammount.count * desc.mass;
	const Math::Vec3 payloadInertiaMoments = mass * Math::Vec3(	pos.cross(Math::Vec3(1.0, 0.0, 0.0)).length(),
																pos.cross(Math::Vec3(0.0, 1.0, 0.0)).length(),
																pos.cross(Math::Vec3(0.0, 0.0, 1.0)).length());
	for(int i = 0; i < 3; i++)
		state.payloadInertiaTensor[i][i] += payloadInertiaMoments[i];
	if(desc.fuel)
		state.fuelSystem.externalFuel += mass;
	addPayload(state.airframe, payloadTree, where);
}

void Sim::SFM::Airplane::removePayload(State & state, const PayloadTree & payloadTree, PayloadTree::const_iterator where)
{
	Math::Vec3 pos = getNodePos(payloadTree, where);
	const PayloadMassDesc & desc = PayloadMassDatabase::instance().getDesc(where->typeId);
	const float mass = desc.solidMass ? where->ammount.mass :  where->ammount.count * desc.mass;
	const Math::Vec3 payloadInertiaMoments = mass * Math::Vec3(pos.x() * pos.x(), pos.y() * pos.y(), pos.z() * pos.z());
	for(int i = 0; i < 3; i++)
		state.payloadInertiaTensor[i][i] -= payloadInertiaMoments[i];
	if(desc.fuel)
		state.fuelSystem.externalFuel -= mass;
	removePayload(state.airframe, payloadTree, where);
}

static const float Sim::SFM::Airplane::g = 9.81f;

inline float getQ(const Env & env)
{
	return 0.5f * env.rho * env.TAS * env.TAS;
}

void getSatmParam(float P, unsigned int & level, float & altKm, Satm1976::AtmParam & atmParam)
{
	float Pr = P / Satm1976::P0;
	level = Satm1976::updateLevelByPr(level, Pr);
	altKm = Satm1976::getAltKmByPr(level, Pr);
	Satm1976::getAtmParamByAltKm(altKm, level, atmParam);
}

float Sim::SFM::Airplane::getInstTurnNyMax(	const Desc & desc, const Env & env, const State & state,
											Cache & cache)
{
	float altKm;
	Satm1976::AtmParam atmParam;
	getSatmParam(env.P, cache.level, altKm, atmParam);

	JetEngine::Environment jetEnv;
	jetEnv.altKm = altKm;
	jetEnv.M = env.M;
	jetEnv.dT = env.T - atmParam.Tr * Satm1976::T0;
	float Pmax = desc.engineCount * getThrustMax(JetEngine::Database::instance().getDesc(desc.jetEngineId), jetEnv, state.jetEngine, cache.jetEngine);

	const AirframeForce::Desc & airframeDesc = Airframe::Database::instance().getDesc(desc.airframeId).force;

	cache.airframe.force.tableIndex = airframeDesc.table.findIndex(env.M, cache.airframe.force.tableIndex);
	AirframeForce::Desc::Param param;
	getAerParam(airframeDesc, env.M, cache.airframe.force.tableIndex, param);

	float q = getQ(env);
	float S = airframeDesc.S;

	cache.airframe.force.tableIndex = airframeDesc.table.findIndex(env.M, cache.airframe.force.tableIndex);
	float CyMax = airframeDesc.table.eval(env.M, cache.airframe.force.tableIndex, Math::makeStructMember(&AirframeForce::Desc::Param::CyMax));

	float mass = getMass(desc, state);

	struct FuncTangForce : public std::unary_function<float, float>
	{
		FuncTangForce(	const AirframeForce::Desc::Param & param, float P,
						float mass, float q, float S) : param1_(param), P_(P), mass_(mass), q_(q), S_(S) {;}
		float operator()(float AoA) const
		{
			return	P_ * cos(AoA) -
					(param1_.cx0 + pow(param1_.dCydAoA * AoA, 2) * param1_.bPol2 + pow(param1_.dCydAoA * AoA, 4) * param1_.bPol4) * q_ * S_;
		}
	private:
		const AirframeForce::Desc::Param & param1_;
		float P_;
		float mass_;
		float q_;
		float S_;
	};
	float alphaMax = CyMax / param.dCydAoA;
	FuncTangForce funcTangForce(param, Pmax, mass, q, S);
 	if(funcTangForce(0.0f) < 0.0f)
 		return Math::invalid<float>();
	float AoA = Math::evalSecant(0.0f, alphaMax, 1.0E-4f, funcTangForce);
	const float FT = funcTangForce(AoA);
	AoA = std::min(AoA, alphaMax);

	const float Ny = ((airframeDesc.Cy0 + AoA * param.dCydAoA) * q * S) / (mass * g - Pmax * sin(AoA)) ;
	return Ny;
}

float Sim::SFM::Airplane::getTASbyNy(const Desc & desc, const Env & env, const State & state, float Ny,
									 Cache & cache)
{
	const AirframeForce::Desc & airframeDesc = Airframe::Database::instance().getDesc(desc.airframeId).force;
	float mass = getMass(desc, state);
	return AirframeForce::getTASbyLift(	airframeDesc,
										env.rho,
										getSoundVel(env.T),
										Ny * mass * g,
										cache.airframe.force.tableIndex);
}

float getNxMax(const Desc & desc, const Env & env, const State & state,
			   float mass, float altKm, float T, const AirframeForce::Desc::Param & param,
			   Cache & cache)
{
	const AirframeForce::Desc & airframeDesc = Airframe::Database::instance().getDesc(desc.airframeId).force;
	float q = getQ(env);
	cache.airframe.force.tableIndex = airframeDesc.table.findIndex(env.M, 0);
	float dCydAoA = airframeDesc.table.eval(env.M, cache.airframe.force.tableIndex, Math::makeStructMember(&AirframeForce::Desc::Param::dCydAoA));
	float CyMax = airframeDesc.table.eval(env.M, cache.airframe.force.tableIndex, Math::makeStructMember(&AirframeForce::Desc::Param::CyMax));
	float Cy = mass * g / (q * airframeDesc.S);
	Cy = std::min(Cy, CyMax);
	float dCy = Cy - airframeDesc.Cy0;
	float cx = param.cx0 + pow(dCy, 2) * param.bPol2 + pow(dCy, 4) * param.bPol4;
	float AoA = dCy / dCydAoA;

	const JetEngine::Desc & jetEngineDesc = JetEngine::Database::instance().getDesc(desc.jetEngineId);
	JetEngine::Env jetEngineEnv;
	jetEngineEnv.altKm = altKm;
	jetEngineEnv.M = env.M;
	jetEngineEnv.dT = env.T - T;
	float Pmax = desc.engineCount * JetEngine::getThrustMax(jetEngineDesc, jetEngineEnv, state.jetEngine, cache.jetEngine);
	float Nx = (Pmax * cos(AoA) - cx * q * airframeDesc.S) / mass;
	return Nx;
}

inline float getVyMax(const Desc & desc, const Env & env, const State & state,
					  float mass, float altKm, float T, const AirframeForce::Desc::Param & param,
					  Cache & cache)
{
	float Nx = ::getNxMax(	desc, env, state, 
							mass, altKm, T, param,
							cache);
	return Nx * env.TAS / g;
}

float Sim::SFM::Airplane::getTASMax(const Desc & desc, const Env & env, const State & state,
									Cache & cache)
{
	const AirframeForce::Desc & airframeDesc = Airframe::Database::instance().getDesc(desc.airframeId).force;
	float mass = getMass(desc, state);

	float altKm;
	Satm1976::AtmParam atmParam;
	getSatmParam(env.P, cache.level, altKm, atmParam);
	float T = atmParam.Tr * Satm1976::T0;
	float soundVel = getSoundVel(env.T);

	Env curEnv = env;

	const AirframeForce::Desc::Table::Element & curr = airframeDesc.table.getElement(cache.airframe.force.tableIndex);
	float currVa = curr.arg * soundVel;
	curEnv.M = curr.arg;
	curEnv.TAS = currVa;
	float currNx = ::getNxMax(	desc, curEnv, state, 
								mass, altKm, T, curr.val,
								cache);
	if(currNx < 0.0f)
		return Math::invalid<float>();

	char dir = 0;
	while(true)
	{
		const AirframeForce::Desc::Table::Element & next = airframeDesc.table.getElement(cache.airframe.force.tableIndex + 1);
		float nextVa = next.arg * soundVel;
		curEnv.M = next.arg;
		curEnv.TAS = nextVa;
		float nextNx = ::getNxMax(	desc, curEnv, state,
									mass, altKm, T, next.val,
									cache);

		if(currNx > 0.0f && nextNx < 0.0f)
		{
			const float k = (nextNx - currNx) / (nextVa - currVa);
			return -(currNx - k * currVa) / k;
		}
		else if(nextNx > 0.0f)
		{
			if(	dir == -1 ||
				cache.airframe.force.tableIndex == airframeDesc.table.size() - 2)
				return nextVa;
			else
			{
				cache.airframe.force.tableIndex++;
				dir = 1;
			}
		}
		else if(currNx < 0.0f)
		{
			if(	dir == 1 ||
				cache.airframe.force.tableIndex == 0)
				return currVa;
			else
			{
				cache.airframe.force.tableIndex--;
				dir = -1;
			}
		}
		currNx = nextNx;
		currVa = nextVa;
	}
}

float Sim::SFM::Airplane::getNyMax(	const Desc & desc, const Env & env, const State & state,
									Cache & cache)
{
	const AirframeForce::Desc & airframeDesc = Airframe::Database::instance().getDesc(desc.airframeId).force;
	cache.airframe.force.tableIndex = airframeDesc.table.findIndex(env.M, cache.airframe.force.tableIndex);
	float CyMax = airframeDesc.table.eval(env.M, cache.airframe.force.tableIndex, Math::makeStructMember(&AirframeForce::Desc::Param::CyMax));
	float q = getQ(env);
	float mass = getMass(desc, state);
	return CyMax * q * airframeDesc.S / (mass * g);
}

float Sim::SFM::Airplane::getNxMax(	const Desc & desc, const Env & env, const State & state,
									Cache & cache)
{
	float mass = getMass(desc, state);

	float altKm;
	Satm1976::AtmParam atmParam;
	getSatmParam(env.P, cache.level, altKm, atmParam);
	float T = atmParam.Tr * Satm1976::T0;

	const AirframeForce::Desc & airframeDesc = Airframe::Database::instance().getDesc(desc.airframeId).force;
	AirframeForce::Desc::Param param;
	getAerParam(airframeDesc, env.M, cache.airframe.force.tableIndex, param);

	return ::getNxMax(	desc, env, state,
						mass, altKm, T, param,
						cache);
}

float Sim::SFM::Airplane::getVyMax(	const Desc & desc, const Env & env, const State & state,
									Cache & cache)
{
	float mass = getMass(desc, state);

	float altKm;
	Satm1976::AtmParam atmParam;
	getSatmParam(env.P, cache.level, altKm, atmParam);
	float T = atmParam.Tr * Satm1976::T0;

	const AirframeForce::Desc & airframeDesc = Airframe::Database::instance().getDesc(desc.airframeId).force;
	AirframeForce::Desc::Param param;
	getAerParam(airframeDesc, env.M, cache.airframe.force.tableIndex, param);

	return ::getVyMax(	desc, env, state,
						mass, altKm, T, param,
						cache);
}

static LevelFlightParam getLevelFlightParam(const Desc & desc, const Env & env, const State & state,
											float mass, float altKm, float T, const AirframeForce::Desc::Param & param,
											JetEngine::Cache & cache)
{
	const AirframeForce::Desc & airframeDesc = Airframe::Database::instance().getDesc(desc.airframeId).force;

	float q = getQ(env);
	float S = airframeDesc.S;

	struct FuncThrust: public std::unary_function<float, float>
	{
		FuncThrust(	const AirframeForce::Desc::Param & param, float Cy0,
			float mass, float qIn, float Sin) : param1_(param), Cy0_(Cy0), mass_(mass), q(qIn), S(Sin) {;}
		float operator()(float AoA) const
		{
			return	(Cy0_ + param1_.dCydAoA * AoA) * q * S - mass_ * g +
					(param1_.cx0 + pow(param1_.dCydAoA * AoA, 2) * param1_.bPol2 + pow(param1_.dCydAoA * AoA, 4) * param1_.bPol4) * q * S * tan(AoA);
		}
	private:
		const AirframeForce::Desc::Param & param1_;
		float Cy0_;
		float mass_;
		float q;
		float S;
	};
	float alphaMax = param.CyMax / param.dCydAoA;
	FuncThrust funcThrust(param, airframeDesc.Cy0, mass, q, S);
	float AoA = Math::evalSecant(0.0f, alphaMax, 1.0E-4f, funcThrust);
	if(AoA > 0.0f && AoA < alphaMax)
	{
		float P = (mass * g - (airframeDesc.Cy0 + param.dCydAoA * AoA) * q * S) / sin(AoA);
		//float P2 = (param.cx0 + pow(param.dCydAoA * AoA, 2) * param.bPol) * q * S / cos(AoA);
		//assert(fabs(P - P2) < 0.02f * P);
		const JetEngine::Desc & jetEngineDesc = JetEngine::Database::instance().getDesc(desc.jetEngineId);
		JetEngine::Environment jetEngineEnv;
		jetEngineEnv.altKm = altKm;
		jetEngineEnv.M = env.M;
		jetEngineEnv.dT = env.T - T;
		JetEngine::State jetEngineState;
		jetEngineState.afterburner = false;
		float Pmax = desc.engineCount * JetEngine::getThrustMax(jetEngineDesc, jetEngineEnv, jetEngineState, cache);
		if(state.jetEngine.afterburner && jetEngineDesc.hasAfterburner)
		{
			jetEngineState.afterburner = true;
			float Paft = desc.engineCount * JetEngine::getThrustMax(jetEngineDesc, jetEngineEnv, jetEngineState, cache);
			float PmaxAbs = std::max(Pmax, Paft);
			if(P < PmaxAbs)
				return LevelFlightParam(AoA, Thrust(P > Pmax, P, P / PmaxAbs));
		}
		else
		{
			if(P < Pmax)
				return LevelFlightParam(AoA, Thrust(false, P, P / Pmax));
		}
	}
	return LevelFlightParam();
}

LevelFlightParam Sim::SFM::Airplane::getLevelFlightParam(	const Desc & desc, const Env & env, const State & state,
															Cache & cache)
{
	float mass = getMass(desc, state);

	float altKm;
	Satm1976::AtmParam atmParam;
	getSatmParam(env.P, cache.level, altKm, atmParam);
	float T = atmParam.Tr * Satm1976::T0;

	const AirframeForce::Desc & airframeDesc = Airframe::Database::instance().getDesc(desc.airframeId).force;
	AirframeForce::Desc::Param param;
	getAerParam(airframeDesc, env.M, cache.airframe.force.tableIndex, param);

	return ::getLevelFlightParam(	desc, env, state,
									mass, altKm, T, param,
									cache.jetEngine);
}

Thrust Sim::SFM::Airplane::getThrust(	const Desc & desc, const Env & env, const State & state,
										Cache & cache)
{
	float mass = getMass(desc, state);

	float altKm;
	Satm1976::AtmParam atmParam;
	getSatmParam(env.P, cache.level, altKm, atmParam);
	float T = atmParam.Tr * Satm1976::T0;

	const AirframeForce::Desc & airframeDesc = Airframe::Database::instance().getDesc(desc.airframeId).force;
	AirframeForce::Desc::Param param;
	getAerParam(airframeDesc, env.M, cache.airframe.force.tableIndex, param);

	return ::getLevelFlightParam(	desc, env, state,
									mass, altKm, T, param,
									cache.jetEngine).thrust;
}

float getConsumption( const Desc & desc, const Env & env, const State & state,
					   float mass, float altKm, float T, const AirframeForce::Desc::Param & param,
					   JetEngine::Cache & cache)
{
	Thrust thrust = getLevelFlightParam(desc, env, state,
										mass, altKm, T, param,
										cache).thrust;
	if(thrust.abs != Math::invalid<float>())
	{
		const JetEngine::Desc & jetEngineDesc = JetEngine::Database::instance().getDesc(desc.jetEngineId);
		JetEngine::Environment jetEngineEnv;
		jetEngineEnv.altKm = altKm;
		jetEngineEnv.M = env.M;
		jetEngineEnv.dT = env.T - T;
		JetEngine::State jetEngineState;
		jetEngineState.afterburner = thrust.afterburner;
		float Ce = thrust.abs * getCe(jetEngineDesc, thrust.rel, jetEngineEnv, jetEngineState, cache);
		return Ce;
	}
	else
		return Math::invalid<float>();
}

float Sim::SFM::Airplane::getConsumption(	const Desc & desc, const Env & env, const State & state,
											Cache & cache)
{
	float mass = getMass(desc, state);

	float altKm;
	Satm1976::AtmParam atmParam;
	getSatmParam(env.P, cache.level, altKm, atmParam);
	float T = atmParam.Tr * Satm1976::T0;

	const AirframeForce::Desc & airframeDesc = Airframe::Database::instance().getDesc(desc.airframeId).force;
	AirframeForce::Desc::Param param;
	getAerParam(airframeDesc, env.M, cache.airframe.force.tableIndex, param);

	return ::getConsumption(desc, env, state,
							mass, altKm, T, param,
							cache.jetEngine);
}

float Sim::SFM::Airplane::getTASEndurance(	const Desc & desc, const Env & env, const State & state,
											Cache & cache)
{
	const AirframeForce::Desc & airframeDesc = Airframe::Database::instance().getDesc(desc.airframeId).force;
	float mass = getMass(desc, state);

	float altKm;
	Satm1976::AtmParam atmParam;
	getSatmParam(env.P, cache.level, altKm, atmParam);
	float T = atmParam.Tr * Satm1976::T0;
	float soundVel = getSoundVel(env.T);

	Env curEnv = env;

	const AirframeForce::Desc::Table::Element & curr = airframeDesc.table.getElement(cache.airframe.force.tableIndex);
	float currVa = curr.arg * soundVel;
	curEnv.M = curr.arg;
	curEnv.TAS = currVa;
	float currConsumption = ::getConsumption(	desc, curEnv, state,
												mass, altKm, T, curr.val,
												cache.jetEngine);

	char dir = 0;
	while(true)
	{
		const AirframeForce::Desc::Table::Element & next = airframeDesc.table.getElement(cache.airframe.force.tableIndex + 1);
		float nextVa = next.arg * soundVel;
		curEnv.M = next.arg;
		curEnv.TAS = nextVa;
		float nextConsumption = ::getConsumption(	desc, curEnv, state,
													mass, altKm, T, next.val,
													cache.jetEngine);
		if(	!currConsumption ||
			nextConsumption < currConsumption)
		{
			if(	dir == -1 ||
				cache.airframe.force.tableIndex == airframeDesc.table.size() - 2)
				return nextVa;
			else
			{
				cache.airframe.force.tableIndex++;
				dir = 1;
			}
		}
		else if(!nextConsumption ||
				nextConsumption > currConsumption)
		{
			if(	dir == 1 ||
				cache.airframe.force.tableIndex == 0)
				return currVa;
			else
			{
				cache.airframe.force.tableIndex--;
				dir = -1;
			}
		}
		currConsumption = nextConsumption;
		currVa = nextVa;
	}
}

inline float getWayConsumption(	const Desc & desc, const Env & env, const State & state,
								float mass, float altKm, float T, const AirframeForce::Desc::Param & param,
								JetEngine::Cache & cache)
{
	float consumption = getConsumption(	desc, env, state,
										mass, altKm, T, param,
										cache);
	return consumption != Math::invalid<float>() ? consumption / env.TAS : Math::invalid<float>();
}

float Sim::SFM::Airplane::getTASCruise(	const Desc & desc, const Env & env, const State & state,
										Cache & cache)
{
	const AirframeForce::Desc & airframeDesc = Airframe::Database::instance().getDesc(desc.airframeId).force;
	float mass = getMass(desc, state);

	float altKm;
	Satm1976::AtmParam atmParam;
	getSatmParam(env.P, cache.level, altKm, atmParam);
	float T = atmParam.Tr * Satm1976::T0;
	float soundVel = getSoundVel(env.T);

	Env curEnv = env;

	const AirframeForce::Desc::Table::Element & curr = airframeDesc.table.getElement(cache.airframe.force.tableIndex);
	float currVa = curr.arg * soundVel;
	curEnv.M = curr.arg;
	curEnv.TAS = currVa;
	float currWayConsumption = ::getWayConsumption(desc, curEnv, state,
													mass, altKm, T, curr.val,
													cache.jetEngine);

	char dir = 0;
	while(true)
	{
		const AirframeForce::Desc::Table::Element & next = airframeDesc.table.getElement(cache.airframe.force.tableIndex + 1);
		float nextVa = next.arg * soundVel;
		curEnv.M = next.arg;
		curEnv.TAS = nextVa;
		float nextWayConsumption = ::getWayConsumption(desc, curEnv, state,
														mass, altKm, T, next.val,
														cache.jetEngine);
		if(	!currWayConsumption ||
			nextWayConsumption < currWayConsumption)
		{
			if(	dir == -1 ||
				cache.airframe.force.tableIndex == airframeDesc.table.size() - 2)
				return nextVa;
			else
			{
				cache.airframe.force.tableIndex++;
				dir = 1;
			}
		}
		else if(!nextWayConsumption ||
				nextWayConsumption > currWayConsumption)
		{
			if(	dir == 1 ||
				cache.airframe.force.tableIndex == 0)
				return currVa;
			else
			{
				cache.airframe.force.tableIndex--;
				dir = -1;
			}
		}
		currWayConsumption = nextWayConsumption;
		currVa = nextVa;
	}
}

float Sim::SFM::Airplane::getMaxVyTAS(	const Desc & desc, const Env & env, const State & state, float minM, float maxM,
										Cache & cache)
{
	const AirframeForce::Desc & airframeDesc = Airframe::Database::instance().getDesc(desc.airframeId).force;
	float mass = getMass(desc, state);

	float altKm;
	Satm1976::AtmParam atmParam;
	getSatmParam(env.P, cache.level, altKm, atmParam);
	float T = atmParam.Tr * Satm1976::T0;
	float soundVel = getSoundVel(env.T);

	cache.airframe.force.tableIndex = 0;
	cache.jetEngine.thrustIndex = Math::Table2DIndex(0, 0);	

	Env curEnv = env;

	AirframeForce::Desc::Param param;
	getAerParam(airframeDesc, minM, cache.airframe.force.tableIndex, param);
	float maxVyVa = minM * soundVel;
	curEnv.M = minM;
	curEnv.TAS = maxVyVa;
	float maxVy = ::getVyMax(	desc, curEnv, state, 
								mass, altKm, T, param,
								cache);

	char dir = 0;
	while(true)
	{
		const AirframeForce::Desc::Table::Element & curr = airframeDesc.table.getElement(++cache.airframe.force.tableIndex);
		float TAS = curr.arg * soundVel;
		curEnv.M = curr.arg;
		curEnv.TAS = TAS;
		float Vy = ::getVyMax(	desc, curEnv, state,
								mass, altKm, T, curr.val,
								cache);

		if(Vy > maxVy)
		{
			maxVy = Vy;
			maxVyVa = TAS;
		}
		if(	curr.arg > maxM ||
			cache.airframe.force.tableIndex == airframeDesc.table.size() - 2)
		{
			return maxVyVa;
		}
	}
}