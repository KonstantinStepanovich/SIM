#include "stdafx.h"

#include "Utils/Arrow.hpp"

#include "Debug/CheckMath.hpp"

#include <osg/Group>
#include <osg/Geode>
#include <osg/MatrixTransform>
#include <osg/ShapeDrawable>

using namespace Sim::GraphicsOSG;

Arrow::Arrow(const osg::Vec3 & vec,
			 float zeroDistance,
			 float radius,
			 const osg::Vec4 & color) :	_vec(vec),
										_zeroDistance(zeroDistance),
										_color(color),
										_group(new osg::Group()),
										_matrixTransformTrunk(new osg::MatrixTransform()),
										_geodeTrunk(new osg::Geode()),
										_matrixTransformCap(new osg::MatrixTransform()),
										_geodeCap(new osg::Geode())
{
	_matrixTransformTrunk->setName("arrow trunk");
	_trunk = new osg::ShapeDrawable(new osg::Cylinder(osg::Vec3(0.0f,0.0f,0.0f), radius, 1.0f));
	_trunk->setColor(color);
	_geodeTrunk->addDrawable(_trunk);
	_matrixTransformTrunk->addChild(_geodeTrunk);
	_group->addChild(_matrixTransformTrunk);

	_matrixTransformCap->setName("arrow cap");
	_cap = new osg::ShapeDrawable(new osg::Cone(osg::Vec3(0.0f,0.0f,0.0f), 3 * radius, 3 * 5 * radius));
	_cap->setColor(color);
	_geodeCap->addDrawable(_cap);
	_matrixTransformCap->addChild(_geodeCap);
	_group->addChild(_matrixTransformCap);

	update_();
}

void Arrow::set(const osg::Vec3 & vec)
{
	_vec = vec;
	update_();
}

void Arrow::setZeroDistance(float zeroDistance)
{
	_zeroDistance = zeroDistance;
	update_();
}

void Arrow::setColor(const osg::Vec4 & color)
{
	_trunk->setColor(color);
	_cap->setColor(color);
}

osg::Node * Arrow::getNode() const
{
	return _group;
}

void Arrow::update_()
{
	const float length = _vec.length();
	if(length > 1.0E-3)
	{
		_group->setNodeMask(std::numeric_limits<osg::Node::NodeMask>::max());

		const osg::Vec3 vecNorm = _vec / length;
		const osg::Vec3 start = vecNorm * _zeroDistance;
		//Trunk
		{
			osg::Matrix matrix;
			osg::Quat rot;
			rot.makeRotate(osg::Vec3(0.0f, 0.0f, -1.0f), vecNorm);
			matrix.setRotate(rot);	
			//matrix.postMultScale(osg::Vec3(length, 1.0f, 1.0f));		
			osg::Matrix scaleMatrix;
			//scaleMatrix.makeScale(length, 1.0f, 1.0f);
			//matrix = scaleMatrix * matrix;
			matrix(2, 0) *= length;
			matrix(2, 1) *= length;
			matrix(2, 2) *= length;
			matrix.setTrans(start + _vec * 0.5f);
			validate(matrix.ptr(), 16);
			_matrixTransformTrunk->setMatrix(matrix);
		}
		//Cap
		{
			osg::Matrix matrix;
			osg::Quat rot;
			rot.makeRotate(osg::Vec3(0.0f, 0.0f, 1.0f), vecNorm);
			matrix.setRotate(rot);
			matrix.setTrans(start + _vec);
			validate(matrix.ptr(), 16);
			_matrixTransformCap->setMatrix(matrix);
		}
	}
	else
		_group->setNodeMask(0);
}