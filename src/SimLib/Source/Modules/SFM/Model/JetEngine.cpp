#include "Modules/SFM/Model/JetEngine.hpp"

#include "Math/Scalar.hpp"
#include "Database/Variant.hpp"
#include "Database/VariantUtil.hpp"
#include "Utils/VariantMathUtil.hpp"

using namespace Sim;
using namespace Sim::SFM::JetEngine;

namespace Sim { namespace SFM { namespace JetEngine {

inline void load(const Variant & variant, Desc::Thrust & thrust)
{
	thrust.maximal		= variant.index(0);
	thrust.afterburner	= variant.index(1, VariantRoot(Math::invalid<float>()));
}

inline void load(const Variant & variant, Desc::Cons & cons)
{
	cons.maximal		= variant.index(0);
	cons.afterburner	= variant.index(1, VariantRoot(Math::invalid<float>()));
}

inline void load(const Variant & variant, Desc::Param & element)
{
	load(variant.key("thrust"),	element.thrust);
	load(variant.key("cons"),	element.cons);
}

inline void save(Variant & variant, const Desc::Thrust & thrust)
{
	variant[0u] = thrust.maximal;
	if(!Math::isInvalid(thrust.afterburner))
		variant[1u] = thrust.afterburner;
}

inline void save(Variant & variant, const Desc::Cons & cons)
{
	variant[0u] = cons.maximal;
	if(!Math::isInvalid(cons.afterburner))
		variant[1u] = cons.afterburner;
}

inline void save(Variant & variant, const Desc::Param & element)
{
	save(variant["thrust"],	element.thrust);
	save(variant["cons"],	element.cons);
}

static const State defaultState = { 0.0f, false };

static const Cache defaultCache = { Math::Table2DIndex(0, 0) };

} } }

void Sim::SFM::JetEngine::load(const Variant & variant, Desc & desc)
{
	desc.hasAfterburner = variant.key("hasAfterburner", VariantRoot(false));
	loadFromTable(variant, "table", desc.table);
	loadFromTable(variant, "Kdr", desc.kDr);
	desc.acceleration = variant.key("acceleration");
	desc.cx = variant.key("cx");
}

void Sim::SFM::JetEngine::save(Variant & variant, const Desc & desc)
{
	saveToTable(variant, "table", desc.table);
	saveToTable(variant, "Kdr", desc.kDr);
	variant["acceleration"] = desc.acceleration;
	variant["cx"] = desc.cx;
}

struct ThrustAccess
{
	typedef float Type;
	ThrustAccess(bool afterburner) : afterburner_(afterburner) {;}
	inline float & operator()(Desc::Param & element) const
	{
		return afterburner_ ? element.thrust.afterburner : element.thrust.maximal;
	}
	inline const float & operator()(const Desc::Param & element) const
	{
		return afterburner_ ? element.thrust.afterburner : element.thrust.maximal;
	}
private:
	bool afterburner_;
};

float Sim::SFM::JetEngine::getThrustMax(const Desc & desc,
										const Env & env, const State & state,
										Cache & cache)
{
	float coeffT = (1.0f + 0.00618f * env.dT);
	cache.thrustIndex = desc.table.findIndex(env.altKm, env.M, cache.thrustIndex);
	return desc.table.eval(	env.altKm, env.M,
							cache.thrustIndex,
							ThrustAccess(state.afterburner));
}

struct ConsAccess
{
	typedef float Type;
	ConsAccess(bool afterburner) : afterburner_(afterburner) {;}
	inline float & operator()(Desc::Param & element) const
	{
		return afterburner_ ? element.cons.afterburner : element.cons.maximal;
	}
	inline const float & operator()(const Desc::Param & element) const
	{
		return afterburner_ ? element.cons.afterburner : element.cons.maximal;
	}
private:
	bool afterburner_;
};

float Sim::SFM::JetEngine::getCe(	const Desc & desc,
									const Env & env, const State & state,
									Cache & cache)
{
	cache.thrustIndex = desc.table.findIndex(env.altKm, env.M, cache.thrustIndex);
	return desc.table.eval(	env.altKm, env.M,
							cache.thrustIndex,
							ConsAccess(state.afterburner)) / 3600.0f;
}

float Sim::SFM::JetEngine::getCe(	const Desc & desc,
									float Pr, const Env & env, const State & state,
									Cache & cache)
{
	float Ce = getCe(desc, env, state, cache);
	float nDr = getEngineRpmRel(Pr);
	return Ce * desc.kDr.eval(nDr);
}