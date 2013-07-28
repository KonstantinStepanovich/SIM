#ifndef _SFM_JetEngine_hpp_
#define _SFM_JetEngine_hpp_

#include "Math/BilinearInterpolation.hpp"
#include "Math/FamilyOfPolynoms.hpp"

#include "Database/Entity.hpp"

//Jet Engine with afterburner or not

namespace Sim
{

class Variant;

namespace SFM
{

namespace JetEngine
{

struct Descriptor : public DescBase
{
	bool hasAfterburner; //has afterburner
	union Thrust
	{
		//Maximal thrust, N
		struct
		{
			float maximal;
			float afterburner;
		};
		float value[2];
	};
	union Cons
	{
		//Fuel consumption, kg/(N*h)
		struct
		{
			float maximal;
			float afterburner;
		};
		float value[2];
	};
	struct Param
	{
		Thrust	thrust;
		Cons	cons;
	};
	struct ScalarsAccessParam
	{
		enum { SIZE = 4 };
		enum {	FIELD_THRUST_MAX,
				FIELD_THRUST_AFT,
				FIELD_CONS_MAX,
				FIELD_CONS_AFT };
		inline float & operator()(Param & element, size_t index) const { return index < FIELD_CONS_MAX ? element.thrust.value[index] : element.cons.value[index - FIELD_CONS_MAX]; };
		inline const float & operator()(const Param & element, size_t index) const { return index < FIELD_CONS_MAX ? element.thrust.value[index] : element.cons.value[index - FIELD_CONS_MAX]; };
	};
	typedef Math::BilinearInterpolator<float, Param, ScalarsAccessParam> Table;
	Table table;
	enum { POLYNOM_ENG_DROSSEL = 4 };
	Math::Polynom<float, POLYNOM_ENG_DROSSEL> kDr; //engine drossel characteristics
	float acceleration; //acceleration, tau in first-order-filter
	float cx; //drag of cold engine
};
typedef Descriptor Desc;

void load(const Variant & variant, Desc & desc);

void save(Variant & variant, const Desc & desc);

typedef Database<Desc> Database;

struct Environment
{
	float altKm; //Satm1976 equivalent altitude, km
	float M; //Mach number
	float dT; //temperature difference T-TSatm1976(altKm), K
};
typedef Environment Env;

struct State
{
	float thrust;
	bool afterburner;
};
extern const State defaultState;

struct Cache
{
	Math::Table2DIndex thrustIndex;
};
extern const Cache defaultCache;

//returns maximal thrust, N
float getThrustMax(	const Desc & desc,
					const Env & env, const State & state,
					Cache & cache);

//returns fuel consumption, kg/N/s
float getCe(const Desc & desc,
			const Env & env, const State & state,
			Cache & cache);

const float Pr0 = 0.05f; //idle relative thrust

//returns jet engine relative RPM by relative thrust (Pr = P/Pmax)
inline float getEngineRpmRel(float Pr)
{
	return 0.92f * pow((Pr - Pr0) / (1.0f - Pr0) + 0.2872f, 0.33f);
}

//returns fuel consumption, kg/N/s, accounts drossel characteristics
float getCe(const Desc & desc,
			float Pr, const Env & env, const State & state,
			Cache & cache);

}

}

}

#endif _SFM_JetEngine_hpp_