#ifndef _Shape_hpp_
#define _Shape_hpp_

#include "Interface/IShape.hpp"

#include "Math/Vec3.hpp"

namespace Sim
{

namespace Interfaces
{

class Sphere : public ISphere
{
public:
	Sphere(Math::Real radius);
	virtual Math::Real getRadius() const;
protected:
	Math::Real radius_;
};

class Box : public IBox
{
public:
	Box(const Math::Vec3 & size);
	virtual const Math::Vec3 & getSize() const;
protected:
	Math::Vec3 size_;
};

class Cylinder : public ICylinder
{
public:
	Cylinder(Math::Real height, Math::Real radius);
	virtual const Math::Real getRadius() const;
	virtual const Math::Real getHeight() const;
protected:
	Math::Real height_;
	Math::Real radius_;
};

}

}

#endif _Shape_hpp_