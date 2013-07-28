#include "stdafx.h"
#include "Utils/MassCalculator.hpp"
#include "Implementation/Mass.hpp"

#include <ode/mass.h>

using namespace Sim;
using namespace Sim::Interfaces;
using namespace Sim::ShapeGen;

IMass::Data convertMass(const dMass & mass)
{
	IMass::Data data;
	data.mass = mass.mass;
	data.centerOfMass = Math::Vec3(mass.c[0], mass.c[1], mass.c[3]);
	data.inertiaTensor.setValue(mass.I[0], mass.I[1 * 4],		mass.I[2 * 4],
								mass.I[1], mass.I[1 * 4 + 1],	mass.I[2 * 4 + 1],
								mass.I[2], mass.I[1 * 4 + 2],	mass.I[2 * 4 + 2]);
	return data;
}

IMass::Data Sim::ShapeGen::makeSphereMass(Math::Real density, Math::Real radius)
{
	if(density == NoDensity)
		return makeNoMass();
	dMass dmass;
	dMassSetSphere(&dmass, density, radius);
	return convertMass(dmass);
}

IMass::Data Sim::ShapeGen::makeBoxMass(Math::Real density, const Math::Vec3 & size)
{
	if(density == NoDensity)
		return makeNoMass();
	dMass dmass;
	dMassSetBox(&dmass, density, size.x(), size.y(), size.z());
	return convertMass(dmass);
}

IMass::Data Sim::ShapeGen::makeCylinderMass(Math::Real density, Math::Real radius, Math::Real length)
{
	if(density == NoDensity)
		return makeNoMass();
	dMass dmass;
	dMassSetCylinder(&dmass, density, 2, radius, length);
	return convertMass(dmass);
}
