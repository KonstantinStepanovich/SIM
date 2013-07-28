#ifndef _Satm1976_hpp_
#define _Satm1976_hpp_

//Standard atmosphere 1976

namespace Sim
{

namespace Satm1976
{

extern const float T0; //sea level temperature, K
extern const float P0; //sea level pressure, Pa
extern const float rho0; //sea level air density, kg/m^3

struct AtmParam
{
	float Tr; //relative temperature, T/T0
	float Pr; //relative pressure, P/P0
	float rhor; //relative air density, rho/rho0
};

unsigned int getLevelByAltKm(float altKm);

unsigned int updateLevelByAltKm(unsigned int level, float altKm);

void getAtmParamByAltKm(float altKm, unsigned int level, AtmParam & atmParam);

unsigned int getLevelByPr(float Pr);

unsigned int updateLevelByPr(unsigned int level, float Pr);

float getAltKmByPr(unsigned int level, float Pr);

}

}

#endif _Satm1976_hpp_