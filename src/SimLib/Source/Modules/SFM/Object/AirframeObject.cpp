#include "Modules/SFM/Object/AirframeObject.hpp"

using namespace Sim;
using namespace Sim::SFM;
using namespace Sim::SFM::Airframe;

//-=Airframe=-

Object::Object(const Desc & desc, State & state) :	desc_(desc),
													state_(state),
													cache_(defaultCache),
													force_(0.0f, 0.0f, 0.0f),
													torque_(0.0f, 0.0f, 0.0f)
{
}

void Object::simulate(ModelTime dt, const Env & env)
{
	calcForceTorque(desc_, state_, env, cache_, force_, torque_);
}