#ifndef _SFM_Airframe_hpp_
#define _SFM_Airframe_hpp_

#include "Modules/SFM/Model/AirframeForce.hpp"
#include "Modules/SFM/Model/AirframeTorque.hpp"

#include "Database/Entity.hpp"

namespace Sim
{

class Variant;

namespace SFM
{

namespace Airframe
{

struct Descriptor : public DescBase
{
	AirframeForce::Desc		force;
	AirframeTorque::Desc	torque;
};
typedef Descriptor Desc;

void load(const Variant & variant, Desc & desc);

void save(Variant & variant, const Desc & desc);

typedef Database<Desc> Database;

struct State
{
	AirframeForce::State	force;
	AirframeTorque::State	torque;
};

extern const State defaultState;

void addPayload(State & state, const PayloadTree & payloadTree, PayloadTree::const_iterator where);

void removePayload(State & state, const PayloadTree & payloadTree, PayloadTree::const_iterator where);

struct Environment
{
	AirframeForce::Environment force;
	AirframeTorque::Environment torque;
};
typedef Environment Env;

struct Cache
{
	AirframeForce::Cache force;
};

extern const Cache defaultCache;

void calcForceTorque(const Desc & desc, const State & state, const Env & env, Cache & cache, Math::Vec3 & force, Math::Vec3 & torque);

}

}

}

#endif _SFM_Airframe_hpp_