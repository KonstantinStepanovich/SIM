#ifndef _Box3_hpp_
#define _Box3_hpp_

#include "Math/Scalar.hpp"
#include "Math/Vec3.hpp"

#include <xutility>
#include <assert.h>

namespace Sim
{

namespace Math
{

class Box3
{
public:
	Box3() : min(0.0, 0.0, 0.0), max(0.0, 0.0, 0.0)
	{
	}
	Box3(const Vec3 & point)
	{
		min = max = point;
	}
	Box3(const Vec3 & minIn, const Vec3 & maxIn) : min(minIn), max(maxIn)
	{
		assert(min.x() <= max.x() && min.y() <= max.y() && min.z() <= max.z());
	}
	void expand(const Vec3 & point)
	{
		max.setX(std::max(max.x(), point.x()));
		max.setY(std::max(max.y(), point.y()));
		max.setZ(std::max(max.z(), point.z()));
		min.setX(std::min(min.x(), point.x()));
		min.setY(std::min(min.y(), point.y()));
		min.setZ(std::min(min.z(), point.z()));
	}
	void expand(const Box3 & box)
	{
		expand(box.min);
		expand(box.max);
	}
	Vec3 getCenter() const
	{
		return (min + max) / 2;
	}
	Vec3 getSize() const
	{
		return max - min;
	}
	Real getRadius() const
	{
		return getSize().length() / 2;
	}
	bool isInside(const Vec3 & point) const
	{		
		return 	min.x() < point.x() && point.x() < max.x() &&
				min.y() < point.y() && point.y() < max.y() &&
				min.z() < point.z() && point.z() < max.z();
	}
	bool isInside(const Box3 & box) const
	{		
		return	isInside(box.min) &&
				isInside(box.max);
	}
	bool intersects(const Box3 & box) const
	{
		return	std::max(min.x(), box.min.x()) <= std::min(max.x(), box.max.x()) &&
				std::max(min.y(), box.min.y()) <= std::min(max.y(), box.max.y()) &&
				std::max(min.z(), box.min.z()) <= std::min(max.z(), box.max.z());
	}
	// float distance(const Vec3 & point) const
	// {
		// if(min.y() > point.y())
		// {
			// if(max.x() < point.x())
				// return (Vec3(max.x(), min.y()) - point).length();
			// else if(min.x() > point.x())
				// return (Vec3(min.x(), min.y()) - point).length();
			// else
				// return min.y() - point.y();
		// }
		// else if(max.y() < point.y())
		// {
			// if(max.x() < point.x())
				// return (Vec3(max.x(), max.y()) - point).length();
			// else if(min.x() > point.x())
				// return (Vec3(min.x(), max.y()) - point).length();
			// else
				// return point.y() - max.y();
		// }
		// else
		// {
			// if(max.x() < point.x())
				// return point.x() - max.x();
			// else if(min.x() > point.x())
				// return min.x() - point.x();
			// else
			// {
				// return	-std::min(	std::min(point.x() - min.x(), max.x() - point.x()),
									// std::min(point.y() - min.y(), max.y() - point.y()));
			// }
		// }	
	// }
	Vec3 min;
	Vec3 max;
};

}

}

#endif _Box3_hpp_