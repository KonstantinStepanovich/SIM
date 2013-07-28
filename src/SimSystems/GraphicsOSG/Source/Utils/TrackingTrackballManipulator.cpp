#include "stdafx.h"

#include "Objects/BodyObject.hpp"
#include "Utils/TrackingTrackballManipulator.hpp"

#include <osg/MatrixTransform>

using namespace Sim::GraphicsOSG;

TrackingTrackballManipulator::TrackingTrackballManipulator() : bodyToTrack_(NULL), useBodyFrame_(false)
{

}

osg::Matrixd TrackingTrackballManipulator::getMatrix() const
{
	if(bodyToTrack_ != NULL)
	{
		osg::Vec3 point = _center + bodyToTrack_->getMatrix().getTrans();
		osg::Matrixd mat = osg::Matrixd::translate(0.0,0.0,_distance) * osg::Matrixd::rotate(_rotation) * osg::Matrixd::translate(point);
		if(useBodyFrame_)
			mat = mat * osg::Matrixd::rotate(bodyToTrack_->getMatrix().getRotate());
		return mat;
	}
	else
		return osgGA::TrackballManipulator::getMatrix();
}

osg::Matrixd TrackingTrackballManipulator::getInverseMatrix() const
{
	if(bodyToTrack_ != NULL)
	{
		osg::Vec3 point = _center + bodyToTrack_->getMatrix().getTrans();
		osg::Matrixd mat = osg::Matrixd::translate(point);
		if(useBodyFrame_)
			mat.setRotate(bodyToTrack_->getMatrix().getRotate());
		mat = osg::Matrixd::inverse(mat) * osg::Matrixd::rotate(_rotation.inverse()) * osg::Matrixd::translate(0.0,0.0,-_distance);
		return mat;
	}
	else
		return osgGA::TrackballManipulator::getInverseMatrix();
}