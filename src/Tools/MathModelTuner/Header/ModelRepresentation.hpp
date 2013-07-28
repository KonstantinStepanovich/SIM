#ifndef _ModelRepresentation_hpp_
#define _ModelRepresentation_hpp_

#include "WidgetData/WidgetData.hpp"

#include <map>

struct TestRepresentation
{
	WidgetsDataCompCreate parameters;
	WidgetsDataCompCreate result;
};

class ModelRepresentation
{
public:
	ModelRepresentation();
	void load(const Variant & data);
	inline WidgetsDataCreate & parameters() { return parametersRepresentation_; }
	const TestRepresentation & test(const std::string & name) const;
private:
	WidgetsDataCreate parametersRepresentation_;
	typedef std::map<std::string, TestRepresentation> TestRepresentations;
	TestRepresentations testRepresentations_;	
};

class ModelRepresentationBase
{
public:
	static ModelRepresentationBase & instance();
	void load(const Variant & data);
	const ModelRepresentation & get(const std::string & name);
private:
	ModelRepresentationBase();
	typedef std::map<std::string, ModelRepresentation> ModelRepresentations;
	ModelRepresentations modelRepresentations_;
};

#endif _ModelRepresentation_hpp_