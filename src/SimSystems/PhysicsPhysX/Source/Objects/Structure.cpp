#include "stdafx.h"

#include "Objects/Structure.hpp"
#include "Scene.hpp"
#include "Utils/MultiGeometryBody.hpp"

#include "Interface/IEntity.hpp"
#include "Interface/IDynamicBody.hpp"
#include "Interface/IMass.hpp"
#include "Events.hpp"

#include "Database/Variant.hpp"
#include "Database/ParserLua.hpp"

#include <algorithm>

#include "PhysXAPI/PxRigidDynamic.h"

using namespace Sim;
using namespace Sim::Framework;
using namespace Sim::Interfaces;
using namespace Sim::PhysicsPhysX;

Structure::Structure(const ObjectName & objectName, Scene * scene) : Object(objectName, scene)
{
}

Structure::~Structure()
{
}

const ObjectClass & Structure::getClass() const
{
	static const ObjectClass objectClass("Structure");
	return objectClass;
}

Event Structure::getEventsOfInterest() const
{
	return Object::getEventsOfInterest() | EVENT_TYPE_ID_CHANGED;
}

void Structure::onEvent(ISubject * subject, Event event)
{
	if(event & EVENT_TYPE_ID_CHANGED)
	{
		const TypeId & typeId = dynamic_cast<IEntity*>(subject)->getTypeId();
		VariantRoot variant;
		loadLua(("FlightModel/Database/Models/" + typeId + ".lua").c_str(), "physic", variant);
		load(variant);
	}
	if(event & EVENT_POSITION_CHANGED)
	{
		bodies_[mainBodyIndex_]->getBody().onPositionChanged(dynamic_cast<IPosition*>(subject));	
	}
	if(event & EVENT_ROTATION_CHANGED)
	{
		bodies_[mainBodyIndex_]->getBody().onRotationChanged(dynamic_cast<IRotation*>(subject));
	}
	if(event & EVENT_DYNAMIC_BODY_CHANGED)
	{
		bodies_[mainBodyIndex_]->getBody().onDynamicChanged(dynamic_cast<IDynamicBody*>(subject));
	}
	if(event & EVENT_MASS_CHANGED)
	{
		bodies_[mainBodyIndex_]->getBody().onMassChanged(dynamic_cast<IMass*>(subject));
	}
}

void Structure::getPosition(IPosition::Data & data) const
{
	bodies_[mainBodyIndex_]->getBody().getPosition(scene_->getModelTime(), data);
}

void Structure::getRotation(IRotation::Data & data) const
{
	bodies_[mainBodyIndex_]->getBody().getRotation(scene_->getModelTime(), data);
}

void Structure::onBodyMove(const Body * body)
{
	if(!body->isKinematic())
		notifyEvent(EVENT_POSITION_CHANGED | EVENT_ROTATION_CHANGED);
}

void Structure::update()
{
	std::for_each(bodies_.begin(), bodies_.end(), std::mem_fun(&MultiGeometryBody::update));
}

void Structure::load(const Variant & variant)
{
	mainBodyIndex_ = variant.key("mainBodyIndex");
	if(const Variant vBodies = variant.tryKey("bodies"))
	{
		bodies_.resize(vBodies.size());
		for(Variant::ConstIterator itV = vBodies.begin();
			itV != vBodies.end();
			++itV)
		{
			MultiGeometryBody * multiGeometryBody = new MultiGeometryBody(this, scene_->getSDK(), scene_->get());
			multiGeometryBody->load(*itV);
			bodies_[itV.index()] = multiGeometryBody;
		}
	}
}

physx::PxActor * Structure::getID()
{
	return bodies_[mainBodyIndex_]->getBody().get();
}