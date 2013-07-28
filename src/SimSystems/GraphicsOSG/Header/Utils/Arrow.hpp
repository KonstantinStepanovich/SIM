#ifndef _GraphicsOSG_Arrow_hpp_
#define _GraphicsOSG_Arrow_hpp_

#include <osg/ref_ptr>
#include <osg/Referenced>
#include <osg/Vec3>
#include <osg/Vec4>

namespace osg
{
class Node;
class Group;
class Geode;
class ShapeDrawable;
class MatrixTransform;
}

namespace Sim
{

namespace GraphicsOSG
{

class Arrow : public osg::Referenced
{
public:
	Arrow(	const osg::Vec3 & vec = osg::Vec3(1.0f, 0.0f, 0.0f),
			float zeroDistance = 0.0f,
			float radius = 0.1f,
			const osg::Vec4 & color = osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	void set(const osg::Vec3 & vec);
	inline void setZeroDistance(float zeroDistance);
	void setColor(const osg::Vec4 & color);
	osg::Node * getNode() const;
private:
	osg::ref_ptr<osg::Group> _group;

	osg::ref_ptr<osg::MatrixTransform> _matrixTransformTrunk;
	osg::ref_ptr<osg::Geode> _geodeTrunk;
	osg::ref_ptr<osg::ShapeDrawable> _trunk;

	osg::ref_ptr<osg::MatrixTransform> _matrixTransformCap;
	osg::ref_ptr<osg::Geode> _geodeCap;
	osg::ref_ptr<osg::ShapeDrawable> _cap;
	
	osg::Vec3 _vec;
	float _zeroDistance;
	osg::Vec4 _color;

	void update_();
};

}

}

#endif _GraphicsOSG_Arrow_hpp_