#ifndef _ShapeGen_BodyInterceptor_hpp_
#define _ShapeGen_BodyInterceptor_hpp_

#include "ISystem.hpp"

#include "Actors/Actor.hpp"

#include <map>

#include "Math/Scalar.hpp"
#include "Math/Vec3.hpp"
#include "Core/ModelTime/ModelTime.hpp"
#include "Core/ModelTime/Activity.hpp"
#include "Database/Variant.hpp"

namespace Sim
{

using namespace Framework;

namespace ShapeGen
{

class Scene;
class Object;
class BodyObject;

class BodyInterceptor : public Actor
{
public:
	BodyInterceptor(const std::string & nameIn, ModelTime modelTime, const Variant & variant);
	~BodyInterceptor();
	static Actor * create(const std::string & nameIn, ModelTime modelTime, const Variant & variant);
	virtual void update(ModelTime modelTime, ModelTime dtime);
	void onProjectileDeath(BodyObject * projectile);
	bool getImpactPoint(BodyObject * bodyObject, ModelTime modelTime, ModelTime & targetTOF) const;
private:
	BodyObject * createDefendingArea_(ModelTime birthTime);
	BodyObject * createProjectile_(const Math::Vec3 & projectileVel, ModelTime birthTime);
	Math::Vec3 getProjectileSpawnPoint_() const;
private:
	class Activity : public Sim::PeriodicActivity
	{
	public:
		Activity(BodyInterceptor * bodyInterceptor, ModelTime dtime);
		virtual ModelTime operator()(ModelTime modelTime);
	private:
		BodyInterceptor * bodyInterceptor_;
	} activity_;

	BodyObject * defendingArea_;

	//parameters
	Math::Vec3 pos_;
	Math::Real projectileVelAbs_;
	Math::Real projectileSize_;
	Math::Real projectileDensity_;
	Math::Real detectionRadius_;
	Math::Real defendingRadius_;
	float defendingAreaColor_[4];
	Math::Real engagementRadius_;
	ModelTime firePeriod_;
	//state
	struct Shot
	{
		BodyObject*	projectile;
		ModelTime	hitTime;
	};
	typedef std::list<Shot> Shots;
	typedef std::map<const BodyObject*, Shots> Targets;
	Targets targets_;
	ModelTime lastShotTime_;
	float projectileColor_[4];
	
	unsigned int projectileCount_;
};

}

}

#endif _ShapeGen_BodyInterceptor_hpp_