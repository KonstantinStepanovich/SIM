#include "stdafx.h"

#include "Actors/Airplane.hpp"
#include "Actors/StandardAtmosphere.hpp"
#include "Objects/AirplaneObject.hpp"
#include "Scene.hpp"

#include "Events.hpp"

#include "Math/Vec3.hpp"
#include "Math/Quat.hpp"
#include "Core/ModelTime/Activity.hpp"
#include "Database/Variant.hpp"
#include "Database/ParserJson.hpp"
#include "Database/DatabaseUtil.hpp"
#include "Utils/VariantMathUtil.hpp"
#include "Utils/Units.hpp"

#include "Modules/SFM/Object/AirplaneObject.hpp"
#include "Modules/SFM/Model/Utils.hpp"

using namespace Sim;
using namespace Sim::ShapeGen;

class Airplane::Activity : public PeriodicActivity
{
public:
	Activity(Airplane * airplane, ModelTime dtime) : airplane_(airplane), PeriodicActivity(dtime) {;}
private:
	virtual ModelTime operator()(ModelTime time)
	{
		airplane_->simulate(getDTime());
		return PeriodicActivity::operator()(time);
	}
	Airplane * airplane_;
};

namespace Sim
{

	namespace ShapeGen
	{

		void loadPayload(const Variant & variant, SFM::Payload::Index index, SFM::Airplane::Object & airplane, SFM::PayloadTree::iterator it)
		{
			SFM::Payload payload;
			payload.typeId = variant.key("typeId").getString();
			payload.index = index;
			payload.ammount.count = 1;
			if(const Variant childs = variant.tryKey("childs"))
			{
				it = airplane.addPayload(it, payload);
				for(Variant::ConstIterator itV = childs.begin();
					itV != childs.end();
					++itV)
				{
					it.up();
					loadPayload(*itV, itV.index(), airplane, it);
					it.down();
				}
			}
			else if(const Variant child = variant.tryKey("child"))
			{
				it = airplane.addPayload(it, payload);
				it.up();	
				loadPayload(child, SFM::Payload::INDEX_NULL, airplane, it);
				it.down();
			}
			else if(const Variant count = variant.tryKey("count"))
			{
				payload.ammount.count = count;
				it = airplane.addPayload(it, payload);
			}
			else
			{
				payload.ammount.mass = variant.key("mass");
				it = airplane.addPayload(it, payload);
			}
		}
	}
}

static void initDatabase()
{
	static LoaderFile<SFM::Airplane::Desc> loaderFileAirplane("SFM/Airplanes/", ".js", &loadJson, "");
	SFM::Airplane::Database::instance().setLoader(&loaderFileAirplane);
	static LoaderFile<SFM::Airframe::Desc> loaderFileAirframe("SFM/Airframes/", ".js", &loadJson, "");
	SFM::Airframe::Database::instance().setLoader(&loaderFileAirframe);
	static LoaderFile<SFM::JetEngine::Desc> loaderFileJetEngine("SFM/JetEngines/", ".js", &loadJson, "");
	SFM::JetEngine::Database::instance().setLoader(&loaderFileJetEngine);
}

REGISER_ACTOR2(Airplane, &initDatabase);

Airplane::Airplane(const std::string & nameIn, ModelTime modelTime, const Variant & variant)
{
	Math::Vec3 position, velocity;
	tryLoadFromTable(variant, "pos", position);
	tryLoadFromTable(variant, "vel", velocity);
	
	Math::Quat rotation;
	Math::Vec3 angularVelocity;
	tryLoadQuat(variant, rotation);
	tryLoadFromTable(variant, "angularVel", angularVelocity);

	TypeId typeId = variant.key("typeId").getString();
	const SFM::Airplane::Desc & desc = SFM::Airplane::Database::instance().getDesc(typeId);
	airplane_.reset(new SFM::Airplane::Object(desc));

	float relFuel = variant.key("fuel");
	airplane_->setFuel(desc.fuelSystem.internalFuelMax * relFuel);

	if(Variant payload = variant.tryKey("payload"))
		loadPayload(payload, 0, *airplane_.get(), airplane_->getPayloadRoot());

	ModelTime dtime = 0.02;
	if(const Variant dtimeV = variant.tryKey("dtime"))
		dtime = dtimeV;
	activity_.reset(new Activity(this, dtime));
	activityScheduler.scheduleNow(activity_.get());

	Math::Vec3 forceCoeffs;
	if(!tryLoadFromTable(variant, "forceCoeffs", forceCoeffs))
		std::fill(forceCoeffs.m_floats, forceCoeffs.m_floats + 3, 1.0);
	
	Math::Vec3 torqueCoeffs;
	if(!tryLoadFromTable(variant, "torqueCoeffs", torqueCoeffs))
		std::fill(forceCoeffs.m_floats, torqueCoeffs.m_floats + 3, 1.0);
	StandardAtmosphere::instance().get().setPoint(position, atmPoint_);
	
	object = new ObjectAirplane(nameIn, modelTime, typeId,
								position, velocity, rotation, angularVelocity,
								*airplane_, forceCoeffs, torqueCoeffs);
	Scene::instance()->addObject(object);
}

Airplane::~Airplane()
{
	activityScheduler.remove(activity_.get());
	Scene::instance()->removeObject(object);
	delete object;
}

Actor * Airplane::create(const std::string & nameIn, ModelTime modelTime, const Variant & variant)
{
	return new Airplane(nameIn, modelTime, variant);
}

template<typename T>
struct Parameter
{
	Parameter(const T valueIn, UnitId unitId) : value(valueIn), unit(UnitManager::instance().getUnit(unitId)) {;}
	const T value;
	const Unit & unit;
};

template<typename T>
inline Parameter<T> makeParameter(const T valueIn, UnitId unitId)
{
	return Parameter<T>(valueIn, unitId);
}

template<class _Traits, typename T> inline
std::basic_ostream<char, _Traits>& __CLRCALL_OR_CDECL operator<<(
	std::basic_ostream<char, _Traits>& _Ostr,
	Parameter<T> & parameter)
{
	return _Ostr << parameter.value * parameter.unit.coeff << " " << parameter.unit.name;
}

void Airplane::simulate(ModelTime dtime)
{
	Interfaces::IPosition::Data position;
	object->getPosition(position);
	Interfaces::IRotation::Data rotation;
	object->getRotation(rotation);

	StandardAtmosphere::instance().get().updatePoint(position.position, atmPoint_);
	AtmParam atmParam;
	StandardAtmosphere::instance().get().getParam(atmPoint_, AtmParam::PARAM_ALL, atmParam);
	SFM::Airplane::Object::Environment env;
	Math::Vec3 TAS = position.velocity - StandardAtmosphere::instance().get().getWind(atmPoint_);
	Math::Vec3 TASloc = quatRotate(rotation.rotation.inverse(), TAS);
	env.M = position.velocity.length() / atmParam.Vs;
	env.P = atmParam.P;
	env.T = atmParam.T;
	env.rho = atmParam.rho;	
	env.TAS = TASloc;
	env.omega = quatRotate(rotation.rotation.inverse(), rotation.angularVelocity);

	airplane_->simulate(dtime, env);

	std::ostringstream stream;
	stream	<< "Type: " << object->getTypeId() << std::endl
		<< "V: " << makeParameter(position.velocity.length(), UNIT_VELOCITY) << std::endl
		<< "M: " << position.velocity.length() / atmParam.Vs << std::endl
		<< "Vy: " << makeParameter(position.velocity.y(), VERTICAL_VELOCITY) << std::endl
		<< "H: " << makeParameter(position.position.y(), UNIT_SIZE) << std::endl
		<< "AoA: " << makeParameter(SFM::getAoA(TASloc), UNIT_ANGLE) << std::endl
		<< "AoS: " << makeParameter(-SFM::getAoS(TASloc), UNIT_ANGLE) << std::endl
		<< "Mass: " << makeParameter(airplane_->getMass(), UNIT_MASS) << std::endl
		<< "Fuel: " << makeParameter(airplane_->getFuelMass(), UNIT_MASS) << "("
		<< 100 * airplane_->getFuelMass() / airplane_->getDesc().fuelSystem.internalFuelMax << "%)" << std::endl
		<< "Thrust: " << 100.0 * airplane_->getThrustRelative() << " %, " <<
		makeParameter(airplane_->getThrust(), UNIT_THRUST) <<
		(airplane_->isAfterburnerEngaged() ? ", Afterburner" : "");
	object->setDebugState(stream.str());

	object->update();
}