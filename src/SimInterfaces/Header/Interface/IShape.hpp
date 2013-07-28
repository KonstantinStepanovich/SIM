#ifndef _IShape_hpp_
#define _IShape_hpp_

#include "Math/Scalar.hpp"
#include "Math/Vec3.hpp"

namespace Sim
{

namespace Interfaces
{

class IShape
{
public:
	enum Type { SHAPE_TYPE_VOID = -1,
				SHAPE_TYPE_SPHERE,
				SHAPE_TYPE_BOX,
				SHAPE_TYPE_CYLINDER,
				SHAPE_TYPE_MAX };
	virtual ~IShape() {;}
	virtual Type getType() const = 0;
};

class ISphere : public IShape
{
public:
	virtual ~ISphere() {;}
	virtual Math::Real getRadius() const = 0;
	virtual Type getType() const { return SHAPE_TYPE_SPHERE; }
};

class IBox : public IShape
{
public:
	virtual ~IBox() {;}
	virtual const Math::Vec3 & getSize() const = 0;
	virtual Type getType() const { return SHAPE_TYPE_BOX; }
};

class ICylinder : public IShape
{
public:
	virtual ~ICylinder() {;}
	virtual const Math::Real getRadius() const = 0;
	virtual const Math::Real getHeight() const = 0;
	virtual Type getType() const { return SHAPE_TYPE_CYLINDER; }
};

}

}

#endif _IShape_hpp_