#ifndef _GraphicsOSG_Geometry_hpp_
#define _GraphicsOSG_Geometry_hpp_

#include "Database/Entity.hpp"
#include "Database/DatabaseUtil.hpp"

#include <vector>

#include <osg/ref_ptr>
#include <osg/Referenced>

namespace osg
{
class Node;
class Texture2D;
}

namespace Sim
{

class Variant;

namespace Interfaces
{
	class IEntity;
	class IAnimated;
}

using Interfaces::IEntity;
using Interfaces::IAnimated;

namespace GraphicsOSG
{

class Animation : public osg::Referenced
{
public:
	virtual ~Animation();
	virtual void operator()(float arg) const = 0;
};

typedef std::vector<float> AnimationArgs;
typedef std::vector<osg::ref_ptr<Animation>> AnimationGroup;
typedef std::vector<AnimationGroup> AnimationGroups;

typedef std::map<std::string, osg::ref_ptr<osg::Texture2D>> Textures;

class Geometry : public DescBase
{
public:
	static std::string path;
	Geometry();
	~Geometry();
	void load(const Variant & variant);
	void onEntityChanged(IEntity * entity);
	void onAnimationChanged(IAnimated * animated);
	void animate(unsigned int arg, float value);
	inline osg::Node * getNode() const { return _root; }
private:
	osg::ref_ptr<osg::Node> _root;
	AnimationGroups _animationGroups;
	Textures _textures;
};

void load(const Variant & variant, Geometry & geometry);


}

}

#endif _GraphicsOSG_Geometry_hpp_