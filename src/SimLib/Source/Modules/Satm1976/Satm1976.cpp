#include "Modules/Satm1976/Satm1976.hpp"

#include <algorithm>
#include <functional>
#include <math.h>

static const float REARTH = 6369.0f; //radius of the Earth (km)
static const float GMR = 34.163195f; //hydrostatic constant

static const size_t NTAB = 8;
static const float htab[NTAB] = {0.0f, 11.0f, 20.0f, 32.0f, 47.0f, 51.0f, 71.0f, 84.852f}; //levels, km
static const float ttab[NTAB] = {288.15f, 216.65f, 216.65f, 228.65f, 270.65f, 270.65f, 214.65f, 186.946f}; //temperature base by level, K
static const float ptab[NTAB] = {1.0f, 2.233611E-1f, 5.403295E-2f, 8.5666784E-3f, 1.0945601E-3f, 6.6063531E-4f, 3.9046834E-5f, 3.68501E-6f}; //pressure by level, 
static const float gtab[NTAB] = {-6.5f, 0.0f, 1.0f, 2.8f, 0.0f, -2.8f, -2.0f, 0.0f}; //temperature gradient by level, K

static const float psi = 7.0f / 5.0f; //air adiabate factor
static const float R = 8.31441f; //perfect gas constant
static const float airMolarMass = 0.0290f; //molar mass of air, kg/mol

#define INLINE_SATM

const float Sim::Satm1976::T0 = 273.0f + 15.0f; //15 C
const float Sim::Satm1976::P0 = 101325.0f; //1 atm
const float Sim::Satm1976::rho0 = 1.22500f;

using struct Sim::Satm1976::AtmParam;

INLINE_SATM unsigned int Sim::Satm1976::getLevelByAltKm(float altKm)
{
	return std::upper_bound(htab, htab + NTAB, altKm) - htab - 1;
}

INLINE_SATM unsigned int Sim::Satm1976::updateLevelByAltKm(unsigned int level, float altKm)
{
	if(altKm < htab[level])
		return level > 0 ? std::upper_bound(htab, htab + level, altKm) - htab - 1 : 0;
	else if(level < NTAB - 1 &&
			altKm >= htab[level + 1])
		return std::upper_bound(htab + level + 1, htab + NTAB, altKm) - htab - 1;
	else
		return level;
}

INLINE_SATM void Sim::Satm1976::getAtmParamByAltKm(float altKm, unsigned int level, AtmParam & atmParam)
{
	float T = ttab[level] + gtab[level] * (altKm - htab[level]);
	float Pr = gtab[level] == 0.0 ? 
							ptab[level] * exp(-GMR * (altKm - htab[level]) / ttab[level])
							:
							ptab[level] * pow(ttab[level] / T, GMR / gtab[level]);
	atmParam.Tr = T / ttab[0];
	atmParam.Pr = Pr;
	atmParam.rhor = Pr / atmParam.Tr;
}

INLINE_SATM unsigned int Sim::Satm1976::getLevelByPr(float Pr)
{
	return std::find_if(ptab, ptab + NTAB, std::bind1st(std::less_equal<float>(), Pr)) - ptab;
}

INLINE_SATM unsigned int Sim::Satm1976::updateLevelByPr(unsigned int level, float Pr)
{
	if(Pr > ptab[level])
		return std::upper_bound(ptab, ptab + level, Pr, std::not2(std::less_equal<float>())) - ptab - 1;
	else if(level < NTAB - 1 &&
			Pr < ptab[level + 1])
		return std::upper_bound(ptab + level + 1, ptab + NTAB, Pr, std::not2(std::less_equal<float>())) - ptab - 1;
	else
		return level;
}

INLINE_SATM float Sim::Satm1976::getAltKmByPr(unsigned int level, float Pr)
{
	if(gtab[level] == 0.0)
	{
		return htab[level] + ttab[level] * log(Pr / ptab[level]) / -GMR;
	}
	else
	{
		float T = ttab[level] / exp(log(Pr / ptab[level]) * gtab[level] / GMR);
		return htab[level] + (T - ttab[level]) / gtab[level];
	}
}