#include "Utils/Projectile.hpp"

Projectile::Projectile(Scene * scene, const std::string & nameIn) : Actor(nameIn)
{
	scene->addObject(nameIn, "Body");
}

Projectile::~Projectile()
{
	if(bodyObject_ != NULL)
		delete bodyObject_;
}

void Projectile::update(Scene * scene, ModelTime modelTime, ModelTime dtime)
{
	if(modelTime > disappearTime_)
	{
		scene->killObject(bodyObject_);
		bodyObject_ = NULL;
	}
}

Object * Projectile::extend(Scene * scene, const ObjectName & objectName, const ObjectClass & objectClass, ModelTime modelTime)
{
	
}