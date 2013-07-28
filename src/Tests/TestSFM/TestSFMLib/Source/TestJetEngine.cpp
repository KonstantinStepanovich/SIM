#include "stdafx.h"

#include "TestJetEngine.hpp"

#include "Modules/SFM/Model/JetEngine.hpp"

FuncPmax::FuncPmax(	const JetEngine::Desc & desc,
					const JetEngine::State & state) : desc_(desc), state_(state)
{
}

float FuncPmax::operator()(const JetEngine::Env & env) const
{
	JetEngine::Cache cache = JetEngine::defaultCache;
	return getThrustMax(desc_, env, state_, cache);
}

FuncCe::FuncCe(	const JetEngine::Desc & desc,
				const JetEngine::State & state) : desc_(desc), state_(state)
{

}

float FuncCe::operator()(const JetEngine::Env & env) const
{
	JetEngine::Cache cache = JetEngine::defaultCache;
	return getCe(desc_, env, state_, cache);
}
