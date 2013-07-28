#ifndef _ShapeGen_MassCalculator_hpp_
#define _ShapeGen_MassCalculator_hpp_

#include "Interface/IMass.hpp"

#include "Math/Scalar.hpp"

namespace Sim
{

using Interfaces::IMass;

namespace ShapeGen
{

IMass::Data makeSphereMass(Math::Real density, Math::Real radius);
IMass::Data makeBoxMass(Math::Real density, const Math::Vec3 & size);
IMass::Data makeCylinderMass(Math::Real density, Math::Real radius, Math::Real length);

}

}

#endif _ShapeGen_MassCalculator_hpp_