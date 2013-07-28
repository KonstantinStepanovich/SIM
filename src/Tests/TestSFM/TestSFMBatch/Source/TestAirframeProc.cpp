#include "stdafx.h"

#include "TestAirframe.hpp"
#include "ParamIteration.hpp"

#include <stdio.h>
#include <iostream>
#include <math.h>

#include "Algorithm/Exception.hpp"
#include "Database/DatabaseUtil.hpp"
#include "Database/ParserLua.hpp"

#include "Modules/SFM/Model/Airframe.hpp"

using namespace Sim;
using namespace Sim::SFM;

static void read(int argc, const char ** argv, TestAirframe & test)
{
	if(argc < 3)
		THROW_EXCEPTION(Exception, "parameters missed");

	const char * airframeName = argv[0];
	Airframe::Desc airframeDesc;
	loadDescFile(airframeName, "../../SFM/Airframes/", "lua", &loadLua, "", airframeDesc);
	test.desc = airframeDesc.force;
	
	read(test.desc.table , test.M);
	sscanf(argv[1], "%f", &test.M.step);

	test.AoA.start = 0.0f;
	float CyMax = test.desc.table.getElement(test.desc.table.size() - 1).val.CyMax;
	float dCydAoA = test.desc.table.getElement(test.desc.table.size() - 1).val.dCydAoA;
	test.AoA.end = CyMax / dCydAoA;
	sscanf(argv[2], "%f", &test.AoA.step);
}

static void perform(const TestLD & test)
{
	iteratePrint2(test.M, test.AoA, FuncLD(test.desc));
}

void runTestLD(int argc, const char ** argv)
{
	TestLD test;
	read(argc, argv, test);
	perform(test);
}

static void read(int argc, const char ** argv, TestAoAOptimal & test)
{
	if(argc < 2)
		THROW_EXCEPTION(Exception, "parameters missed");

	const char * airframeName = argv[0];
	Airframe::Desc airframeDesc;
	loadDescFile(airframeName, "../../SFM/Airframes/", "lua", &loadLua, "", airframeDesc);
	test.desc = airframeDesc.force;

	read(test.desc.table, test.M);
	sscanf(argv[1], "%f", &test.M.step);
}

void performTestAoAOptimal(const TestAoAOptimal & test)
{
	iteratePrint1(test.M, FuncAoAOptimal(test.desc));
}

void runTestAoAOptimal(int argc, const char ** argv)
{
	TestAoAOptimal test;
	read(argc, argv, test);
	performTestAoAOptimal(test);
}

void performTestMaxLD(const TestMaxLD & test)
{
	iteratePrint1(test.M, FuncMaxLD(test.desc));
}

void runTestMaxLD(int argc, const char ** argv)
{
	TestMaxLD test;
	read(argc, argv, test);
	performTestMaxLD(test);
}

static void read(int argc, const char ** argv, TestGlideM & test)
{
	if(argc < 1)
		THROW_EXCEPTION(Exception, "parameters missed");

	const char * airframeName = argv[0];
	Airframe::Desc airframeDesc;
	loadDescFile(airframeName, "../../SFM/Airframes/", "lua", &loadLua, "", airframeDesc);
	test.desc = airframeDesc.force;
}

void performTestGlideM(const TestGlideM & test)
{
	Math::TableIndex tableIndex = 0;
	std::cout << getGlideM(test.desc, tableIndex) << " M";
};

void runTestGlideM(int argc, const char ** argv)
{
	TestGlideM test;
	read(argc, argv, test);
	performTestGlideM(test);
}

struct TestTorqueMxMy
{
	AirframeTorque::DescMxMy desc;
	IterationRange<float> AoS;
	enum TorqueAxisId { TORQUE_MX, TORQUE_MY };
	TorqueAxisId torqueAxisId;
	AirframeTorque::State state;
	double Cy;
};

void performTestMxMy(const TestTorqueMxMy & test)
{
	struct MxMyAoS
	{
		double MxAoS;
		double MyAoS;
	};
	struct FuncMxMy : std::unary_function<double, MxMyAoS>
	{
		FuncMxMy(const AirframeTorque::DescriptorMxMy & desc, const AirframeTorque::State & state, float Cy) : 
			desc_(desc), state_(state), Cy_(Cy) {;}
		MxMyAoS operator()(double AoS) const
		{
			MxMyAoS mxMyAoS;
			AirframeTorque::calcMxMyAoS(desc_, state_, AoS, Cy_, mxMyAoS.MxAoS, mxMyAoS.MyAoS);
			return mxMyAoS;
		}
	private:
		const AirframeTorque::DescriptorMxMy & desc_;
		AirframeTorque::State state_;
		float Cy_;
	};
	
	std::cout << test.state.aileron << std::endl;
	std::cout << test.state.wingIntegrity << std::endl;
	std::cout << test.state.rudder << std::endl;
	std::cout << test.state.finIntegrity << std::endl;
	std::cout << test.Cy << std::endl;

	if(test.torqueAxisId == TestTorqueMxMy::TORQUE_MX)
		return	iteratePrint1(	test.AoS,
							FuncMxMy(test.desc, test.state, test.Cy), 
							makePrintMember(&MxMyAoS::MxAoS));
	else
		return	iteratePrint1(	test.AoS,
							FuncMxMy(test.desc, test.state, test.Cy), 
							makePrintMember(&MxMyAoS::MyAoS));
}

void runTestMxMy(int argc, const char ** argv)
{
	if(argc < 7)
		THROW_EXCEPTION(Exception, "parameters missed");

	TestTorqueMxMy test;

	const char * airframeName = argv[0];
	Airframe::Desc airframeDesc;
	loadDescFile(airframeName, "../../SFM/Airframes/", "lua", &loadLua, "", airframeDesc);
	test.desc = airframeDesc.torque.MxMy;

	int torqueAxisId;
	sscanf(argv[1], "%d", &torqueAxisId);
	test.torqueAxisId = (TestTorqueMxMy::TorqueAxisId)torqueAxisId;

	test.AoS.start = -M_PI_2;
	test.AoS.end = M_PI_2;
	test.AoS.step = M_PI / 50.0f;

	test.state.flags = AirframeTorque::State::FLAG_WING_INTEGRITY | AirframeTorque::State::FLAG_FIN_INTEGRITY;
	sscanf(argv[2], "%lf", &test.state.aileron);
	sscanf(argv[3], "%lf", &test.state.wingIntegrity);
	sscanf(argv[4], "%lf", &test.state.rudder);
	sscanf(argv[5], "%lf", &test.state.finIntegrity);
	sscanf(argv[6], "%lf", &test.Cy);

	performTestMxMy(test);
}

struct TestTorqueMz
{
	AirframeTorque::DescMz desc;
	IterationRange<float> AoA;
	AirframeTorque::State state;
};

void performTestMz(const TestTorqueMz & test)
{
	struct FuncMz : std::unary_function<double, double>
	{
		FuncMz(	const AirframeTorque::DescriptorMz & desc, const AirframeTorque::State & state) : desc_(desc),
				state_(state) {;}
		double operator()(double AoA) const
		{
			return AirframeTorque::calcMzAoA(desc_, state_, AoA);
		}
	private:
		const AirframeTorque::DescriptorMz & desc_;
		AirframeTorque::State state_;
	};
	std::cout << test.state.elevator << std::endl;
	std::cout << test.state.stabilizierIntegrity << std::endl;
	iteratePrint1(test.AoA, FuncMz(test.desc, test.state));	
}

void runTestMz(int argc, const char ** argv)
{
	if(argc < 3)
		THROW_EXCEPTION(Exception, "parameters missed");

	TestTorqueMz test;

	const char * airframeName = argv[0];
	Airframe::Desc airframeDesc;
	loadDescFile(airframeName, "../../SFM/Airframes/", "lua", &loadLua, "", airframeDesc);
	test.desc = airframeDesc.torque.Mz;

	test.AoA.start = -M_PI;
	test.AoA.end = M_PI;
	test.AoA.step = M_PI / 50.0f;

	test.state.flags = AirframeTorque::State::FLAG_STABILIZIER_INTEGRITY;
	sscanf(argv[1], "%lf", &test.state.elevator);
	sscanf(argv[2], "%lf", &test.state.stabilizierIntegrity);

	performTestMz(test);
}

struct TestTorqueMxOmegaX
{
	AirframeTorque::DescriptorMxMy desc;
	IterationRange<float> omegaX;
	double AoA;
};

void performTestMz(const TestTorqueMxOmegaX & test)
{
	struct FuncMxOmegaX : std::unary_function<double, double>
	{
		FuncMxOmegaX(const AirframeTorque::DescriptorMxMy & desc, double AoA) : desc_(desc), alpha_(AoA) {;}
		double operator()(double omegaX) const
		{
			return AirframeTorque::calcMxOmegaX(desc_, omegaX, alpha_);
		}
	private:
		const AirframeTorque::DescriptorMxMy & desc_;
		const double alpha_;
	};
	std::cout << test.AoA << std::endl;
	iteratePrint1(test.omegaX, FuncMxOmegaX(test.desc, test.AoA));	
}

void runTestMXOmegaX(int argc, const char ** argv)
{
	if(argc < 2)
		THROW_EXCEPTION(Exception, "parameters missed");

	TestTorqueMxOmegaX test;

	const char * airframeName = argv[0];
	Airframe::Desc airframeDesc;
	loadDescFile(airframeName, "../../SFM/Airframes/", "lua", loadLua, "", airframeDesc);
	test.desc = airframeDesc.torque.MxMy;

	test.omegaX.start = 0.0;
	test.omegaX.end = 5.4;
	test.omegaX.step = 0.05;

	sscanf(argv[1], "%lf", &test.AoA);

	performTestMz(test);
}