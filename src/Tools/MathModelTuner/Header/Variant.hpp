#ifndef _JVariant_hpp_
#define _JVariant_hpp_

#include "json/value.h"

typedef Json::Value Variant;

inline int getInt(const Variant & variant, const char * name, int defaultValue)
{
	const Variant & v = variant[name];
	return !v.empty() ? v.asInt() : defaultValue;
}

inline double getDouble(const Variant & variant, const char * name, double defaultValue)
{
	const Variant & v = variant[name];
	return !v.empty() ? v.asDouble() : defaultValue;
}

#endif _JVariant_hpp_