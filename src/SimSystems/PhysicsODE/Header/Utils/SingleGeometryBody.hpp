#ifndef _PhysicsODE_SingleGeometryBody_hpp_
#define _PhysicsODE_SingleGeometryBody_hpp_

#include "Utils/Body.hpp"
#include "Interface/IShape.hpp"

namespace Sim
{

namespace Interfaces
{
class IShape;
}

using Interfaces::IShape;

namespace PhysicsODE
{

class SingleGeometryBody :	public Body,
							public IShape
{
public:
	SingleGeometryBody();
	virtual void onShapeChanged(Interfaces::IShape * shape);
	virtual void onMassChanged(IMass * mass);	
private:
	dGeomID geomID_;
};

}

}

#endif _PhysicsODE_SingleGeometryBody_hpp_