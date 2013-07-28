//#include "stdafx.h"

#include "Debug/CheckValue.hpp"

#include "Modules/SFM/Object/JetEngineObject.hpp"

using namespace Sim;
using namespace Sim::SFM::JetEngine;

Object::Object(const Desc & desc) :	desc_(desc), state_(defaultState), cache_(defaultCache),
									thrustReq_(0.0f), thrust_(desc.acceleration, 0.0f),
									consumption_(0.0f), thrustRelative_(0.0f)
{
}

void Object::simulate(ModelTime dt, const Environment & env)
{
	assert(!state_.afterburner || desc_.hasAfterburner);

	float Pmax = getThrustMax(desc_, env, state_, cache_);
	float P = thrust_.simulate(thrustReq_ * Pmax, dt);
	thrustRelative_ = P / Pmax;
	validateScalar(P);
	
	float Ce = getCe(desc_, P / Pmax, env, state_, cache_);
	validateScalar(Ce);
	consumption_ = P * Ce;
}