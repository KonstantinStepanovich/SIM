#include "stdafx.h"
#include "Modules/Satm1976/Satm1976.hpp"
#include <math.h>

using namespace Sim::Satm1976;

void testAltPr()
{
	printf("Testing Pr = f(alt) and alt = f(Pr)\n");
	unsigned int failCounter = 0;
	const float altKm[] = {0.0f, 0.1f, 0.8f, 2.0f, 4.0f, 7.5f, 10.0f, 15.0f, 18.5f, 25.0f, 35.0f};
	const float tolerance = 1.0E-5f;
	for(unsigned int i = 0; i < sizeof(altKm) / sizeof(float); i++)
	{
		printf("alt=%.2f km ", altKm[i]);
		unsigned int level = getLevelByAltKm(altKm[i]);
		AtmParam atmParam;
		getAtmParamByAltKm(altKm[i], level, atmParam);
		float altKmRtrn = getAltKmByPr(level, atmParam.Pr);
		printf("Pr=%.2f, alt=f(Pr)=%.2f", atmParam.Pr, altKmRtrn);
		if(fabs(altKm[i] - altKmRtrn) < tolerance)
			printf(" OK\n");
		else
		{
			failCounter++;
			printf(" FAIL!\n");
		}
	}
	if(failCounter == 0)
		printf("SUCCESS!\n");
	else
		printf("%d tests FAILED!\n", failCounter);
}

void testModel()
{
	const float TK0 = -273.0f;
	const float T0 = 15.0f - TK0;
	const float P0 = 101325.0f;
	const float rho0 = 1.225f;

	const float toleranceRho = 0.01f;
	const float toleranceP = 1000.0f;
	const float toleranceT = 0.5f;

	struct Data
	{
		float altKm;
		float rho; // density, kg/m^3
		float P; //pressure, Pa
		float Va; //sound velocity, m/s
		float T; //temperature, C
	};
	Data data[]  = {
		{0,	1.225,	101325,	340.3,	15},
		{0.2,1.202,	98945,	339.5,	13.7},
		{0.5,1.167,	95461,	338.4,	11.75},
		{1,	1.112,	89876,	336.4,	8.5},
		{2,	1.007,	79501,	332.5,	2},
		{3,	0.909,	70121,	328.6,	-4.5},
		{4,	0.819,	61660,	324.6,	-11},
		{5,	0.736,	54048,	320.5,	-17.5},
		{6,	0.660,	47218,	316.5,	-24},
		{7,	0.590,	41105,	312.3,	-30.5},
		{8,	0.526,	35652,	308.1,	-37},
		{9,	0.466,	30742,	303.8,	-43.5},
		{10,0.413,	26436,	299.5,	-50},
		{11,0.364,	22632,	295.1,	-56.5},
		{12,0.311,	19330,	295.1,	-56.5},
		{13,0.266,	16510,	295.1,	-56.5},
		{14,0.227,	14102,	295.1,	-56.5},
		{15,0.194,	12045,	295.1,	-56.5},
		{16,0.165,	10287,	295.1,	-56.5},
		{17,0.141,	8787,	295.1,	-56.5},
		{18,	0.121,	7505,	295.1,	-56.5},
		{19,	0.103,	6410,	295.1,	-56.5},
		{20,	0.088,	5475,	295.1,	-56.5} };

	printf("Testing P, T, rho = f(alt)\n");
	unsigned int failCounter = 0;
	for(unsigned int i = 0; i < sizeof(data) / sizeof(Data); i++)
	{
		printf("alt=%.2f km,", data[i].altKm);
		unsigned int level = getLevelByAltKm(data[i].altKm);
		AtmParam atmParam;
		getAtmParamByAltKm(data[i].altKm, level, atmParam);
		printf("T=%.2f/%.2f C", atmParam.Tr * T0 + TK0, data[i].T);
		if(fabs(atmParam.Tr * T0 + TK0 - data[i].T) < toleranceT)
			printf("-OK,");
		else
		{
			failCounter++;
			printf("-FAILED,");
		}

		printf("P=%.f/%.f Pa", P0 * atmParam.Pr, data[i].P);
		if(fabs(P0 * atmParam.Pr - data[i].P) < toleranceP)
			printf("-OK,");
		else
		{
			failCounter++;
			printf("-FAILED,");
		}

		printf("rho=%.3f/%.3f kg/m^3", rho0 * atmParam.rhor, data[i].rho);
		if(fabs(rho0 * atmParam.rhor - data[i].rho) < toleranceRho)
			printf("-OK");
		else
		{
			failCounter++;
			printf("-FAILED");
		}
		printf("\n");
	}
	if(failCounter == 0)
		printf("SUCCESS!\n");
	else
		printf("%d tests FAILED!\n", failCounter);
}

int _tmain(int argc, _TCHAR* argv[])
{
	testAltPr();
	printf("Press Enter to start next test\n");
	getchar();

	testModel();
	printf("Press Enter to exit\n");
	getchar();

	//Sim::Satm1976 * satm1976 = new Sim::Satm1976();
	//satm1976->setBaseWind(Sim::Math::Vec3(0.0f, 0.0f, 3.0f));

	//Sim::IAtmosphere::AtmPoint atmPoint;
	//atmPoint.point = Sim::Math::Vec3(-23412.0f, 214596.0f, 968312.0f);
	//Sim::Math::Vec3 wind = satm1976->getWind(atmPoint);
	return 0;
}
