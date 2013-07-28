#ifndef _ShapeGen_ProjectileStorage_hpp_
#define _ShapeGen_ProjectileStorage_hpp_

#include "Actors/Actor.hpp"

#include "Math/Box3.hpp"
#include "Core/ModelTime/Activity.hpp"

#include <list>

namespace Sim
{

namespace ShapeGen
{

class BodyObject;
class BodyInterceptor;

class ProjectileStorage : public Actor
{
	ProjectileStorage();
	virtual void release();
public:
	static ProjectileStorage & instance();
	static Actor * create(const std::string & nameIn, ModelTime modelTime, const Variant & variant);
	inline size_t size() const { return projectiles_.size(); }
	void add(BodyObject * bodyObject, BodyInterceptor * bodyInterceptor, ModelTime disappearTime);
	ModelTime pop();
private:
	class Activity : public Sim::Activity
	{
	public:
		Activity(ProjectileStorage * projectileStorage);
		virtual ModelTime operator()(ModelTime modelTime);
	private:
		ProjectileStorage * projectileStorage_;
	} activity_;
	struct Projectile
	{
		BodyObject * bodyObject;
		BodyInterceptor * bodyInterceptor;
		ModelTime disappearTime;
	};
	typedef std::list<Projectile> Projectiles;
	Projectiles projectiles_;
};

}

}

#endif _ShapeGen_ProjectileStorage_hpp_