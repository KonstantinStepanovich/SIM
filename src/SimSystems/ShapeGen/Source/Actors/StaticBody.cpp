#include "stdafx.h"

#include "Objects/BodyObject.hpp"
#include "Scene.hpp"
#include "Actors/StaticBody.hpp"

#include "Math/Vec3.hpp"
#include "Math/Quat.hpp"
#include "Database/Variant.hpp"
#include "Utils/VariantMathUtil.hpp"

using namespace Sim;
using namespace Sim::ShapeGen;

using Interfaces::IMass;

REGISER_ACTOR(StaticBody);

StaticBody::StaticBody(const std::string & nameIn, ModelTime modelTime, const Variant & variant)
{
	Math::Vec3 pos;
	load(variant.key("pos"), pos);	
	Math::Quat quat = Math::Quat::getIdentity();
	tryLoadQuat(variant, quat);
	
	IMass::Data mass = Interfaces::makeNoMass();	
	bodyObject_ = new BodyObject(nameIn, variant.key("typeId"), pos, Math::Vec3(0.0, 0.0, 0.0), quat, Math::Vec3(0.0, 0.0, 0.0), mass, modelTime);
	Scene::instance()->addObject(bodyObject_);
}

StaticBody::~StaticBody()
{
	Scene::instance()->removeObject(bodyObject_);
	delete bodyObject_;
}

Actor * StaticBody::create(const std::string & nameIn, ModelTime modelTime, const Variant & variant)
{
	return new StaticBody(nameIn, modelTime, variant);
}