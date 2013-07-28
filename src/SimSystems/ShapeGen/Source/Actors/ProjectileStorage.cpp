#include "stdafx.h"

#include "Actors/Actor.hpp"
#include "Actors/World.hpp"
#include "Actors/ProjectileStorage.hpp"
#include "Actors/BodyInterceptor.hpp"
#include "Scene.hpp"

#include "Objects/BodyObject.hpp"

#include <algorithm>

using namespace Sim;
using namespace Sim::ShapeGen;

ProjectileStorage::Activity::Activity(ProjectileStorage * projectileStorage) : projectileStorage_(projectileStorage)
{
}

ModelTime ProjectileStorage::Activity::operator()(ModelTime modelTime)
{
	ModelTime nextTime = projectileStorage_->pop();
	assert(nextTime > modelTime || projectileStorage_->size() == 0);
	return nextTime;
}

REGISER_ACTOR(ProjectileStorage);

ProjectileStorage::ProjectileStorage() : activity_(this)
{
	World::instance().addActor(this);
}

void ProjectileStorage::release()
{
	activityScheduler.remove(&activity_);
	for(Projectiles::iterator itP = projectiles_.begin();
		itP != projectiles_.end();
		++itP)
	{
		Scene::instance()->removeObject(itP->bodyObject);
		delete itP->bodyObject;
	}
}

ProjectileStorage & ProjectileStorage::instance()
{
	static ProjectileStorage projectileStorage;
	return projectileStorage;
}

Actor * ProjectileStorage::create(const std::string & nameIn, ModelTime modelTime, const Variant & variant)
{
	return &ProjectileStorage::instance();
}

void ProjectileStorage::add(BodyObject * bodyObject, BodyInterceptor * bodyInterceptor, ModelTime disappearTime)
{
	const Projectile projectile = { bodyObject, bodyInterceptor, disappearTime };
	struct CompareProjectileDisappearTime
	{
		inline bool operator()(const Projectile & left, const Projectile & right) const
		{
			return left.disappearTime < right.disappearTime;
		}
	};
	Projectiles::iterator itP = std::lower_bound(projectiles_.begin(), projectiles_.end(), projectile, CompareProjectileDisappearTime());
	projectiles_.insert(itP, projectile);
	if(projectiles_.size() == 1)
		activityScheduler.schedule(disappearTime, &activity_);
}

ModelTime ProjectileStorage::pop()
{
	Scene::instance()->removeObject(projectiles_.front().bodyObject);
	projectiles_.front().bodyInterceptor->onProjectileDeath(projectiles_.front().bodyObject);
	projectiles_.pop_front();	
	return !projectiles_.empty() ? projectiles_.front().disappearTime : 0.0;
}