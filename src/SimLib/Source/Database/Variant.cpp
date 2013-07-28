#include "Database/Variant.hpp"

#include <fstream>

#include "json/reader.h"
#include "json/writer.h"

namespace Sim
{

const Variant VariantConstIterator::operator *()
{
	if(parent_->data().type() == Json::arrayValue)
		return Variant(const_cast<Json::Value&>(*it_), parent_, it_.index());
	else
		return Variant(const_cast<Json::Value&>(*it_), parent_, it_.memberName());
}

Variant VariantIterator::operator *()
{
	if(parent_->data().type() == Json::arrayValue)
		return Variant(*it_, parent_, it_.index());
	else
		return Variant(*it_, parent_, it_.memberName());
}

static Json::Value jsonValueNull;

const Variant Variant::Null(jsonValueNull);

Variant::Variant(Json::Value & ref) : ref_(ref), parent_(NULL)
{
}

Variant::Variant(Json::Value & ref, const Variant * parent, unsigned int index) : ref_(ref), parent_(parent), indexUInt_(index)
{
}

Variant::Variant(Json::Value & ref, const Variant * parent, const char * index) : ref_(ref), parent_(parent), indexStr_(index)
{
}

Variant::operator int() const
{
	return ref_.asInt();
}

Variant::operator unsigned int() const
{
	return ref_.asUInt();
}

Variant::operator float() const
{
	return (float)ref_.asDouble();
}

Variant::operator double() const
{
	return ref_.asDouble();
}

Variant::operator const char * () const
{
	return ref_.asCString();
}

Variant::operator std::string () const
{
	return ref_.asString();
}

Variant::operator bool () const
{
	return ref_.type() != Json::nullValue && ref_.asBool();
}

Variant Variant::operator[](unsigned int index)
{
	return Variant(ref_[index], this, index);
}

Variant Variant::operator[](const char * index)
{
	return Variant(ref_[index], this, index);
}

Variant Variant::index(unsigned int val) const
{
	if(	ref_.type() != Json::arrayValue ||
		val >= ref_.size())
	{
		std::ostringstream oss;
		oss << "Index not found ";
		dumpPath_(oss);
		oss << ".[" << val << "]";
		THROW_EXCEPTION(Exception, oss.str().c_str());
	}
	return Variant(ref_[val], this, val);
}

Variant Variant::key(const char * val) const
{
	if(	ref_.type() != Json::objectValue ||
		!ref_.isMember(val))
	{
		std::ostringstream oss;
		oss << "Index not found ";
		dumpPath_(oss);
		oss << ".[" << val << "]";
		THROW_EXCEPTION(Exception, oss.str().c_str());
	}
	return Variant(ref_[val], this, val);
}

Variant Variant::index(unsigned int val, const Variant & def) const
{
	if(	ref_.type() != Json::arrayValue ||
		val >= ref_.size())
		return def;
	else
		return Variant(ref_[val], this, val);
}

Variant Variant::key(const char * val, const Variant & def) const
{
	if(	ref_.type() != Json::objectValue ||
		!ref_.isMember(val))
		return def;
	else
		return Variant(ref_[val], this, val);
}

Variant Variant::tryIndex(unsigned int index) const
{
	return Variant(ref_[index], this, index);
}

Variant Variant::tryKey(const char * key) const
{
	return Variant(ref_[key], this, key);
}

Variant::ConstIterator Variant::begin() const
{
	return ConstIterator(((const Json::Value&)ref_).begin(), this);
}

Variant::ConstIterator Variant::end() const
{
	return ConstIterator(((const Json::Value&)ref_).end(), this);
}

Variant::Iterator Variant::begin()
{
	return Iterator(ref_.begin(), this);
}

Variant::Iterator Variant::end()
{
	return Iterator(ref_.end(), this);
}

size_t Variant::size() const
{
	return ref_.size();
}

bool Variant::empty() const
{
	return ref_.empty();
}

void Variant::setInt(int val)
{
	ref_ = Json::Value(val);
}

int Variant::getInt() const
{
	if(	ref_.type() != Json::intValue &&
		ref_.type() != Json::uintValue &&
		ref_.type() != Json::realValue)
	{
		std::ostringstream oss;
		oss << "Index ";
		dumpPath_(oss);
		oss << " is not an integer value";
		THROW_EXCEPTION(Exception, oss.str().c_str());
	}
	return ref_.asInt();
}

void Variant::setReal(double val)
{
	ref_ = Json::Value(val);
}

double Variant::getReal() const
{
	if(	ref_.type() != Json::intValue &&
		ref_.type() != Json::uintValue &&
		ref_.type() != Json::realValue)
	{
		std::ostringstream oss;
		oss << "Index ";
		dumpPath_(oss);
		oss << " is not a real value";
		THROW_EXCEPTION(Exception, oss.str().c_str());
	}
	return ref_.asDouble();
}

void Variant::setBool(bool val)
{
	ref_ = Json::Value(val);
}

double Variant::getBool() const
{
	if(ref_.type() != Json::booleanValue)
	{
		std::ostringstream oss;
		oss << "Index ";
		dumpPath_(oss);
		oss << " is not a boolean value";
		THROW_EXCEPTION(Exception, oss.str().c_str());
	}
	return ref_.asBool();
}

void Variant::setString(const char * val)
{
	ref_ = Json::Value(val);
}

const char * Variant::getString() const
{
	if(ref_.type() != Json::stringValue)
	{
		std::ostringstream oss;
		oss << "Index ";
		dumpPath_(oss);
		oss << " is not a string value";
		THROW_EXCEPTION(Exception, oss.str().c_str());
	}
	return ref_.asCString();
}

void Variant::dumpPath_(std::ostringstream & oss) const
{
	if(parent_ != NULL)
	{
		parent_->dumpPath_(oss);
		oss << ".";
		if(!indexStr_.empty())
			oss << "[\"" << indexStr_ << "\"]";
		else
			oss << "[" << indexUInt_ << "]";
	}
	else if(!indexStr_.empty())
		oss << "[\"" << indexStr_ << "\"]";
}

VariantRoot::VariantRoot() : Variant(value_)
{
}

VariantRoot::VariantRoot(const VariantRoot & variantRoot) : Variant(value_), value_(variantRoot.value_)
{
}

}