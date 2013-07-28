#include "Modules/SFM/Model/AirframeForce.hpp"

#include "Math/Scalar.hpp"
#include "Math/Mat3x3.hpp"
#include "Math/Equation.hpp"
#include "Debug/CheckValue.hpp"
#include "Debug/CheckMath.hpp"

#include "Database/Variant.hpp"
#include "Database/VariantUtil.hpp"
#include "Utils/VariantMathUtil.hpp"

namespace Sim { namespace SFM { namespace AirframeForce {

void load(const Variant & variant, Desc::Param & param)
{
	param.cx0		= variant.key("cx0");
	param.dCydAoA	= variant.key("dCydAoA");
	param.CyMax		= variant.key("CyMax");
	param.bPol2		= variant.key("bPol2");
	param.bPol4		= variant.key("bPol4");
}

void save(Variant & variant, const Desc::Param & param)
{
	variant["cx0"]			= param.cx0;
	variant["dCydAoA"]	= param.dCydAoA;
	variant["CyMax"]		= param.CyMax;
	variant["bPol2"]		= param.bPol2;
	variant["bPol4"]		= param.bPol4;
}

bool hasContainer(const PayloadTree & payloadTree, PayloadTree::const_iterator where)
{
	PayloadTree::const_iterator it = where;
	while(it != payloadTree.root())
	{
		it.down();
		if(PayloadDragDatabase::instance().getDesc(it->typeId).inner)
			return true;
	}
	return false;
}

} } }

using namespace Sim;
using namespace Sim::SFM::AirframeForce;

void Sim::SFM::AirframeForce::load(const Variant & variant, Desc & desc)
{
	loadFromTable(variant, "table", desc.table);
	desc.Cy0 = variant.key("Cy0");
	desc.flapsCx = variant.key("flapsCx");
	desc.flapsCy = variant.key("flapsCy");
	desc.airBrakeCx = variant.key("airBrakeCx");
	desc.dragParachuteCx = variant.key("dragParachuteCx");
	desc.gearCx = variant.key("gearCx");
	desc.S = variant.key("S");
}

void Sim::SFM::AirframeForce::save(Variant & variant, const Desc & desc)
{
	saveToTable(variant, "table", desc.table);
	variant["Cy0"] = desc.Cy0;
	variant["flapsCx"] = desc.flapsCx;
	variant["flapsCy"] = desc.flapsCy;
	variant["airBrakeCx"] = desc.airBrakeCx;
	variant["dragParachuteCx"] = desc.dragParachuteCx;
	variant["gearCx"] = desc.gearCx;
	variant["S"] = desc.S;
}

void Sim::SFM::AirframeForce::load(const Variant & variant, PayloadDragDesc & desc)
{
	desc.cx = variant.key("cx", VariantRoot(0.0f));
	desc.inner = variant.key("inner", VariantRoot(false));
}

void Sim::SFM::AirframeForce::save(Variant & variant, const PayloadDragDesc & desc)
{
	variant["cx"] = desc.cx;
	variant["inner"] = desc.inner;
}

void Sim::SFM::AirframeForce::addPayload(State & state, const PayloadTree & payloadTree, PayloadTree::const_iterator where)
{
	if(hasContainer(payloadTree, where))
		return;
	const Payload & payload = *where;
	state.payloadCx += PayloadDragDatabase::instance().getDesc(payload.typeId).cx;
}

void Sim::SFM::AirframeForce::removePayload(State & state, const PayloadTree & payloadTree, PayloadTree::const_iterator where)
{
	if(hasContainer(payloadTree, where))
		return;
	const Payload & payload = *where;
	state.payloadCx += PayloadDragDatabase::instance().getDesc(payload.typeId).cx;
}


inline float getKCx(float M) //cx(M) = KCx(M) * cx, cx factor for payload
{
	if(M < 0.8f)
		return 1.f;
	else if(M < 1.0f)
		return (1.f + 8.9168f * (M - 0.8f));
	else
		return (0.33996f * M * M - 2.1376f * M + 4.581f);
}

AerCoeffs Sim::SFM::AirframeForce::getAerCoeffsLim(	const Desc & desc, const Desc::Param & param,
													float AoA)
{
	float dCy;
	const float alphaMax = fabs(param.CyMax / param.dCydAoA);
	if(fabs(AoA) > 2.0f * alphaMax)
		dCy = 0.0f;
	else if(AoA > alphaMax)
		dCy = param.CyMax + (alphaMax - AoA) * param.dCydAoA;
	else if(AoA < -alphaMax)
		dCy = param.CyMax + (AoA + alphaMax) * param.dCydAoA;
	else
		dCy = AoA * param.dCydAoA;
	assert(fabs(dCy) <= param.CyMax);
	return AerCoeffs(getCxBydCy(param, dCy), desc.Cy0 + dCy);
}

AerCoeffs Sim::SFM::AirframeForce::getStateAerCoeffs(const Desc & desc, float M, const State & state)
{
	return AerCoeffs(	state.flaps * desc.flapsCx + state.airBrake * desc.airBrakeCx +
						state.gear * desc.gearCx + state.dragParachute * desc.dragParachuteCx +
						state.payloadCx * getKCx(M),
						state.flaps * desc.flapsCy);
}

AerCoeffs Sim::SFM::AirframeForce::calcAerCoeffs(const Descriptor & desc, const State & state, const Env & env, Cache & cache)
{
	Real TAS = env.TAS.length();
	if(TAS < 1.0E-5f) //No money - no honey
		return AerCoeffs(0.0, 0.0);

	//Force
	Math::Vec3 velNorm = env.TAS / TAS;
	Real AoA = atan2(-velNorm.dot(Math::Vec3(0.0, 1.0, 0.0)), velNorm.dot(Math::Vec3(1.0, 0.0, 0.0)));
	validateScalar(AoA);

	Desc::Param param;
	getAerParam(desc, env.M, cache.tableIndex, param);
	AerCoeffs aerCoeffs = getAerCoeffs(desc, param, AoA) + getStateAerCoeffs(desc, env.M, state);
	validate(aerCoeffs.cx);
	validate(aerCoeffs.cy);
	return aerCoeffs;
}

Math::Vec3 Sim::SFM::AirframeForce::calcAirVelForceByAerCoeffs(const Descriptor & desc, const Env & env, const AerCoeffs & aerCoeffs)
{
	const Real q = 0.5f * env.TAS.length2() * env.rho;
	Math::Vec3 airVelForce = q * desc.S * Math::Vec3(-aerCoeffs.cx, aerCoeffs.cy, 0.0f);
	validate(airVelForce);
	return airVelForce;
}

Math::Vec3 Sim::SFM::AirframeForce::calcForceByAirVelForce(const Math::Vec3 & VaNorm, const Math::Vec3 & airVelForce)
{
	Math::Matrix3x3 airVelBasis;
	airVelBasis[0] = VaNorm;
	airVelBasis[2] = Math::Vec3(0.0f, 0.0f, 1.0f);
	airVelBasis[1] = airVelBasis[2].cross(airVelBasis[0]);
	airVelBasis[2] = airVelBasis[0].cross(airVelBasis[1]);
	Math::Vec3 force = airVelBasis.inverse() * airVelForce;
	validate(force);
	return force;
}

Math::Vec3 Sim::SFM::AirframeForce::calcForceByAerCoeffs(const Descriptor & desc, const Env & env, const AerCoeffs & aerCoeffs)
{
	const Math::Vec3 airVelForce = calcAirVelForceByAerCoeffs(desc, env, aerCoeffs);
	double va = env.TAS.length();
	if(va > 1.0E-5)
	{
		const Math::Vec3 force = calcForceByAirVelForce(env.TAS / va, airVelForce);
		return force;
	}
	else
		return Math::Vec3(0.0, 0.0, 0.0);
}

Math::Vec3 Sim::SFM::AirframeForce::calcForce(const Descriptor & desc, const State & state, const Env & env, Cache & cache)
{
	const AerCoeffs aerCoeffs = calcAerCoeffs(desc, state, env, cache);
	const Math::Vec3 force = calcForceByAerCoeffs(desc, env, aerCoeffs);
	return force;
}

float Sim::SFM::AirframeForce::getGlideM(const Desc & desc, Math::TableIndex & tableIndex)
{
	char dir = 0;
	while(true)
	{
		const Desc::Table::Element & curr = desc.table.getElement(tableIndex);
		const Desc::Table::Element & next = desc.table.getElement(tableIndex + 1);

		float dCx0dM = (next.val.cx0 - curr.val.cx0) / (next.arg - curr.arg);
		float dBPoldM = (next.val.bPol2 - curr.val.bPol2) / (next.arg - curr.arg);

		float dM = -(dCx0dM * curr.val.bPol2 + dBPoldM * curr.val.cx0) / (2 * dCx0dM * dBPoldM);
		if(dM > 0.0f)
			return curr.arg + dM;
		else
		{
			float LDcur = getMaxLD(curr.val, desc.Cy0);
			float LDnext = getMaxLD(next.val, desc.Cy0);
			if(LDcur <= LDnext)
			{
				if(	dir == -1 ||
					tableIndex == desc.table.size() - 2)
					return next.arg;
				else
				{
					tableIndex++;
					dir = 1;
				}
			}
			else
			{
				if(	dir == 1 ||
					tableIndex == 0)
					return curr.arg;
				else
				{
					tableIndex--;
					dir = -1;
				}
			}
		}
	}
}

float Sim::SFM::AirframeForce::getTASbyLift(const Desc & desc, float rho, float Vs, float L, Math::TableIndex & tableIndex)
{
	float leftL, rightL;
	while(true)
	{
		const Desc::Table::Element & left = desc.table.getElement(tableIndex);
		const Desc::Table::Element & right = desc.table.getElement(tableIndex + 1);
		leftL = left.val.CyMax * rho * pow(Vs * left.arg, 2) / 2.0f * desc.S;
		rightL = right.val.CyMax * rho * pow(Vs * right.arg, 2) / 2.0f * desc.S;
		if(L < leftL)
		{
			if(tableIndex > 0)
				tableIndex--;
			else
				return left.arg * Vs;
		}
		else if(L > rightL)
		{
			if(tableIndex < desc.table.size() - 2)
				tableIndex++;
			else
				return right.arg * Vs;
		}
		else
		{
			const float dCyMaxdM = (right.val.CyMax - left.val.CyMax) / (right.arg - left.arg);
			class Func : public std::unary_function<float, float>
			{
			public:
				Func(	float M0, float CyMax0, float dCyMaxdM, float L,
					float rho, float Vs, float S) : M0_(M0), CyMax0_(CyMax0), dCyMaxdM_(dCyMaxdM),
					L_(L), rho_(rho), Vs_(Vs), S_(S)
				{
				}
				inline float operator()(float M) const
				{
					return (CyMax0_ + dCyMaxdM_ * (M - M0_)) * rho_ * 0.5f * pow(M * Vs_, 2) * S_ - L_;
				}
			private:
				float M0_;
				float CyMax0_;
				float dCyMaxdM_;				
				float L_;
				float rho_;
				float Vs_;
				float S_;
			};
			Func func(left.arg, left.val.CyMax, dCyMaxdM, L, rho, Vs, desc.S);

			class Deriv : public std::unary_function<float, float>
			{
			public:
				Deriv(	float M0, float CyMax0, float dCyMaxdM, float L,
					float rho, float Vs, float S) : M0_(M0), CyMax0_(CyMax0), dCyMaxdM_(dCyMaxdM),
					L_(L), rho_(rho), Vs_(Vs), S_(S)
				{
				}
				inline float operator()(float M) const
				{
					return rho_ * pow(Vs_, 2) * S_ * M * (CyMax0_ + dCyMaxdM_ * (3 * M / 2.0f - M0_));
				}
			private:
				float M0_;
				float CyMax0_;
				float dCyMaxdM_;				
				float L_;
				float rho_;
				float Vs_;
				float S_;
			};
			Deriv deriv(left.arg, left.val.CyMax, dCyMaxdM, L, rho, Vs, desc.S);

			const float M = Math::findNewton(0.5f * (left.arg + right.arg), 0.01f, func, deriv);
			return M * Vs;
		}
	}
}