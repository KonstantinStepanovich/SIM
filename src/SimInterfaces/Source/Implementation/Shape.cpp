#include "stdafx.h"

#include "Implementation/Shape.hpp"

using namespace Sim;
using namespace Sim::Interfaces;

Sphere::Sphere(Math::Real radius) : radius_(radius)
{

}

Math::Real Sphere::getRadius() const
{
	return radius_;
}

Box::Box(const Math::Vec3 & size) : size_(size)
{

}

const Math::Vec3 & Box::getSize() const
{
	return size_;
}

Cylinder::Cylinder(Math::Real height, Math::Real radius) : height_(height), radius_(radius)
{

}

const Math::Real Cylinder::getRadius() const
{
	return radius_;
}

const Math::Real Cylinder::getHeight() const
{
	return height_;
}
