#include "stdafx.h"

#include "Utils/Geometry.hpp"

#include "Interface/IEntity.hpp"
#include "Interface/IAnimated.hpp"

#include "Database/Variant.hpp"
#include "Database/VariantUtil.hpp"
#include "Database/ParserJson.hpp"
#include "Utils/VariantMathUtil.hpp"

#include <osg/Geode>
#include <osg/MatrixTransform>
#include <osg/ShapeDrawable>
#include <osg/AnimationPath>
#include <osg/TexMat>
#include <osg/Texture2D>
#include <osg/StateSet>
#include <osgDB/ReadFile>

using namespace Sim;
using namespace Sim::GraphicsOSG;

Animation::~Animation()
{
}

namespace Sim
{

inline void load(const Variant & variant, osg::Vec3 & vec)
{
	load(variant, vec._v);
}

inline void load(const Variant & variant, osg::Vec4 & vec)
{
	load(variant, vec._v);
}

inline void load(const Variant & variant, osg::Quat & quat)
{
	for(int i = 0; i < 4; i++)
		quat[i] = variant.index(i);
}

inline bool tryLoadQuat(const Variant & variant, osg::Quat & quat)
{
	if(tryLoadFromTable(variant, "quat", quat))
		return true;
	else if(const Variant vRot = variant.tryKey("rot"))
	{
		osg::Vec3 axis;
		loadFromTable(vRot, "axis", axis);
		double angle = angle = vRot.key("angle");
		quat.makeRotate(angle, axis);
		return true;
	}
	else if(const Variant vEuler = variant.tryKey("euler"))
	{
		quat =	osg::Quat(vEuler.index(0u).getReal(), osg::Vec3(0.0f, 1.0f, 0.0f)) *
				osg::Quat(vEuler.index(1u).getReal(), osg::Vec3(0.0f, 0.0f, 1.0f)) * 
				osg::Quat(vEuler.index(2u).getReal(), osg::Vec3(1.0f, 0.0f, 0.0f));
		return true;
	}
	else
		return false;
}

inline void load(const Variant & variant, osg::AnimationPath & animationPath)
{
	for(Variant::ConstIterator itP = variant.begin();
		itP != variant.end();
		++itP)
	{
		const Variant vControlPoint = *itP;

		float phase = vControlPoint.key("phase");
		const Variant vTransform = vControlPoint.key("transform");

		osg::Vec3 position;
		osg::Quat rotation;
		osg::Vec3 scale(1.0, 1.0, 1.0);

		tryLoadFromTable(vTransform, "pos", position);
		tryLoadQuat(vTransform, rotation);
		tryLoadFromTable(vTransform, "scale", scale);

		osg::AnimationPath::ControlPoint controlPoint(position, rotation, scale);
		animationPath.insert(phase, controlPoint);
	}
}

namespace GraphicsOSG
{

void load(const Variant & variant, Geometry & geometryDesc)
{
	geometryDesc.load(variant);
}

}

}

template<class NodeType>
class FindNodeByName : public osg::NodeVisitor
{
public:
	FindNodeByName(const std::string & name) :
		osg::NodeVisitor(NODE_VISITOR, TRAVERSE_ALL_CHILDREN), name_(name), found_(NULL)
	{
	}
	virtual void apply(NodeType & node)
	{
		if(node.getName() == name_)
		{
			found_ = &node;
			setTraversalMode(TRAVERSE_NONE);
		}
	}
	inline NodeType * getNode() { return found_; }
private:
	std::string name_;
	NodeType * found_;
};

template<class NodeType>
NodeType * findNode(osg::Node * root, const std::string & name)
{
	FindNodeByName<NodeType> findNodeByName(name);
	root->accept(findNodeByName);
	root->traverse(findNodeByName);
	return findNodeByName.getNode();
}

class AnimationTransformPath : public Animation
{
public:
	AnimationTransformPath(osg::Node * root, const Variant & variant)
	{
		std::string transformName = variant.key("transformName");
		osg::Transform * transform = findNode<osg::Transform>(root, transformName);
		if(transform == NULL)
			THROW_EXCEPTION(Exception, OSS("Transform \"" << transformName << "\" not found"));
		_matrixTransform = static_cast<osg::MatrixTransform*>(transform);
		_animationPath = new osg::AnimationPath();
		_animationPath->setLoopMode(osg::AnimationPath::NO_LOOPING);
		_animationPath->clear();
		load(variant.key("path"), *_animationPath);
	}
	virtual void operator()(float arg) const
	{
		osg::Matrix matrix;
		if(_animationPath->getMatrix(arg, matrix))
			_matrixTransform->setMatrix(matrix);
	}
private:
	osg::MatrixTransform *				_matrixTransform;
	osg::ref_ptr<osg::AnimationPath>	_animationPath;
};

class AnimationTransformScale : public Animation
{
public:
	AnimationTransformScale(osg::Node * root, const Variant & variant) : _axis(0u)
	{
		std::string transformName = variant.key("transformName");
		osg::Transform * transform = findNode<osg::Transform>(root, transformName);
		if(transform == NULL)
			THROW_EXCEPTION(Exception, OSS("Transform \"" << transformName << "\" not found"));
		_matrixTransform = static_cast<osg::MatrixTransform*>(transform);
		_axis = variant.key("axis");
		if(_axis > 2)
			THROW_EXCEPTION(Exception, OSS("Wrong axis " << _axis));
	}
	virtual void operator()(float arg) const
	{
		osg::Matrix matrix = _matrixTransform->getMatrix();
		osg::Vec3 scale(1.0f, 1.0f, 1.0f);
		osg::Vec3 oldScale = matrix.getScale();
		scale[_axis] = arg / oldScale[_axis];
		matrix.postMultScale(scale);
		_matrixTransform->setMatrix(matrix);
	}
private:
	osg::MatrixTransform *	_matrixTransform;
	unsigned int			_axis;
};

class AnimationShapeColor : public Animation
{
public:
	AnimationShapeColor(osg::Node * root, const Variant & variant)
	{
		std::string shapeName = variant.key("shapeName");
		osg::Geode * geode = findNode<osg::Geode>(root, shapeName);
		if(	geode == NULL ||
			geode->getNumDrawables() == 0)
			THROW_EXCEPTION(Exception, OSS("Geode \"" << shapeName << "\" not found"));
		_shapeDrawable = static_cast<osg::ShapeDrawable*>(geode->getDrawable(0));		
		_color = variant.key("color");
	}
	virtual void operator()(float arg) const
	{
		osg::Vec4 color =_shapeDrawable->getColor();
		color[_color] = arg;
		_shapeDrawable->setColor(color);
	}
private:
	osg::ShapeDrawable *	_shapeDrawable;
	unsigned int			_color;
};

class NodeFactory
{
	NodeFactory()
	{
		nodeCreators_["Group"] = &NodeFactory::createGroup_;
		nodeCreators_["Transform"] = &NodeFactory::createTransform_;
		nodeCreators_["Box"] = &NodeFactory::createBox_;
		nodeCreators_["Cylinder"] = &NodeFactory::createCylinder_;
		nodeCreators_["Sphere"] = &NodeFactory::createSphere_;
		nodeCreators_["Capsule"] = &NodeFactory::createCapsule_;
	}
public:
	static NodeFactory & instance()
	{
		static NodeFactory nodeFactory;
		return nodeFactory;
	}
	osg::Node * create(const Textures & textures, const Variant & variant)
	{
		std::string type = variant.key("type");
		NodeCreators::const_iterator itNC = nodeCreators_.find(type);
		if(itNC == nodeCreators_.end())
			THROW_EXCEPTION(Exception, OSS("Unknown node type \"" << type << "\""));
		return (this->*(itNC->second))(textures, variant);
	}
private:
	void setTexture_(osg::Geode * geode, const Textures & textures, const Variant & variant)
	{
		if(Variant vTexture = variant.tryKey("texture"))
		{
			std::string name = vTexture.key("name");
			osg::Vec3 pos(0.0, 0.0, 0.0);
			tryLoadFromTable(vTexture, "pos", pos);
			osg::Quat quat;
			tryLoadQuat(vTexture, quat);
			osg::Vec3 scale(1.0, 1.0, 1.0);
			tryLoadFromTable(vTexture, "scale", scale);

			Textures::const_iterator itT = textures.find(name);
			if(itT == textures.end())
				THROW_EXCEPTION(Exception, OSS("Unknown texture \"" << name << "\""));		
			osg::StateSet* stateOne = new osg::StateSet();
			stateOne->setTextureAttributeAndModes(0, itT->second, osg::StateAttribute::ON);
			geode->setStateSet(stateOne);

			osg::TexMat* texmat = new osg::TexMat();
			texmat->setMatrix(osg::Matrix::translate(pos) * osg::Matrix::rotate(quat) * osg::Matrix::scale(scale));
			geode->getOrCreateStateSet()->setTextureAttributeAndModes(0, texmat);					
		}
	}
	void createNode_(osg::Node * node, const Variant & variant)
	{
		node->setName(variant.key("name", VariantRoot("")).getString());
	}
	void createElements_(osg::Group * group, const Textures & textures, const Variant & variant)
	{
		for(Variant::ConstIterator itE = variant.begin();
			itE != variant.end();
			++itE)
		{
			osg::Node * node = create(textures, *itE);
			group->addChild(node);
		}
	}
	osg::Node * createGroup_(const Textures & textures, const Variant & variant)
	{
		osg::Group * group = new osg::Group();
		createNode_(group, variant);
		createElements_(group, textures, variant.key("children"));
		return group;
	}
	osg::Node * createTransform_(const Textures & textures, const Variant & variant)
	{
		osg::MatrixTransform * matrixTransform = new osg::MatrixTransform();
		createNode_(matrixTransform, variant);
		osg::Vec3 pos;
		tryLoadFromTable(variant, "pos", pos);
		osg::Quat quat;
		tryLoadQuat(variant, quat);
		osg::Vec3 scale(1.0, 1.0, 1.0);
		tryLoadFromTable(variant, "scale", scale);
		osg::Matrix matrix;
		matrix.setTrans(pos);
		matrix.setRotate(quat);
		matrix.postMultScale(scale);
		matrixTransform->setMatrix(matrix);
		createElements_(matrixTransform, textures, variant.key("children"));
		return matrixTransform;
	}
	osg::Node * createBox_(const Textures & textures, const Variant & variant)
	{
		osg::Geode * geode = new osg::Geode();
		createNode_(geode, variant);
		
		osg::Vec3 pos(0.0f, 0.0f, 0.0f);
		tryLoadFromTable(variant, "pos", pos);
		osg::Quat quat;
		tryLoadQuat(variant, quat);
		
		osg::Vec3 size;
		load(variant.key("size"), size);

		osg::Vec4 color;
		tryLoadFromTable(variant, "color", color);

		osg::Box * box = new osg::Box(pos, size.x(), size.y(), size.z());
		box->setRotation(quat);
		osg::ShapeDrawable * drawable = new osg::ShapeDrawable(box);
		drawable->setColor(color);
		geode->addDrawable(drawable);
		setTexture_(geode, textures, variant);

		return geode;
	}
	osg::Node * createCylinder_(const Textures & textures, const Variant & variant)
	{
		osg::Geode * geode = new osg::Geode();
		createNode_(geode, variant);
		
		osg::Vec3 pos(0.0f, 0.0f, 0.0f);
		tryLoadFromTable(variant, "pos", pos);
		osg::Quat quat;
		tryLoadQuat(variant, quat);

		osg::Vec4 color;
		tryLoadFromTable(variant, "color", color);
		
		const float radius = variant.key("radius");
		const float height = variant.key("height");
		osg::Cylinder * cylinder = new osg::Cylinder(pos, radius, height);
		cylinder->setRotation(quat);
		osg::ShapeDrawable * drawable = new osg::ShapeDrawable(cylinder);
		drawable->setColor(color);
		geode->addDrawable(drawable);
		setTexture_(geode, textures, variant);

		return geode;
	}
	osg::Node * createSphere_(const Textures & textures, const Variant & variant)
	{
		osg::Geode * geode = new osg::Geode();
		createNode_(geode, variant);
		
		osg::Vec3 pos(0.0f, 0.0f, 0.0f);
		tryLoadFromTable(variant, "pos", pos);

		osg::Vec4 color;
		tryLoadFromTable(variant, "color", color);
		
		const float radius = variant.key("radius");
		osg::ShapeDrawable * drawable = new osg::ShapeDrawable(new osg::Sphere(pos, radius));
		drawable->setColor(color);
		geode->addDrawable(drawable);
		setTexture_(geode, textures, variant);

		return geode;
	}
	osg::Node * createCapsule_(const Textures & textures, const Variant & variant)
	{
		osg::Geode * geode = new osg::Geode();
		createNode_(geode, variant);

		osg::Vec3 pos(0.0f, 0.0f, 0.0f);
		tryLoadFromTable(variant, "pos", pos);

		osg::Vec4 color;
		tryLoadFromTable(variant, "color", color);

		const float radius = variant.key("radius");
		const float height = variant.key("height");
		osg::ShapeDrawable * drawable = new osg::ShapeDrawable(new osg::Capsule(pos, radius, height));
		drawable->setColor(color);
		geode->addDrawable(drawable);
		setTexture_(geode, textures, variant);

		return geode;
	}
	typedef osg::Node*(NodeFactory::*NodeCreator)(const Textures & textures, const Variant & variant);
	typedef std::map<std::string, NodeCreator> NodeCreators;
	NodeCreators nodeCreators_;
};

class AnimationFactory
{
	AnimationFactory()
	{
		animationCreators_["TransformPath"] = &createAnimationTransformPath_;
		animationCreators_["TransformScale"] = &createAnimationTransformScale_;
		animationCreators_["ShapeColor"] = &createAnimationShapeColor_;
	}
public:
	static AnimationFactory & instance()
	{
		static AnimationFactory animationFactory;
		return animationFactory;
	}
	Animation * create(osg::Node * root, const Variant & variant)
	{
		std::string type = variant.key("type");
		AnimationCreators::const_iterator itAC = animationCreators_.find(type);
		if(itAC == animationCreators_.end())
			THROW_EXCEPTION(Exception, OSS("Unknown animation type \"" << type << "\""));
		return (*itAC->second)(root, variant);
	}
private:	
	static Animation * createAnimationTransformPath_(osg::Node * root, const Variant & variant)
	{
		return new AnimationTransformPath(root, variant);
	}
	static Animation * createAnimationTransformScale_(osg::Node * root, const Variant & variant)
	{
		return new AnimationTransformScale(root, variant);
	}
	static Animation * createAnimationShapeColor_(osg::Node * root, const Variant & variant)
	{
		return new AnimationShapeColor(root, variant);
	}
	typedef Animation*(*AnimationCreator)(osg::Node * root, const Variant & variant);
	typedef std::map<std::string, AnimationCreator> AnimationCreators;
	AnimationCreators animationCreators_;
};

static void load(const Variant & variant, osg::Node * root, AnimationGroup & animationGroup)
{
	animationGroup.resize(variant.size());
	for(Variant::ConstIterator itA = variant.begin();
		itA != variant.end();
		++itA)
		animationGroup[itA.index()] = AnimationFactory::instance().create(root, *itA);
}

static void load(const Variant & variant, osg::Node * root, AnimationGroups & animationGroups)
{
	animationGroups.resize(variant.size());
	for(Variant::ConstIterator itAG = variant.begin();
		itAG != variant.end();
		++itAG)
		load(*itAG, root, animationGroups[itAG.index()]);
}

static void load(const Variant & variant, const TypeId & typeId, Textures & textures)
{
	for(Variant::ConstIterator itV = variant.begin();
		itV != variant.end();
		++itV)
	{
		const Variant vTexture = *itV;
		std::string file = vTexture.key("file");
		osg::Texture2D * texture = new osg::Texture2D();
		texture->setDataVariance(osg::Object::DYNAMIC);
		texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
		texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
		texture->setMaxAnisotropy(8.0f);
		std::string filePath = Geometry::path + typeId + "/" + file;
		osg::Image* image = osgDB::readImageFile(filePath.c_str());
		texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT); 
		texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
		texture->setImage(image);
		textures[itV.key()] = texture;
	}
}

std::string Geometry::path = "Common/Geometry/Graphics/";

Geometry::Geometry()
{
}

Geometry::~Geometry()
{
}

void Geometry::load(const Variant & variant)
{
	_root = NULL;
	if(Variant vTextures =  variant.tryKey("textures"))
		::load(vTextures, typeId, _textures);
	_root = NodeFactory::instance().create(_textures, variant.key("geometry"));
	_animationGroups.clear();
	const Variant vAnimation = variant.tryKey("animation");
	if(vAnimation)
		::load(vAnimation, _root, _animationGroups);
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

void Geometry::animate(unsigned int arg, float value)
{
	const AnimationGroup & animationGroup = _animationGroups[arg];
	for(AnimationGroup::const_iterator itA = animationGroup.begin();
		itA != animationGroup.end();
		++itA)
		(**itA)(value);
}