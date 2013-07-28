#ifndef _GraphicsOSG_TrackingTrackballManipulator_hpp_
#define _GraphicsOSG_TrackingTrackballManipulator_hpp_

#include "ISystem.hpp"

#include <osgGA/TrackballManipulator>

namespace Sim
{

namespace GraphicsOSG
{

class BodyObject;

class TrackingTrackballManipulator : public osgGA::TrackballManipulator
{
public:
	enum Mode
	{

	};
	TrackingTrackballManipulator();
	inline void setBodyToTrack(const BodyObject * bodyToTrack) { bodyToTrack_ = bodyToTrack; }
	inline void setUseBodyFrame(bool useBodyFrame) { useBodyFrame_ = useBodyFrame; }
	virtual osg::Matrixd getMatrix() const;
	virtual osg::Matrixd getInverseMatrix() const;
private:
	const BodyObject * bodyToTrack_;
	bool useBodyFrame_;
};

}

}

#endif _GraphicsOSG_TrackingTrackballManipulator_hpp_