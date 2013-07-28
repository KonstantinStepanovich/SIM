#include "Modules/SFM/Model/Airframe.hpp"

#include "Database/Variant.hpp"
#include "Database/VariantUtil.hpp"

void Sim::SFM::Airframe::load(const Variant & variant, Desc & desc)
{
	load(variant.key("force"), desc.force);
	load(variant.key("torque"), desc.torque);
}

void Sim::SFM::Airframe::save(Variant & variant, const Desc & desc)
{
	save(variant["force"], desc.force);
	save(variant["torque"], desc.torque);
}

static const Sim::SFM::Airframe::State Sim::SFM::Airframe::defaultState = {
	{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
	{
		0.0,
		0.0,
		0.0,
		Sim::SFM::AirframeTorque::State::FLAG_NOTHING,
		0.0f,
		1.0f,
		1.0f,
	}
};

static const Sim::SFM::Airframe::Cache Sim::SFM::Airframe::defaultCache = {{0}};

void Sim::SFM::Airframe::calcForceTorque(const Descriptor & desc, const State & state, const Env & env, Cache & cache, Math::Vec3 & force, Math::Vec3 & torque)
{
	const AirframeForce::AerCoeffs aerCoeffs = AirframeForce::calcAerCoeffs(desc.force, state.force, env.force, cache.force);
	force = AirframeForce::calcForceByAerCoeffs(desc.force, env.force, aerCoeffs);

	AirframeTorque::Env envTorque = env.torque;
	envTorque.Cy = aerCoeffs.cy;
	torque = AirframeTorque::calcTorque(desc.torque, state.torque, envTorque);
}

void Sim::SFM::Airframe::addPayload(State & state, const PayloadTree & payloadTree, PayloadTree::const_iterator where)
{
	addPayload(state.force, payloadTree, where);	
}

void Sim::SFM::Airframe::removePayload(State & state, const PayloadTree & payloadTree, PayloadTree::const_iterator where)
{
	removePayload(state.force, payloadTree, where);
}