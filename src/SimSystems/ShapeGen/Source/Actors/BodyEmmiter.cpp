#include "stdafx.h"

#include "Actors/BodyEmmiter.hpp"
#include "Actors/MovingBodyStorage.hpp"
#include "Objects/BodyObject.hpp"
#include "Scene.hpp"
#include "Utils/MassCalculator.hpp"

#include "Math/Scalar.hpp"
#include "Database/Variant.hpp"
#include "Utils/VariantMathUtil.hpp"

using namespace Sim;
using namespace Sim::Framework;
using namespace Sim::Interfaces;
using namespace Sim::ShapeGen;

REGISER_ACTOR(BodyEmmiter);

template<typename T>
inline T rand()
{
	return (T)rand() / RAND_MAX; //uniform random 0..1
}

inline Math::Vec3 rndVec3()
{
	return Math::Vec3(rand<btScalar>(), rand<btScalar>(), rand<btScalar>());
}

inline void rndVec4(float (&v)[4])
{
	v[0] = rand<btScalar>();
	v[1] = rand<btScalar>();
	v[2] = rand<btScalar>();
	v[3] = rand<btScalar>();
}

class BodyType
{
public:
	BodyType()
	{
	}
	virtual BodyObject * createBody(const ObjectName & objectName,
									const Math::Vec3 & position, const Math::Vec3 & velocity,
									const Math::Quat & rotation, const Math::Vec3 & angularVelocity,
									float minSize, float maxSize,
									float density, ModelTime birthTime) = 0;
};

class BodyTypeSphere : public BodyType
{
public:
	BodyTypeSphere()
	{
	}
	virtual BodyObject * createBody(const ObjectName & objectName,
									const Math::Vec3 & position, const Math::Vec3 & velocity,
									const Math::Quat & rotation, const Math::Vec3 & angularVelocity,
									float minSize, float maxSize,
									float density, ModelTime birthTime)
	{
		const Math::Real radius = minSize + (maxSize - minSize) * rand<float>();
		IMass::Data mass = makeSphereMass(density, radius);
		BodyObject * bodyObject = new BodyObject(	objectName, "Sphere",
													position, velocity,
													rotation, angularVelocity,
													mass, birthTime);
		bodyObject->addArg(0, radius);
		return bodyObject;
	}
};

class BodyTypeBox : public BodyType
{
public:
	BodyTypeBox()
	{		
	}
	virtual BodyObject * createBody(const ObjectName & objectName,
									const Math::Vec3 & position, const Math::Vec3 & velocity,
									const Math::Quat & rotation, const Math::Vec3 & angularVelocity,
									float minSize, float maxSize,
									float density, ModelTime birthTime)
	{
		const Math::Vec3 size(	minSize + (maxSize - minSize) * rand<float>(),
								minSize + (maxSize - minSize) * rand<float>(),
								minSize + (maxSize - minSize) * rand<float>());
		IMass::Data mass = makeBoxMass(density, size);
		BodyObject * bodyObject = new BodyObject(	objectName, "Box",
													position, velocity,
													rotation, angularVelocity,
													mass, birthTime);
		bodyObject->addArg(0, size.x());
		bodyObject->addArg(1, size.y());
		bodyObject->addArg(2, size.z());
		return bodyObject;
	}
};

class BodyTypeCylinder : public BodyType
{
public:
	BodyTypeCylinder()
	{		
	}
	virtual BodyObject * createBody(const ObjectName & objectName,
									const Math::Vec3 & position, const Math::Vec3 & velocity,
									const Math::Quat & rotation, const Math::Vec3 & angularVelocity,
									float minSize, float maxSize,
									float density, ModelTime birthTime)
	{
		const Math::Real radius = minSize + (maxSize - minSize) * rand<float>();
		const Math::Real height = minSize + (maxSize - minSize) * rand<float>();
		IMass::Data mass = makeCylinderMass(density, radius, height);
		BodyObject * bodyObject = new BodyObject(	objectName, "Cylinder",
													position, velocity,
													rotation, angularVelocity,
													mass, birthTime);
		bodyObject->addArg(0, radius);
		bodyObject->addArg(1, height);
		return bodyObject;
	}
};

class RandomBodyGenerator
{
	RandomBodyGenerator()
	{
		static BodyTypeSphere bodyTypeSphere;
		bodyTypes_.push_back(&bodyTypeSphere);
		static BodyTypeBox bodyTypeBox;
		bodyTypes_.push_back(&bodyTypeBox);
		static BodyTypeCylinder bodyTypeCylinder;
		bodyTypes_.push_back(&bodyTypeCylinder);
	}
public:
	static RandomBodyGenerator & instance()
	{
		static RandomBodyGenerator randomBodyGenerator;
		return randomBodyGenerator;
	}
	BodyObject * createBody(const ObjectName & objectName,
							const Math::Vec3 & position, const Math::Vec3 & velocity,
							const Math::Quat & rotation, const Math::Vec3 & angularVelocity,
							float minSize, float maxSize,
							const float density, ModelTime birthTime)
	{
		unsigned int bodyType = rand<float>() * bodyTypes_.size();
		return bodyTypes_[bodyType]->createBody(objectName, position, velocity, rotation, angularVelocity, minSize, maxSize, density, birthTime);
	}
private:
	typedef std::vector<BodyType*> BodyTypes;
	BodyTypes bodyTypes_;
};

BodyEmmiter::Activity::Activity(BodyEmmiter * bodyEmmiter, ModelTime dtime) : PeriodicActivity(dtime), bodyEmmiter_(bodyEmmiter)
{
}

ModelTime BodyEmmiter::Activity::operator()(ModelTime time)
{
	bodyEmmiter_->update(time, getDTime());
	return PeriodicActivity::operator()(time);
}

BodyEmmiter::BodyEmmiter(const std::string & nameIn, ModelTime modelTime, const Variant & variant) : Actor(variant), activity_(this, 0.5), bodyCount_(0u)
{
	name = nameIn;
	::load(variant.key("pos"), pos_);
	tryLoadQuat(variant, rot_);
	::load(variant.key("size"), size_);

	minVel_ = variant.key("minVel");
	maxVel_ = variant.key("maxVel");
	maxAngle_ = variant.key("maxAngle");
	minBodySize_ = variant.key("minBodySize");
	maxBodySize_ = variant.key("maxBodySize");
	bodyDensity_ = variant.key("bodyDensity");
	bodyRate_ = variant.key("bodyRate");
	load(variant.key("planeColor"), planeColor_);
	load(variant.key("bodyColorMin"), bodyColorMin_);
	load(variant.key("bodyColorMax"), bodyColorMax_);

	plane_ = createPlane_(modelTime);
	Scene::instance()->addObject(plane_);

	//const Variant vBodyTypes = variant.key("bodyTypes");
	//bodyTypes_.resize(vBodyTypes.size());
	//for(Variant::ConstIterator itBT = vBodyTypes.begin();
	//	itBT != vBodyTypes.end();
	//	++itBT)
	//	bodyTypes_[itBT.index()].load(*itBT);

	activityScheduler.scheduleNow(&activity_);
}

BodyEmmiter::~BodyEmmiter()
{
	activityScheduler.remove(&activity_);
	Scene::instance()->removeObject(plane_);
	delete plane_;
}

Actor * BodyEmmiter::create(const std::string & nameIn, ModelTime modelTime, const Variant & variant)
{
	return new BodyEmmiter(nameIn, modelTime, variant);
}

unsigned int factorial(unsigned int n)
{
	unsigned int result = 1;
	for(unsigned int i = 1; i < n; i++)
		result *= i;
	return result;
}

inline double poisson(unsigned int n, ModelTime lambda, ModelTime dtime)
{
	return pow(lambda * dtime, (int)n) * exp(-lambda * dtime) / factorial(n);
}

unsigned int getEventsCount(double rndVal, ModelTime lambda, ModelTime dtime)
{
	if(rndVal < 1.0E-5)
		return 1;

	unsigned int n = 1;
	while(true)
	{
		double P = poisson(n, lambda, dtime);
		if(rndVal > P)
			return n - 1;
		else
			n++;
	}
}

void BodyEmmiter::update(ModelTime modelTime, ModelTime dtime)
{
	unsigned int eventCount = getEventsCount(rand<ModelTime>(), bodyRate_, dtime);
	for(unsigned int i = 0; i < eventCount; ++i)
	{
		++bodyCount_;
		BodyObject * bodyObject = createBody_(modelTime);
		MovingBodyStorage::instance().add(bodyObject);
		Scene::instance()->addObject(bodyObject);
	}
}

inline Math::Vec3 multVec3byElement(const Math::Vec3 & left, const Math::Vec3 & right)
{
	return Math::Vec3(	left.x() * right.x(),
						left.y() * right.y(),
						left.z() * right.z());
}

BodyObject * BodyEmmiter::createPlane_(ModelTime modelTime)
{
	const Math::Real planeThickness = 1.0;
	const Math::Vec3 planePos = pos_ - quatRotate(rot_, Math::Vec3(1.0, 0.0, 0.0)) * (maxBodySize_ + planeThickness + size_.x() / 2);
	IMass::Data mass;
	mass.mass = NoMass;
	BodyObject * plane = new BodyObject("Emitter Plane - " + name, "Box",
										planePos, Math::Vec3(0.0, 0.0, 0.0),
										rot_, Math::Vec3(0.0f, 0.0f, 0.0f),
										mass,
										modelTime);
	plane->addArg(0, planeThickness);
	plane->addArg(1, size_.y());
	plane->addArg(2, size_.z());

	for(int c = 0; c < 3; ++c)
		plane->addArg(c + 3, planeColor_[c]);

	return plane;
}

BodyObject * BodyEmmiter::createBody_(ModelTime modelTime)
{
	Math::Vec3 pos = - size_ / 2 + multVec3byElement(rndVec3(), size_);
	pos = quatRotate(rot_, pos);
	pos += pos_;	
	
	Math::Vec3 bodyRotAxis = 2.0f * rndVec3() - Math::Vec3(1.0f, 1.0f, 1.0f);
	bodyRotAxis.normalize();
	Math::Quat rot(bodyRotAxis, rand<float>() * 6.28f - 3.14f);
	rot.normalize();

	Math::Real vel = minVel_ + (maxVel_ - minVel_) * rand<float>();
	Math::Real az = maxAngle_ * rand<Math::Real>();
	Math::Real elev = maxAngle_ * rand<Math::Real>();
	Math::Vec3 velocity = vel * Math::Vec3(cos(az) * cos(elev), sin(elev), sin(az) * cos(elev));
	velocity = quatRotate(rot_, velocity);

	const Math::Vec3 maxAngularVelocity(1.0f, 1.0f, 1.0f);
	Math::Vec3 angularVelocity = 2 * multVec3byElement(rndVec3(), maxAngularVelocity) - maxAngularVelocity;

	BodyObject * bodyObject = RandomBodyGenerator::instance().createBody(	OSS("Body - " << name << " - " << bodyCount_),
																			pos, velocity, rot, angularVelocity, minBodySize_, maxBodySize_,
																			bodyDensity_, modelTime);
	for(int c = 0; c < 3; ++c)
		bodyObject->addArg(c + 3, bodyColorMin_[c] + rand<float>() * (bodyColorMax_[c] - bodyColorMin_[c]));
	return bodyObject;
}