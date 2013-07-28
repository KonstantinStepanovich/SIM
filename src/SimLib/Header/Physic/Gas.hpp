#ifndef _Gas_hpp_
#define _Gas_hpp_

#include <math.h>

namespace Sim
{

static const float psi = 7.0f / 5.0f; //air adiabate factor
static const float R = 8.31441f; //perfect gas constant
static const float airMolarMass = 0.0290f; //molar mass of air, kg/mol

inline float getSoundVel(float T) //get air velocity by temperature, K
{
	return sqrt(psi * R * T / airMolarMass);
}

}

#endif _Gas_hpp_