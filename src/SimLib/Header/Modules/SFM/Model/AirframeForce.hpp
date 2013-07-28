#ifndef _SFM_AirframeForce_hpp_
#define _SFM_AirframeForce_hpp_

#include "Modules/SFM/Payload.hpp"

#include "Math/LinearInterpolation.hpp"
#include "Math/Vec3.hpp"

#include "Database/Entity.hpp"

namespace Sim
{

class Variant;

namespace SFM
{

namespace AirframeForce
{

//Descriptors

struct Descriptor : public DescBase
{
	union Param
	{
		struct 
		{
			float cx0;
			float dCydAoA; //AoA in rad
			float CyMax;
			float bPol2;
			float bPol4;
		};
		enum { SIZE = 5 };
		float value[SIZE];
	};
	struct ScalarsAccessParam
	{
		enum { SIZE = 5 };
		inline float & operator()(Param & param, size_t index) const { return param.value[index]; };
		inline const float & operator()(const Param & param, size_t index) const { return param.value[index]; };
	};
	typedef Math::LinearInterpolator<float, Param, ScalarsAccessParam> Table;
	Table table;

	float Cy0; //Cy0 for AOA = 0
	float flapsCx; //flaps drag index
	float flapsCy; //flaps lift index
	float airBrakeCx; //air brake drag index
	float dragParachuteCx; //drag parachute drag index
	float gearCx; //gear drag index
	float S; //characteristic surface square, m^2
};

typedef Descriptor Desc;

void load(const Variant & variant, Descriptor & desc);

void save(Variant & variant, const Descriptor & desc);

typedef Database<Descriptor> AirframeDatabase;

struct PayloadDragDesc : public DescBase
{
	float cx; //payload drag
	bool inner; //is it a container for store inside (childs have no drag effect)
};

void load(const Variant & variant, PayloadDragDesc & desc);

void save(Variant & variant, const PayloadDragDesc & desc);

typedef Database<PayloadDragDesc> PayloadDragDatabase;

//State

struct State
{
	float gear;
	float flaps;
	float airBrake;
	float dragParachute;
	float payloadCx;
};

void addPayload(State & state, const PayloadTree & payloadTree, PayloadTree::const_iterator where);

void removePayload(State & state, const PayloadTree & payloadTree, PayloadTree::const_iterator where);

//Simulation

inline void getAerParam(const Descriptor & desc, float M,
						Math::TableIndex & tableIndex, Descriptor::Param & param)
{
	param = desc.table.eval(M, tableIndex = desc.table.findIndex(M, tableIndex));
}

inline float getCxBydCy(const Descriptor::Param & param, float dCy)
{
	return param.cx0 + param.bPol2 * pow(dCy, 2) + param.bPol4 * pow(dCy, 4);
}

struct AerCoeffs
{
	AerCoeffs(float cxIn, float cyIn) : cx(cxIn), cy(cyIn) {;}
	float cx;
	float cy;
};

inline AerCoeffs operator + (AerCoeffs & left, AerCoeffs & right)
{
	return AerCoeffs(left.cx + right.cx, left.cy + right.cy);
}

inline AerCoeffs getAerCoeffs(const Descriptor & desc, const Descriptor::Param & param, float AoA)
{
	float dCy = AoA * param.dCydAoA;
	return AerCoeffs(getCxBydCy(param, dCy), desc.Cy0 + dCy);
}

AerCoeffs getAerCoeffsLim(const Descriptor & desc, const Descriptor::Param & param, float AoA);

AerCoeffs getStateAerCoeffs(const Descriptor & desc, float M, const State & state);

struct Environment
{
	Math::Vec3 TAS; //true local airspeed, m/s
	float M; //Mach number
	float rho; //air density, kg/m^3
	float q; //air dynamic pressure, kg/m*s
};

typedef Environment Env;

struct Cache
{
	Math::TableIndex tableIndex;
};

AerCoeffs calcAerCoeffs(const Descriptor & desc, const State & state, const Env & env, Cache & cache);

Math::Vec3 calcAirVelForceByAerCoeffs(const Descriptor & desc, const Env & env, const AerCoeffs & aerCoeffs);

Math::Vec3 calcForceByAirVelForce(const Math::Vec3 & VaNorm, const Math::Vec3 & airVelForce);

Math::Vec3 calcForceByAerCoeffs(const Descriptor & desc, const Env & env, const AerCoeffs & aerCoeffs);

Math::Vec3 calcForce(const Descriptor & desc, const State & state, const Env & env, Cache & cache);

//Performance

inline float getOptimalAoA(const Descriptor::Param & param, float Cy0)
{
	return (sqrt(param.cx0 / param.bPol2) - Cy0) / param.dCydAoA;
}

inline float getMaxLD(const Descriptor::Param & param, float Cy0)
{
	float a = sqrt(param.cx0 / param.bPol2);
	return a / (pow(a - Cy0, 2) * param.bPol2 + param.cx0);
}

inline float getMaxLD(const Descriptor::Param & param)
{
	return 1.0f / (2.0f * sqrt(param.cx0 * param.bPol2));
}

float getGlideM(const Descriptor & desc, Math::TableIndex & tableIndex);

float getTASbyLift(const Descriptor & desc, float rho, float Vs, float L,
				   Math::TableIndex & tableIndex);

}

}

}

#endif _SFM_AirframeForce_hpp_ 