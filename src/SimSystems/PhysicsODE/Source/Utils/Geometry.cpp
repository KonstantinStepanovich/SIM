#include "stdafx.h"

#include "Utils/Geometry.hpp"
#include "Utils/Common.hpp"

#include "Interface/IEntity.hpp"
#include "Interface/IAnimated.hpp"

#include <map>

#include "Math/Vec3.hpp"
#include "Math/Quat.hpp"
#include "Math/Box3.hpp"
#include "Utils/VariantMathUtil.hpp"
#include "Database/Variant.hpp"
#include "Database/ParserJson.hpp"
#include "Algorithm/Exception.hpp"

#include "ode/objects.h"
#include "ode/collision_space.h"
#include "ode/collision.h"

using namespace Sim;
using namespace Sim::PhysicsODE;
using namespace Sim::Interfaces;

namespace Sim
{

namespace PhysicsODE
{

//Geometry Node

class GeometryNodeFactory
{
public:
	GeometryNodeFactory()
	{
		ÒreateMap_["Sphere"] = &createSphere_;
		ÒreateMap_["Box"] = &createBox_;
		ÒreateMap_["Cylinder"] = &createCylinder_;
		ÒreateMap_["Capsule"] = &createCapsule_;
		ÒreateMap_["Group"] = &createGroup_;
		ÒreateMap_["Transform"] = &createTransform_;
	}
	static GeometryNodeFactory & instance()
	{
		static GeometryNodeFactory geometryNodeFactory;
		return geometryNodeFactory;
	}
	GeometryNode * create(dSpaceID spaceID, dBodyID bodyID, const Variant & variant)
	{
		std::string type = variant.key("type");
		CreateMap::iterator itCM = ÒreateMap_.find(type);
		if(itCM == ÒreateMap_.end())
			THROW_EXCEPTION(Exception, OSS("Unknown geometry node type \"" << type << "\""));
		GeometryNode * geometryNode = (*itCM->second)(spaceID, bodyID, variant);
		return geometryNode;
	}
private:
	typedef GeometryNode * (*Create)(dSpaceID spaceID, dBodyID bodyID, const Variant & variant);
	typedef std::map<std::string, Create> CreateMap;
	CreateMap ÒreateMap_;
	static GeometryNode * createSphere_(dSpaceID spaceID, dBodyID bodyID, const Variant & variant);
	static GeometryNode * createBox_(dSpaceID spaceID, dBodyID bodyID, const Variant & variant);
	static GeometryNode * createCylinder_(dSpaceID spaceID, dBodyID bodyID, const Variant & variant);
	static GeometryNode * createCapsule_(dSpaceID spaceID, dBodyID bodyID, const Variant & variant);
	static GeometryNode * createGroup_(dSpaceID spaceID, dBodyID bodyID, const Variant & variant);
	static GeometryNode * createTransform_(dSpaceID spaceID, dBodyID bodyID, const Variant & variant);
};

class GeometryNode
{
public:
	GeometryNode(dSpaceID spaceID, dBodyID bodyID, const Variant & variant) :
	  name_(variant.key("name", VariantRoot(""))), enabled(true), parentEnabled(true), obbIsValid_(false)
	{
	}
	virtual ~GeometryNode()
	{
	}
	virtual GeometryNode * find(const std::string & name)
	{
		return name == name_ ? this : NULL;
	}
	void setEnabled(bool on)
	{
		bool prev = enabled && parentEnabled;
		enabled = on;
		bool curr = enabled && parentEnabled;
		if(prev != curr)
			onEnable_(curr);
	}
	void notifyParentEnabled(bool on)
	{
		bool prev = enabled && parentEnabled;
		parentEnabled = on;
		bool curr = enabled && parentEnabled;
		if(prev != curr)
			onEnable_(curr);
	}	
	virtual void transform(const Math::Vec3 & pos, const Math::Quat & quat, const Math::Vec3 & size) = 0;
	inline const std::string & getName() const { return name_; }
	virtual const Math::Box3 & getOBB() const
	{
		if(!obbIsValid_)
		{
			obb_ = calcOBB_();
			obbIsValid_ = true;
		}
		return obb_;
	}
protected:
	bool parentEnabled;
	bool enabled;
private:
	virtual void onEnable_(bool on) = 0;
	virtual Math::Box3 calcOBB_() const = 0;
	std::string name_;
	mutable Math::Box3 obb_;
	mutable bool obbIsValid_;
};

class GeometryNodeTerminal : public GeometryNode
{
public:
	GeometryNodeTerminal(dSpaceID spaceID, dBodyID bodyID, const Variant & variant, dGeomID geomIDIn) : 
		GeometryNode(spaceID, bodyID, variant), geomID(geomIDIn), pos_(Math::Vec3(0.0, 0.0, 0.0)), quat_(Math::Quat::getIdentity())
	{
		dGeomSetBody(geomID, bodyID);
		if(const Variant & vPos = variant.tryKey("pos"))
		{
			load(vPos, pos_);
			dGeomSetOffsetPosition(geomID, pos_.x(), pos_.y(), pos_.z());
		}
		if(tryLoadQuat(variant, quat_))
		{
			const dQuaternion q = { quat_.w(), quat_.x(), quat_.y(), quat_.z() };
			dGeomSetOffsetQuaternion(geomID, q);
		}
	}
	virtual void transform(const Math::Vec3 & pos, const Math::Quat & quat, const Math::Vec3 & size)
	{
		const Math::Vec3 posResult = pos + quatRotate(quat, pos_);
		dGeomSetOffsetPosition(geomID, posResult.x(), posResult.y(), posResult.z());
		const Math::Quat quatResult = quat * quat_;
		const dQuaternion q = { quatResult.w(), quatResult.x(), quatResult.y(), quatResult.z() };
		dGeomSetOffsetQuaternion(geomID, q);
	}
protected:
	dGeomID geomID;
private:
	virtual void onEnable_(bool on)
	{
		if(on)
			dGeomEnable(geomID);
		else
			dGeomDisable(geomID);
	}
	Math::Vec3 pos_;
	Math::Quat quat_;
};

class GeometryNodeSphere : public GeometryNodeTerminal
{
public:
	GeometryNodeSphere(dSpaceID spaceID, dBodyID bodyID, const Variant & variant) : 
		GeometryNodeTerminal(spaceID, bodyID, variant, dCreateSphere(spaceID, 0.0f))
	{
		radius_ = variant.key("radius");
		dGeomSphereSetRadius(geomID, radius_);
	}
	virtual void transform(const Math::Vec3 & pos, const Math::Quat & quat, const Math::Vec3 & size)
	{
		GeometryNodeTerminal::transform(pos, quat, size);
		const float resultRadius = size.x() * radius_;
		dGeomSphereSetRadius(geomID, resultRadius);
	}
private:
	virtual Math::Box3 calcOBB_() const
	{
		const dReal radius = dGeomSphereGetRadius(geomID);
		return Math::Box3(	Math::Vec3(-radius, -radius, -radius),
							Math::Vec3(radius, radius, radius));
	}
	dReal radius_;
};

class GeometryNodeBox : public GeometryNodeTerminal
{
public:
	GeometryNodeBox(dSpaceID spaceID, dBodyID bodyID, const Variant & variant) : 
		GeometryNodeTerminal(spaceID, bodyID, variant, dCreateBox(spaceID, 0.0f, 0.0f, 0.0f))
	{
		Math::Vec3 size;
		load(variant.key("size"), size_);
		dGeomBoxSetLengths(geomID, size_.x(), size_.y(), size_.z());
	}
	virtual void transform(const Math::Vec3 & pos, const Math::Quat & quat, const Math::Vec3 & size)
	{
		GeometryNodeTerminal::transform(pos, quat, size);
		const Math::Vec3 sizeResult(size_.x() * size.x(), size_.y() * size.y(), size_.z() * size.z());
		dGeomBoxSetLengths(geomID, sizeResult.x(), sizeResult.y(), sizeResult.z());
	}
private:
	virtual Math::Box3 calcOBB_() const
	{
		dVector3 size;
		dGeomBoxGetLengths(geomID, size);
		return Math::Box3(	Math::Vec3(-size[0] / 2, -size[1] / 2, -size[2] / 2),
							Math::Vec3(size[0] / 2, size[1] / 2, size[2] / 2));
	}
	Math::Vec3 size_;
};

class GeometryNodeCylinder : public GeometryNodeTerminal
{
public:
	GeometryNodeCylinder(dSpaceID spaceID, dBodyID bodyID, const Variant & variant) : 
		GeometryNodeTerminal(spaceID, bodyID, variant, dCreateCylinder(spaceID, 0.0f, 0.0f))
	{
		radius_ = variant.key("radius");
		height_ = variant.key("height");
		dGeomCylinderSetParams(geomID, radius_, height_);
	}
	virtual void transform(const Math::Vec3 & pos, const Math::Quat & quat, const Math::Vec3 & size)
	{
		GeometryNodeTerminal::transform(pos, quat, size);
		const float resultRadius = radius_ * (size.x() + size.y()) / 2;
		const float resultHeight = height_ * size.z();
		dGeomCylinderSetParams(geomID, resultRadius, resultHeight);
	}
private:
	virtual Math::Box3 calcOBB_() const
	{
		dReal radius, height;
		dGeomCylinderGetParams(geomID, &radius, &height);
		return Math::Box3(	Math::Vec3(-radius, -radius, -height / 2),
							Math::Vec3(radius, radius, height / 2));
	}
	dReal radius_, height_;
};

class GeometryNodeCapsule : public GeometryNodeTerminal
{
public:
	GeometryNodeCapsule(dSpaceID spaceID, dBodyID bodyID, const Variant & variant) : 
	  GeometryNodeTerminal(spaceID, bodyID, variant, dCreateCapsule(spaceID, 0.0f, 0.0f))
	  {
		  radius_ = variant.key("radius");
		  height_ = variant.key("height");
		  dGeomCapsuleSetParams(geomID, radius_, height_);
	  }
	  virtual void transform(const Math::Vec3 & pos, const Math::Quat & quat, const Math::Vec3 & size)
	  {
		  GeometryNodeTerminal::transform(pos, quat, size);
		  const float resultRadius = radius_ * (size.x() + size.y()) / 2;
		  const float resultHeight = height_ * size.z();
		  dGeomCapsuleSetParams(geomID, resultRadius, resultHeight);
	  }
private:
	virtual Math::Box3 calcOBB_() const
	{
		dReal radius, height;
		dGeomCapsuleGetParams(geomID, &radius, &height);
		return Math::Box3(	Math::Vec3(-radius, -radius, -height / 2),
							Math::Vec3(radius, radius, height / 2));
	}
	dReal radius_, height_;
};

class GeometryNodeGroup : public GeometryNode
{
public:
	GeometryNodeGroup(dSpaceID spaceID, dBodyID bodyID, const Variant & variant) : GeometryNode(spaceID, bodyID, variant)
	{
		const Variant vChildren = variant.key("children");
		children_.resize(vChildren.size());
		for(Variant::ConstIterator itC = vChildren.begin();
			itC != vChildren.end();
			++itC)
		{
			GeometryNode * geometryNode = GeometryNodeFactory::instance().create(spaceID, bodyID, *itC);
			children_[itC.index()] = geometryNode;
		}
	}
	~GeometryNodeGroup()
	{
		for(Children::const_iterator itC = children_.begin();
			itC != children_.end();
			++itC)
			delete *itC;
	}
	virtual GeometryNode * find(const std::string & name)
	{
		GeometryNode * geometryNode = GeometryNode::find(name);
		if(geometryNode != NULL)
			return geometryNode;
		for(Children::const_iterator itC = children_.begin();
			itC != children_.end();
			++itC)
		{
			GeometryNode * geometryNode = (*itC)->find(name);
			if(geometryNode != NULL)
				return geometryNode;
		}
		return NULL;
	}	
	virtual void transform(const Math::Vec3 & pos, const Math::Quat & quat, const Math::Vec3 & size)
	{
		for(Children::const_iterator itC = children_.begin();
			itC != children_.end();
			++itC)
			(*itC)->transform(pos, quat, size);
	}
private:
	virtual void onEnable_(bool on)
	{
		for(Children::const_iterator itC = children_.begin();
			itC != children_.end();
			++itC)
			(*itC)->notifyParentEnabled(on);
	}
	virtual Math::Box3 calcOBB_() const
	{
		Math::Box3 obb = Math::Box3();
		for(Children::const_iterator itC = children_.begin();
			itC != children_.end();
			++itC)
			obb.expand((*itC)->getOBB());
		return obb;
	}
	typedef std::vector<GeometryNode*> Children;
	Children children_;
};

class GeometryNodeTransform : public GeometryNodeGroup
{
public:
	GeometryNodeTransform(dSpaceID spaceID, dBodyID bodyID, const Variant & variant) : GeometryNodeGroup(spaceID, bodyID, variant),
		pos_(Math::Vec3(0.0, 0.0 ,0.0)), quat_(Math::Quat::getIdentity()), scale_(Math::Vec3(1.0, 1.0 ,1.0))
	{
	}
	inline void setPos(const Math::Vec3 & pos) { pos_ = pos; transform_(); }
	inline void setQuat(const Math::Quat & quat) { quat_ = quat; transform_(); }
	inline void setScale(const Math::Vec3 & scale) { scale_ = scale; transform_(); }
	inline const Math::Vec3 & getPos() const { return pos_; }
	inline const Math::Quat & getQuat() const { return quat_; }
	inline const Math::Vec3 & getScale() const { return scale_; }
	virtual void transform(const Math::Vec3 & pos, const Math::Quat & quat, const Math::Vec3 & scale)
	{
		GeometryNodeGroup::transform(	pos + pos_, quat * quat_, 
										Math::Vec3(scale.x() * scale_.x(), scale.y() * scale_.y(), scale.z() * scale_.z()));
	}
private:
	void transform_()
	{
		GeometryNodeGroup::transform(pos_, quat_, scale_);
	}
	Math::Vec3 pos_;
	Math::Quat quat_;
	Math::Vec3 scale_;
};

GeometryNode * GeometryNodeFactory::createSphere_(dSpaceID spaceID, dBodyID bodyID, const Variant & variant)
{
	return new GeometryNodeSphere(spaceID, bodyID, variant);
}

GeometryNode * GeometryNodeFactory::createBox_(dSpaceID spaceID, dBodyID bodyID, const Variant & variant)
{
	return new GeometryNodeBox(spaceID, bodyID, variant);
}

GeometryNode * GeometryNodeFactory::createCylinder_(dSpaceID spaceID, dBodyID bodyID, const Variant & variant)
{
	return new GeometryNodeCylinder(spaceID, bodyID, variant);
}

GeometryNode * GeometryNodeFactory::createCapsule_(dSpaceID spaceID, dBodyID bodyID, const Variant & variant)
{
	return new GeometryNodeCapsule(spaceID, bodyID, variant);
}

GeometryNode * GeometryNodeFactory::createGroup_(dSpaceID spaceID, dBodyID bodyID, const Variant & variant)
{
	return new GeometryNodeGroup(spaceID, bodyID, variant);
}

GeometryNode * GeometryNodeFactory::createTransform_(dSpaceID spaceID, dBodyID bodyID, const Variant & variant)
{
	return new GeometryNodeTransform(spaceID, bodyID, variant);
}

//Animation

class GeometryAnimation
{
public:
	virtual ~GeometryAnimation()
	{
	}
	virtual void operator()(double phase) const = 0;
};

class TransformationPath
{
public:
	struct Point
	{
		Point() : pos(Math::Vec3(0.0, 0.0, 0.0)), quat(Math::Quat::getIdentity() ), scale(Math::Vec3(1.0, 1.0, 1.0)) {;}
		Point(const Math::Vec3 & posIn, const Math::Quat & quatIn, Math::Vec3 & scaleIn) : pos(posIn), quat(quatIn), scale(scaleIn) {;}
		Math::Vec3 pos;
		Math::Quat quat;
		Math::Vec3 scale;
	};
	TransformationPath()
	{		
	}
	void clear()
	{
		points_.clear();
	}
	void add(double phase, const Point & point)
	{
		points_[phase] = point;
	}
	bool getPoint(double phase, Point & point) const
	{
		Points::const_iterator itP = points_.upper_bound(phase);
		if(itP != points_.end())
		{
			const double nextPhase = itP->first;
			const Point & next = itP->second;
			--itP;
			if(itP != points_.end())
			{
				const double prevPhase = itP->first;
				const Point & prev = itP->second;
				const double param = (phase - prevPhase) / (nextPhase - prevPhase);
				point = Point(	prev.pos * (1.0f - param) + next.pos * param,
								prev.quat.slerp(next.quat, param),
								prev.scale * (1.0f - param) + next.scale * param);
				return true;
			}
		}
		else if(!points_.empty())
		{
			--itP;
			point = itP->second;
			return true;
		}
		else
			return false;
	}
private:
	typedef std::map<double, Point> Points;
	Points points_;
};

void load(const Variant & variant, TransformationPath & transformationPath)
{
	for(Variant::ConstIterator itP = variant.begin();
		itP != variant.end();
		++itP)
	{
		const Variant vPoint = *itP;
		TransformationPath::Point point;
		const double phase = vPoint.key("phase");
		const Variant vTransform = vPoint.key("transform");
		if(const Variant & vPos = vTransform.tryKey("pos"))
			load(vPos, point.pos);
		tryLoadQuat(vTransform, point.quat);
		if(const Variant & vSize = vTransform.tryKey("scale"))
			load(vSize, point.pos);
		transformationPath.add(phase, point);
	}
}

class AnimationTransformPath : public GeometryAnimation
{
public:
	AnimationTransformPath(GeometryNode * root, const Variant & variant) : geometryNodeTransform_(NULL)
	{
		std::string transformName = variant.key("transformName");
		geometryNodeTransform_ = static_cast<GeometryNodeTransform*>(root->find(transformName));
		if(geometryNodeTransform_ == NULL)
			THROW_EXCEPTION(Exception, OSS("Transform node \"" << transformName << "\" not found"));
		::load(variant.key("path"), transformationPath_);
	}
	virtual void operator()(double arg) const
	{
		Math::Vec3 pos, scale;
		Math::Quat quat;
		TransformationPath::Point point;
		if(transformationPath_.getPoint(arg, point))
		{
			geometryNodeTransform_->setPos(point.pos);
			geometryNodeTransform_->setQuat(point.quat);
			geometryNodeTransform_->setScale(point.scale);
		}
	}
private:
	GeometryNodeTransform * geometryNodeTransform_;
	TransformationPath transformationPath_;
};

class AnimationTransformScale : public GeometryAnimation
{
public:
	AnimationTransformScale(GeometryNode * root, const Variant & variant) : geometryNodeTransform_(NULL)
	{
		std::string transformName = variant.key("transformName");
		geometryNodeTransform_ = static_cast<GeometryNodeTransform*>(root->find(transformName));
		if(geometryNodeTransform_ == NULL)
			THROW_EXCEPTION(Exception, OSS("Transform node \"" << transformName << "\" not found"));
		_axis = variant.key("axis");
		if(_axis > 2)
			THROW_EXCEPTION(Exception, OSS("Wrong axis " << _axis));
	}
	virtual void operator()(double arg) const
	{
		Math::Vec3 scale = geometryNodeTransform_->getScale();
		scale[_axis] = arg;
		geometryNodeTransform_->setScale(scale);
	}
private:
	GeometryNodeTransform * geometryNodeTransform_;
	unsigned int			_axis;
};

class AnimationEnable : public GeometryAnimation
{
public:
	AnimationEnable(GeometryNode * root, const Variant & variant) : geometryNode_(NULL)
	{
		std::string nodeName = variant.key("nodeName");
		geometryNode_ = root->find(nodeName);
		if(geometryNode_ == NULL)
			THROW_EXCEPTION(Exception, OSS("Node node \"" << nodeName << "\" not found"));	
	}
	virtual void operator()(double phase) const
	{
		geometryNode_->setEnabled(phase >= 0.5);
	}
private:
	GeometryNode * geometryNode_;
};

class AnimationFactory
{
	AnimationFactory()
	{
		ÒreateMap_["TransformPath"] = &createAnimationTransformPath_;
		ÒreateMap_["TransformScale"] = &createAnimationTransformScale_;
		ÒreateMap_["Enable"] = &createAnimationEnable_;
	}
public:
	static AnimationFactory & instance()
	{
		static AnimationFactory animationFactory;
		return animationFactory;
	}
	GeometryAnimation * create(GeometryNode * root, const Variant & variant)
	{
		std::string type = variant.key("type");
		CreateMap::const_iterator it— = ÒreateMap_.find(type);
		if(it— == ÒreateMap_.end())
			THROW_EXCEPTION(Exception, OSS("Unknown animation type \"" << type << "\""));
		GeometryAnimation * animation = (*it—->second)(root, variant);
		return animation;
	}
private:	
	static GeometryAnimation * createAnimationTransformPath_(GeometryNode * root, const Variant & variant)
	{
		return new AnimationTransformPath(root, variant);
	}
	static GeometryAnimation * createAnimationTransformScale_(GeometryNode * root, const Variant & variant)
	{
		return new AnimationTransformScale(root, variant);
	}
	static GeometryAnimation * createAnimationEnable_(GeometryNode * root, const Variant & variant)
	{
		return new AnimationEnable(root, variant);
	}	
	typedef GeometryAnimation*(*Create)(GeometryNode * root, const Variant & variant);
	typedef std::map<std::string, Create> CreateMap;
	CreateMap ÒreateMap_;
};

static void load(const Variant & variant, GeometryNode * root, AnimationGroup & animationGroup)
{
	animationGroup.resize(variant.size());
	for(Variant::ConstIterator itA = variant.begin();
		itA != variant.end();
		++itA)
		animationGroup[itA.index()] = AnimationFactory::instance().create(root, *itA);
}

static void load(const Variant & variant, GeometryNode * root, AnimationGroups & animationGroups)
{
	animationGroups.resize(variant.size());
	for(Variant::ConstIterator itAG = variant.begin();
		itAG != variant.end();
		++itAG)
		load(*itAG, root, animationGroups[itAG.index()]);
}

}

}

std::string Geometry::path = "Common/Geometry/Physics/";

Geometry::Geometry(dSpaceID parentSpaceID, dBodyID bodyID) : spaceID_(dSimpleSpaceCreate(parentSpaceID)), bodyID_(bodyID)
{
	dSpaceSetCleanup(spaceID_, 1);
	dGeomSetData((dGeomID)spaceID_, (void*)1);
}

Geometry::~Geometry()
{
	dSpaceDestroy(spaceID_);
}

//void Geometry::setParentSpace(dSpaceID parentSpaceID)
//{
//	dSpaceID oldParentSpaceID = dGeomGetSpace(spaceID_);
//	if(parentSpaceID != oldParentSpaceID)
//	{
//		dSpaceRemove(oldParentSpaceID, spaceID_);
//		dSpaceAdd(parentSpaceID, spaceID_);
//	}
//}

void Geometry::setDynamic(bool flag)
{
	dGeomSetCategoryBits((dGeomID)spaceID_, flag ? FLAG_SPACE_DYNAMIC : FLAG_NULL);
}

void Geometry::onEntityChanged(IEntity * entity)
{
	typeId = entity->getTypeId();
	VariantRoot variant;
	loadJson((path + typeId + ".js").c_str(), "geometry", variant);
	load(variant);
}

void Geometry::onAnimationChanged(IAnimated * animated)
{
	IAnimated::Args args;
	animated->getArgs(args);
	for(IAnimated::Args::const_iterator itA = args.begin();
		itA != args.end();
		++itA)
	{
		const IAnimated::Arg & arg = *itA;
		animate(arg.index, arg.value);
	}
}

/*
Geometry::Geometry(const Geometry & geometry)
{
	spaceID_(dsimplespacecreate(spaceID))
	root_ = geometry.root_->clone(spaceID_);
}
*/

void Geometry::load(const Variant & variant)
{
	root_.reset(GeometryNodeFactory::instance().create(spaceID_, bodyID_, variant.key("geometry")));
	const Variant vAnimation = variant.tryKey("animation");
	if(vAnimation)
		::load(vAnimation, root_.get(), animationGroups_);
}

void Geometry::animate(unsigned int arg, double phase)
{
	if(arg < animationGroups_.size())
	{
		const AnimationGroup & animationGroup = animationGroups_[arg];
		for(AnimationGroup::const_iterator itA = animationGroup.begin();
			itA != animationGroup.end();
			++itA)
			(**itA)(phase);
	}
}

const Sim::Math::Box3 & Geometry::getOBB() const
{
	return root_->getOBB();
}