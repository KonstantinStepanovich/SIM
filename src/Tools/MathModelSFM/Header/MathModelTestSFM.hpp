#ifndef _MathModelTestSFM_hpp_
#define _MathModelTestSFM_hpp_

#include "MathModelSFM.hpp"
#include "ParamIteration.hpp"

#include <vector>

namespace MathModelSFM
{

struct ParametersAlt
{
	double altitude;
};

struct CurveVyMax
{
	Curve curve;
	AirSpeedType airSpeedType;
};

class TestVyMax : public Test
{
public:
	typedef ParametersAlt Parameters;
	TestVyMax(ModelParameters * modelParameters, TestBase * testBase);	
	virtual const char * getName() const;
	virtual void update();
	virtual Value * getResult();
private:
	virtual Value * getSpecificParameters_();
private:
	Parameters parameters_;
	typedef CurveVyMax Result;
	Result result_;
};

struct CurveVminVmax
{
	Curve vMin;
	Curve vMax;
	AirSpeedType airSpeedType;
};

class TestVminVmax : public Test
{
public:
	TestVminVmax(ModelParameters * modelParameters, TestBase * testBase);
	virtual const char * getName() const;
	virtual void update();
	virtual Value * getResult();
private:
	typedef CurveVminVmax Result;
	Result result_;
};

struct ParametersAltArray
{
	std::vector<float> altitude;
};

struct CurvesTurnRate
{
	Curves curves;
	AirSpeedType airSpeedType;
};

class TestTurnRate : public Test
{
public:
	typedef ParametersAltArray Parameters;
	TestTurnRate(ModelParameters * modelParameters, TestBase * testBase);
	virtual const char * getName() const;
	virtual void update();
	virtual Value * getResult();
private:
	virtual Value * getSpecificParameters_();
private:
	Parameters parameters_;
	typedef CurvesTurnRate Result;
	Result result_;
};

struct CurveAoA
{
	Curve curve;
	AirSpeedType airSpeedType;
};

struct ParametersAoA : public ParametersAlt
{
	double AoA0;
};

class TestAoA : public Test
{
public:
	typedef ParametersAoA Parameters;
	TestAoA(ModelParameters * modelParameters, TestBase * testBase);
	virtual const char * getName() const;
	virtual void update();
	virtual Value * getResult();
private:
	virtual Value * getSpecificParameters_();
private:
	Parameters parameters_;
	typedef CurveAoA Result;
	Result result_;
};

struct ParametersMxMy
{
	float aileron;
	float rudder;
	float bettaStep;
	float Cy;
};

struct CurvesMxMy
{
	Points Mx;
	Points My;
};

class TestMxMy : public Test
{
public:
	typedef ParametersMxMy Parameters;
	TestMxMy(ModelParameters * modelParameters, TestBase * testBase);
	virtual const char * getName() const;
	virtual void update();
	virtual Value * getResult();
private:
	virtual Value * getSpecificParameters_();
private:
	Parameters parameters_;
	typedef CurvesMxMy Result;
	Result result_;
};

struct ParametersOmegaX
{
	float altitude;
	float AoA;
	float Cy;
};

struct CurveOmegaX
{
	Curve curve;
	AirSpeedType airSpeedType;
};

class TestOmegaX : public Test
{
public:
	typedef ParametersOmegaX Parameters;
	TestOmegaX(ModelParameters * modelParameters, TestBase * testBase);
	virtual const char * getName() const;
	virtual void update();
	virtual Value * getResult();
private:
	virtual Value * getSpecificParameters_();
private:
	Parameters parameters_;
	typedef CurveOmegaX Result;
	Result result_;
};

struct ParametersMz
{
	float elevator;
	float AoAStep;
};

struct CurveMz
{
	Points Mz;
};

class TestMz : public Test
{
public:
	typedef ParametersMz Parameters;
	TestMz(ModelParameters * modelParameters, TestBase * testBase);
	virtual const char * getName() const;
	virtual void update();
	virtual Value * getResult();
private:
	virtual Value * getSpecificParameters_();
private:
	Parameters parameters_;
	typedef CurveMz Result;
	Result result_;
};

}

#endif _MathModelTestSFM_hpp_