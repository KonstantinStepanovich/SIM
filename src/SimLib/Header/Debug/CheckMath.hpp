#ifndef _CheckMath_hpp_
#define _CheckMath_hpp_

#include "Math/Vec3.hpp"
#include "Math/Quat.hpp"
#include "Math/Mat3x3.hpp"

#include "Debug/CheckValue.hpp"

namespace Sim
{

inline void validate(const Math::Vec3 & vec)
{
	validate(vec.x());
	validate(vec.y());
	validate(vec.z());
}

inline void validate(const Math::Quat & quat)
{
	validate(quat.x());
	validate(quat.y());
	validate(quat.z());
	validate(quat.w());
}

inline void validate(const Math::Matrix3x3 & mat)
{
	validate(mat.getRow(0));
	validate(mat.getRow(1));
	validate(mat.getRow(2));
}

}

#endif _CheckMath_hpp_