#ifndef _TestJetEngine_hpp_
#define _TestJetEngine_hpp_

#include "Iteration.hpp"

#include "Modules/SFM/Model/JetEngine.hpp"

using namespace Sim;
using namespace Sim::SFM;

struct TestJetEngine
{
	JetEngine::Desc desc;
	IterationRange<float> alt;
	IterationRange<float> M;
	mutable JetEngine::Env env;
	JetEngine::State state;
};

typedef TestJetEngine TestP;

struct FuncPmax : std::unary_function<JetEngine::Env, float>
{
	FuncPmax(	const JetEngine::Desc & desc,
				const JetEngine::State & state);
	float operator()(const JetEngine::Env & env) const;
private:
	const JetEngine::Desc & desc_;
	const JetEngine::State & state_;
};

typedef TestJetEngine TestCe;

struct FuncCe : std::unary_function<JetEngine::Env, float>
{
	typedef float Result;
	FuncCe(	const JetEngine::Desc & desc,
			const JetEngine::State & state);
	float operator()(const JetEngine::Env & env) const;
private:
	const JetEngine::Desc & desc_;
	const JetEngine::State & state_;
};

template<class Exp>
struct FuncAltM : std::binary_function<float, float, float>
{
	FuncAltM(JetEngine::Env & env, Exp & exp) : env_(env), exp_(exp)
	{
	}
	float operator()(float alt, float M) const
	{
		env_.altKm = alt * 1.0E-3f;
		env_.M = M;
		return exp_(env_);
	}
private:
	Exp exp_;
	JetEngine::Env & env_;
};

#endif _TestJetEngine_hpp_