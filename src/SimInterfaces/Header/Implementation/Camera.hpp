#ifndef _Camera_hpp_
#define _Camera_hpp_

#include "Interface/ICamera.hpp"

namespace Sim
{

namespace Interfaces
{

class Camera : public ICamera
{
public:
	typedef std::string ObjectName;
	Camera();
	void setType(Type typeIn) { data.type = typeIn; }
	void setObjectToTrackName(const ObjectName & objectName) { data.objectToTrackName = objectName; }	
	void setPointToTrack(const Math::Vec3 & pointToTrack) { data.pointToTrack = pointToTrack; }
	void setDistance(float distance) { data.distance = distance; }	
	void setCenter(const Math::Vec3 & center) { data.center = center; }	
	void setRotation(const Math::Quat & rotation) { data.rotation = rotation; }	
	void setData(const Data & dataIn) { data = dataIn; }
	virtual void getCamera(Data & dataOut) const;
protected:
	Data data;
};

}

}

#endif _Body_hpp_