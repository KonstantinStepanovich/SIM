#include "stdafx.h"

#include "Actors/BodyInterceptor.hpp"
#include "Actors/MovingBodyStorage.hpp"
#include "Actors/Physics.hpp"
#include "Actors/ProjectileStorage.hpp"
#include "Utils/MassCalculator.hpp"
#include "Objects/BodyObject.hpp"
#include "Scene.hpp"

#include "Implementation/Mass.hpp"

#include "Math/Scalar.hpp"
#include "Math/Equation.hpp"
#include "Math/NonLinearSystemOfEquationSolver.hpp"
#include "Debug/CheckMath.hpp"
#include "Utils/VariantMathUtil.hpp"

using namespace Sim;
using namespace Sim::ShapeGen;
using namespace Sim::Interfaces;

const Math::Real protectingAreaHeight = 1.0;

static bool getImpactPoint(const IPosition::Data & position, Math::Real y0,
						   Math::Vec3 & IP, ModelTime & TOF)
{
	Math::Real dt[2];
	unsigned int valCount = Math::evalSqrEquation(position.acceleration.y() / 2, position.velocity.y(), position.position.y() - y0, dt);
	if(valCount > 0)
	{
		TOF = dt[0];
		if(	valCount > 1 &&
			TOF < 0.0)
			TOF = dt[1];
		if(TOF < 0.0)
			return false;
		IP = position.position + position.velocity * TOF + position.acceleration * TOF * TOF / 2;
		return TOF > 0.0;
	}
	else
		return false;
}

static void advance(IPosition::Data & position, ModelTime dtime)
{
	position.velocity += position.acceleration * dtime;
	position.position += position.velocity * dtime + position.acceleration * pow(dtime, 2) / 2;
}

static Math::Vec3 move(IPosition::Data & position, ModelTime dtime)
{
	return position.position + position.velocity * dtime + position.acceleration * pow(dtime, 2) / 2;
}

static Math::Vec3 projection(const Math::Vec3 & vec, const Math::Vec3 & norm)
{
	const Math::Vec3 p = norm.cross(vec).normalized();
	const Math::Vec3 q = norm.cross(p);
	return p * p.dot(vec) + q * q.dot(vec);
}

static Math::Vec3 reflect(const Math::Vec3 & vec, const Math::Vec3 & norm)
{
	const Math::Vec3 t = norm.cross(vec.cross(norm));
	const Math::Vec3 p = -norm.dot(vec) * norm;
	return t + p;
}

inline void bounce(Math::Real m1, Math::Real v1, Math::Real m2, Math::Real v2,
				   Math::Real & u1, Math::Real & u2)
{
	u1 = ((m1 - m2) * v1 + 2 * m2 * v2) / (m1 + m2);
	u2 = (2 * m1 * v1 + (m2 - m1) * v2) / (m1 + m2);
}

static void bounce(Math::Real m1, Math::Real v1, Math::Real m2, Math::Real v2, Math::Real r,
				   Math::Real & u1, Math::Real & u2)
{
	u1 = (m1 * v1 + m2 * v2 + m2 * r * (v2 - v1)) / (m1 + m2);
	u2 = (m1 * v1 + m2 * v2 + m1 * r * (v1 - v2)) / (m1 + m2);
}

static void bounce(Math::Real targetMass, const IPosition::Data & targetPosition,
				   Math::Real projectileMass, const IPosition::Data & projectilePosition,
				   Math::Vec3 & targetVel, Math::Vec3 & projectileVel)
{
	Math::Vec3 up(0.0, 1.0, 0.0);
	Math::Matrix3x3 basis;
	basis[0] = (targetPosition.position - projectilePosition.position).normalized();
	basis[2] = basis[0].cross(up);
	basis[1] = basis[2].cross(basis[0]);

	Math::Vec3 targetVelLoc = basis * targetPosition.velocity;
	Math::Vec3 projectileVelLoc = basis * projectilePosition.velocity;

	Math::Real targetVelX, projectileVelX;
	bounce(	targetMass, targetVelLoc.x(), projectileMass, projectileVelLoc.x(), 0.1,
			targetVelX, projectileVelX);
	targetVelLoc.setX(targetVelX);
	projectileVelLoc.setX(projectileVelX);
	targetVel = basis.inverse() * targetVelLoc;
	projectileVel = basis.inverse() * projectileVelLoc;
}

static bool aimTarget(const IPosition::Data & targetPosition, Math::Real targetRadius,
					  const Math::Vec3 & pos, Math::Real projectileVelAbs, Math::Real projectileRadius, 
					  const Math::Vec3 & gravity,
					  Math::Vec3 & projectileVel, ModelTime & TOF)
{
	Math::Vec3 dir = targetPosition.position - pos;
	projectileVel = dir.normalize() * projectileVelAbs;
	Math::Real t = dir.length() / projectileVelAbs;

	struct Aiming : public Math::NonLinearSystemOfEquations<double, 4>
	{
		IPosition::Data targetPosition;
		Math::Real		projectileVelAbs;
		Math::Vec3		gravity;

		inline bool eval(const Arg & arg, Val & val) const
		{
			double Vx = arg[0];
			double Vy = arg[1];
			double Vz = arg[2];
			const Math::Vec3 V(Vx, Vy, Vz);

			double t = arg[3];
			const Math::Vec3 vec =	(targetPosition.position + targetPosition.velocity * t + targetPosition.acceleration * pow(t, 2) / 2) -
									(V * t + gravity * pow(t, 2) / 2);
			val[0] = vec.x();
			val[1] = vec.y();
			val[2] = vec.z();
			val[3] = V.length() - projectileVelAbs;

			return true;
		}

		inline bool fillJacob(const Arg & arg, Jacob & J) const
		{
			double Vx = arg[0];
			double Vy = arg[1];
			double Vz = arg[2];
			double t = arg[3];

			J[0 * 4 + 0] = -t;
			J[0 * 4 + 1] = 0.0;
			J[0 * 4 + 2] = 0.0;
			J[0 * 4 + 3] = targetPosition.velocity.x() + targetPosition.acceleration.x() * t - Vx;

			J[1 * 4 + 0] = 0.0;
			J[1 * 4 + 1] = -t;
			J[1 * 4 + 2] = 0.0;
			J[1 * 4 + 3] = targetPosition.velocity.y() + targetPosition.acceleration.y() * t - Vy;

			J[2 * 4 + 0] = 0.0;
			J[2 * 4 + 1] = 0.0;
			J[2 * 4 + 2] = -t;
			J[2 * 4 + 3] = targetPosition.velocity.z() + targetPosition.acceleration.z() * t - Vz;

			double V = sqrt(pow(Vx, 2) + pow(Vy, 2) + pow(Vz, 2));
			J[3 * 4 + 0] = Vx / V;
			J[3 * 4 + 1] = Vy / V;
			J[3 * 4 + 2] = Vz / V;
			J[3 * 4 + 3] = 0.0;

			return true;
		}
	};

	Aiming aiming;
	aiming.targetPosition = targetPosition;
	aiming.targetPosition.position -= pos;
	aiming.projectileVelAbs = projectileVelAbs;
	aiming.gravity = gravity;

	Aiming::Val accuracy = { 0.5f * targetRadius, 0.5f * targetRadius, 0.5f * targetRadius, 0.5f * targetRadius }; 
	Aiming::Arg arg = { projectileVel.x(), projectileVel.y(), projectileVel.z(), t };

	if(Sim::Math::solveNonLinearSystemOfEquations(aiming, accuracy, arg))
	{
		projectileVel.setX(arg[0]);
		projectileVel.setY(arg[1]);
		projectileVel.setZ(arg[2]);
		TOF = arg[3];

		IPosition::Data targetPositionAtCollision = targetPosition;
		advance(targetPositionAtCollision, TOF);

		IPosition::Data projectilePositionAtCollision;
		projectilePositionAtCollision.position = pos;
		projectilePositionAtCollision.velocity = projectileVel;
		projectilePositionAtCollision.acceleration = gravity;
		advance(projectilePositionAtCollision, TOF);

		const Math::Vec3 dir = targetPositionAtCollision.position - projectilePositionAtCollision.position;
		const Math::Real dist = dir.length();
		const Math::Vec3 relVel = projectilePositionAtCollision.velocity - targetPositionAtCollision.velocity;
		const Math::Real relVelAbs = relVel.length();

		const Math::Real x = (relVel / relVelAbs).dot(dir);
		const Math::Real y2 = dist * dist - x * x;
		const Math::Real x0 = sqrt(pow(targetRadius + projectileRadius, 2) - y2);
		ModelTime dTOF = -(x0 - x) / relVelAbs;
		TOF += dTOF;
		assert(TOF > 0.0);
		assert(TOF < 5.0);
		return true;
	}
	return false;
}

REGISER_ACTOR(BodyInterceptor);

BodyInterceptor::Activity::Activity(BodyInterceptor * bodyInterceptor, ModelTime dtime) : PeriodicActivity(dtime), bodyInterceptor_(bodyInterceptor)
{
}

ModelTime BodyInterceptor::Activity::operator()(ModelTime modelTime)
{
	bodyInterceptor_->update(modelTime, getDTime());
	return PeriodicActivity::operator ()(modelTime);
}

BodyInterceptor::BodyInterceptor(const std::string & nameIn, ModelTime modelTime, const Variant & variant) : Actor(variant),
	activity_(this, 0.5), lastShotTime_(-std::numeric_limits<ModelTime>::infinity()), projectileCount_(0u)
{
	name = nameIn;
	::load(variant.key("pos"), pos_);
	projectileVelAbs_ = variant.key("projectileVelAbs");
	projectileSize_ = variant.key("projectileSize");
	projectileDensity_ = variant.key("projectileDensity");
	detectionRadius_ = variant.key("detectionRadius");
	defendingRadius_ = variant.key("defendingRadius");
	load(variant.key("defendingAreaColor"), defendingAreaColor_);
	engagementRadius_ = variant.key("engagementRadius");
	firePeriod_ = 1.0 / (Math::Real)variant.key("fireRate");
	load(variant.key("projectileColor"), projectileColor_);	

	defendingArea_ = createDefendingArea_(modelTime);
	Scene::instance()->addObject(defendingArea_);

	activityScheduler.scheduleNow(&activity_);
}

BodyInterceptor::~BodyInterceptor()
{
	activityScheduler.remove(&activity_);
	Scene::instance()->removeObject(defendingArea_);
	delete defendingArea_;
}

Actor * BodyInterceptor::create(const std::string & nameIn, ModelTime modelTime, const Variant & variant)
{
	return new BodyInterceptor(nameIn, modelTime, variant);
}

void BodyInterceptor::update(ModelTime modelTime, ModelTime dtime)
{
	Targets::iterator itT = targets_.begin();
	while(itT != targets_.end())
	{
		Shots & shots = itT->second;
		while(	!shots.empty() &&
				modelTime > shots.back().hitTime + 0.5)
			shots.pop_back();
		if(shots.empty())
			itT = targets_.erase(itT);
		else
			++itT;
	}

	if(modelTime - lastShotTime_ < firePeriod_)
		return;

	BodyObject * target = NULL;
	struct FindMainTarget
	{
		FindMainTarget(BodyInterceptor * bodyInterceptorIn, ModelTime modelTimeIn, BodyObject * & mainTargetIn) : bodyInterceptor(bodyInterceptorIn),
			modelTime(modelTimeIn), mainTarget(mainTargetIn), targetTOFMin(std::numeric_limits<ModelTime>::infinity())
		{
		}
		inline bool operator()(BodyObject * bodyObject)
		{
			ModelTime targetTOF;
			if(bodyInterceptor->getImpactPoint(bodyObject, modelTime, targetTOF))
			{
				mainTarget = bodyObject;
				targetTOFMin = targetTOF;
				return true;
			}
			else
				return false;
		}
		BodyInterceptor *	bodyInterceptor;
		ModelTime			modelTime;
		ModelTime			targetTOFMin;
		BodyObject * &		mainTarget;
	} findMainTarget(this, modelTime, target);

	MovingBodyStorage::instance().iterate(findMainTarget);

	if(target != NULL)
	{
		ModelTime TOF;
		IPosition::Data targetPosition;
		target->getPosition(targetPosition);
		targetPosition.acceleration = Physics::instance().gravity;
		const Math::Box3 obb = target->getOBB();
		targetPosition.position += obb.getCenter();
		const Math::Real targetRadius = obb.getRadius();
		Math::Vec3 projectileVel;
		if(aimTarget(	targetPosition, targetRadius, getProjectileSpawnPoint_(),
						projectileVelAbs_, projectileSize_ / 2, Physics::instance().gravity,
						projectileVel, TOF))
		{
			IPosition::Data projectilePosition;
			projectilePosition.position = pos_;
			projectilePosition.velocity = projectileVel;
			projectilePosition.acceleration = Physics::instance().gravity;
			advance(projectilePosition, TOF);

			Math::Vec3 IP;
			ModelTime TOFl;
			if(	::getImpactPoint(projectilePosition, pos_.y() + protectingAreaHeight / 2, IP, TOFl) &&
				(IP - Math::Vec3(pos_.x(), IP.y(), pos_.z())).length2() < pow(defendingRadius_ + projectileSize_ / 2, 2) &&
				TOFl < TOF)
				return;

			validate(projectileVel);

			BodyObject * projectile = createProjectile_(projectileVel, modelTime);
			Scene::instance()->addObject(projectile);
			ProjectileStorage::instance().add(projectile, this, modelTime + TOF + 0.5);
			Shot shot;
			shot.projectile = projectile;
			shot.hitTime = modelTime + TOF;
			targets_[target].push_back(shot);
			lastShotTime_ = modelTime;
		}
	}
}

void BodyInterceptor::onProjectileDeath(BodyObject * projectile)
{
	Targets::iterator itT = targets_.begin();
	while(itT != targets_.end())
	{
		Shots & shots = itT->second;
		Shots::iterator itS = shots.begin();
		while(itS != shots.end())
		{
			if(itS->projectile == projectile)
				itS = shots.erase(itS);
			else
				++itS;
		}
		if(shots.empty())
			itT = targets_.erase(itT);
		else
			++itT;
	}
}

bool BodyInterceptor::getImpactPoint(BodyObject * bodyObject, ModelTime modelTime, ModelTime & targetTOF) const
{
	IMass::Data targetMass;
	bodyObject->getMass(targetMass);
	IPosition::Data targetPosition;
	bodyObject->getPosition(targetPosition);
	targetPosition.acceleration = Physics::instance().gravity;
	const Math::Real dist = (targetPosition.position - pos_).length();
	if(	dist < detectionRadius_ &&
		dist < engagementRadius_)
	{
		ModelTime prevModelTime = modelTime;
		Targets::const_iterator itT = targets_.find(bodyObject);
		if(itT != targets_.end())
		{
			const Shots & shots = itT->second;
			for(Shots::const_iterator itS = shots.begin();
				itS != shots.end();
				++itS)
			{
				ModelTime dtime = itS->hitTime - prevModelTime;
				if(dtime < 0.0)
					continue;
				advance(targetPosition, dtime);					
				IMass::Data projectileMass;
				itS->projectile->getMass(projectileMass);					
				IPosition::Data projectilePosition;
				itS->projectile->getPosition(projectilePosition);
				projectilePosition.acceleration = Physics::instance().gravity;
				advance(projectilePosition, dtime);
				Math::Vec3 targetVel, projectileVel;
				bounce(	targetMass.mass, targetPosition,
					projectileMass.mass, projectilePosition,						
					targetVel, projectileVel);
				targetPosition.velocity = targetVel;
			}
		}
		const Math::Box3 obb = bodyObject->getOBB();
		targetPosition.position += obb.getCenter();
		Math::Vec3 IP;
		ModelTime TOF;
		const Math::Real targetRadius = obb.getRadius();
		if(::getImpactPoint(	targetPosition, pos_.y() + protectingAreaHeight / 2, IP, TOF) &&
								(IP - Math::Vec3(pos_.x(), IP.y(), pos_.z())).length2() < pow(defendingRadius_ + targetRadius, 2))
		{
			return true;
		}
	}
	return false;
}

BodyObject * BodyInterceptor::createDefendingArea_(ModelTime birthTime)
{
	Math::Quat rot = Math::Quat::getIdentity();
	rot.setRotation(Math::Vec3(1.0, 0.0, 0.0), 1.57f);
	BodyObject * defendingArea = new BodyObject("Defending area - " + name, "Cylinder",
												pos_, Math::Vec3(0.0, 0.0, 0.0),
												rot, Math::Vec3(0.0f, 0.0f, 0.0f),
												Interfaces::makeNoMass(),
												birthTime);
	for(int c = 0; c < 3; ++c)
		defendingArea->addArg(c + 3, defendingAreaColor_[c]);

	defendingArea->addArg(0, defendingRadius_);
	defendingArea->addArg(1, 1.0f);
	return defendingArea;
}

BodyObject * BodyInterceptor::createProjectile_(const Math::Vec3 & projectileVel, ModelTime birthTime)
{
	const IMass::Data mass = makeSphereMass(projectileDensity_, projectileSize_ / 2);
	BodyObject * projectile = new BodyObject(	OSS("Projectile - " << name << projectileCount_++), "Sphere",
												getProjectileSpawnPoint_(), projectileVel,
												Math::Quat::getIdentity(), Math::Vec3(0.0, 0.0, 0.0),
												mass,
												birthTime);

	for(int c = 0; c < 3; ++c)
		projectile->addArg(c + 3, projectileColor_[c]);

	projectile->addArg(0, projectileSize_ / 2);
	return projectile;
}

Math::Vec3 BodyInterceptor::getProjectileSpawnPoint_() const
{
	return Math::Vec3(pos_.x(), pos_.y() + 2 * projectileSize_, pos_.z());
}