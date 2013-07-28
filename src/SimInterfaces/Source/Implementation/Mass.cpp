#include "stdafx.h"

#include "Implementation/Mass.hpp"

#include <limits>

using namespace Sim;
using namespace Sim::Interfaces;

const float Sim::Interfaces::NoMass = std::numeric_limits<float>::infinity();
const float Sim::Interfaces::NoDensity = std::numeric_limits<float>::infinity();

Mass::Mass(const Data & data) : data_(data)
{
}

bool Mass::hasMass() const
{
	return data_.mass != NoMass;
}

bool Mass::getMass(Data & data) const
{
	data = data_;
	return data_.mass != NoMass;
}

IMass::Data Sim::Interfaces::makeNoMass()
{
	IMass::Data data;
	data.mass = NoMass;
	return data;
}
