#ifndef _Variant_hpp_
#define _Variant_hpp_

#include "Json/Value.h"

#include "Algorithm/Exception.hpp"

namespace Json
{
class Value;
}

namespace Sim
{

class Variant;

class VariantConstIterator
{
public:
	VariantConstIterator(const Json::Value::const_iterator & it, const Variant * parent) : it_(it), parent_(parent) {;}
	inline bool operator == (const VariantConstIterator & constIterator) { return it_ == constIterator.it_; }
	inline bool operator != (const VariantConstIterator & constIterator) { return it_ != constIterator.it_; }
	inline VariantConstIterator operator++() { return VariantConstIterator(++it_, parent_); }
	inline VariantConstIterator operator--() { return VariantConstIterator(--it_, parent_); }
	inline size_t operator-(const VariantConstIterator & i) const { return it_ - i.it_; }
	const Variant operator *();
	const int index() const { return it_.index(); }
	const char * key() const { return it_.memberName(); }
private:
	Json::Value::const_iterator it_;
	const Variant * parent_;
};

class VariantIterator
{
public:
	VariantIterator(const Json::Value::iterator & it, const Variant * parent) : it_(it), parent_(parent) {;}
	inline bool operator == (const VariantIterator & constIterator) const { return it_ == constIterator.it_; }
	inline bool operator != (const VariantIterator & constIterator) const { return it_ != constIterator.it_; }
	inline VariantIterator operator++() { return VariantIterator(++it_, parent_); }
	inline VariantIterator operator--() { return VariantIterator(--it_, parent_); }
	inline size_t operator-(const VariantIterator & i) const { return it_ - i.it_; }
	Variant operator *();
	const int index() const { return it_.index(); }
	const char * key() const { return it_.memberName(); }
private:
	Json::Value::iterator it_;
	const Variant * parent_;
};

DECLARE_EXCEPTION(VariantException, Exception)

class Variant
{
public:	
	typedef VariantException Exception;
	typedef VariantConstIterator ConstIterator;
	typedef VariantIterator Iterator;
	static const Variant Null;
	Variant(Json::Value & ref);
	Variant(Json::Value & ref, const Variant * parent, unsigned int index);
	Variant(Json::Value & ref, const Variant * parent, const char * key);
	inline void setFileName(const std::string & indexStr) { indexStr_ = indexStr; }
	inline Json::Value & data() { return ref_; }
	inline const Json::Value & data() const { return ref_; }
	template<typename T>
	inline Variant & operator = (const T & t) { ref_ = t; return *this; }
	operator int() const;
	operator unsigned int() const;
	operator float() const;
	operator double() const;
	operator const char * () const;
	operator std::string () const;
	operator bool () const;
	Variant operator[](unsigned int index);
	Variant operator[](const char * key);
	Variant index(unsigned int val) const;
	Variant key(const char * val) const;
	Variant index(unsigned int val, const Variant & def) const;
	Variant key(const char * val, const Variant & def) const;
	Variant tryIndex(unsigned int index) const;
	Variant tryKey(const char * key) const;
	ConstIterator begin() const;
	ConstIterator end() const;
	Iterator begin();
	Iterator end();
	size_t size() const;
	bool empty() const;
	void setInt(int val);
	int getInt() const;
	void setReal(double val);
	double getReal() const;
	void setBool(bool val);
	double getBool() const;
	void setString(const char * val);
	const char * getString() const;	
	template<typename T> inline T get() const { return (T)getInt(); }
	template<> inline float get<float>() const { return (float)getReal(); }
	template<> inline double get<double>() const { return (double)getReal(); }
	template<> inline const char * get<const char *>() const { return getString(); }
private:
	virtual void dumpPath_(std::ostringstream & oss) const;
	Json::Value & ref_;
	unsigned int indexUInt_;
	std::string indexStr_;
	const Variant * parent_;
};

class VariantRoot : public Variant
{
public:
	VariantRoot();
	VariantRoot(const VariantRoot & variantRoot);
	template<typename T>
	VariantRoot(const T & val) : Variant(value_), value_(val) {;}
private:
	Json::Value value_;
};

}

#endif _Variant_hpp_