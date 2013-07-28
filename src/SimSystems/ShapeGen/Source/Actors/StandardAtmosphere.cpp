#include "stdafx.h"

#include "Actors/StandardAtmosphere.hpp"

#include "Database/Variant.hpp"
#include "Database/ParserLua.hpp"
#include "Utils/VariantMathUtil.hpp"

using namespace Sim;
using namespace Sim::ShapeGen;

REGISER_ACTOR(StandardAtmosphere);

StandardAtmosphere::StandardAtmosphere()
{
}

void StandardAtmosphere::release()
{
}

StandardAtmosphere & StandardAtmosphere::instance()
{
	static StandardAtmosphere standardAtmosphere;
	return standardAtmosphere;
}

Actor * StandardAtmosphere::create(const std::string & nameIn, ModelTime modelTime, const Variant & variant)
{
	StandardAtmosphere::instance().load(variant);
	return &StandardAtmosphere::instance();
}

void StandardAtmosphere::load(const Variant & variant)
{
	Math::Vec3 wind(0.0, 0.0, 0.0);
	tryLoadFromTable(variant, "wind", wind);
	impl_.setBaseWind(wind);
}