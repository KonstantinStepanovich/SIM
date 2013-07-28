#include "Database/ParserJson.hpp"
#include "Database/Variant.hpp"

#include "json/reader.h"
#include "json/writer.h"

#include <fstream>

void Sim::loadJson(const char * path, const char * table, Variant & variant)
{
	Json::Reader reader;
	std::ifstream ifs;
	ifs.open(path);
	if(ifs.is_open())
	{
		reader.parse(ifs, variant.data(), true);
		variant.setFileName(path);
	}
	else
		THROW_EXCEPTION(Exception, OSS("Unable to open file \"" << path << "\""));
	ifs.close();
}

void Sim::saveJson(const char * path, const char * table, const Variant & variant)
{
	Json::StyledStreamWriter writer;
	std::ofstream ofs;
	ofs.open(path);
	if(ofs.is_open())
	{
		if(strlen(table) > 0)
			writer.write(ofs, variant.key(table).data());
		else
			writer.write(ofs, variant.data());
	}
	else
		THROW_EXCEPTION(Exception, OSS("Unable to open file \"" << path << "\""));
	ofs.close();
}