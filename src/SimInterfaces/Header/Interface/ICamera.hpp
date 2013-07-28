#ifndef _ICamera_hpp_
#define _ICamera_hpp_

#include <string>
#include "Math/Vec3.hpp"
#include "Math/Quat.hpp"

namespace Sim
{

namespace Interfaces
{

class ICamera
{
public:
	enum Type
	{
		TYPE_VOID = -1,
		TYPE_TRACK_WORLD_POINT,
		TYPE_TRACK_OBJECT,
		TYPE_TRACK_OBJECT_LOCAL,
		TYPE_TRACK_OBJECT_LOCAL_VELOCITY
	};
	struct Data
	{
		Type		type;
		std::string objectToTrackName;
		Math::Vec3	pointToTrack;
		float		distance;
		Math::Vec3	center;
		Math::Quat	rotation;
	};
	virtual ~ICamera() {;}
	virtual void getCamera(Data & data) const = 0;
};

}

}

#endif _ICamera_hpp_