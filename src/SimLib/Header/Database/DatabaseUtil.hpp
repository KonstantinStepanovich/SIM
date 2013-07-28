#ifndef _DatabaseUtil_hpp_
#define _DatabaseUtil_hpp_

#include "Database/Entity.hpp"
#include "Database/VariantUtil.hpp"
#include "Database/Parser.hpp"

#include "boost/filesystem.hpp"

namespace Sim
{

template<class Desc>
inline void loadDescFile(	const TypeId & typeId,
							const char * dir, const char * ext,
							const Parser::Load & parser, const char * table,
							Desc & desc)
{
	VariantRoot variant;
	parser((dir + typeId + ext).c_str(), table, variant);
	load(variant, desc);
}

template<class Desc>
inline void saveDescFile(const TypeId & typeId,
						 const char * dir, const char * ext,
						 const Parser::Save & saver, const char * table,
						 const Desc & desc)
{
	VariantRoot variant;
	save(variant, desc);
	saver((dir + typeId + ext).c_str(), table, variant);	
}

template<class Desc>
inline void load(const Variant & variant, Database<Desc> & database)
{
	for(Variant::ConstIterator itV = variant.begin();
		itV != variant.end();
		++itV)
	{
		load(*itV, database.regDesc(itV.key()));
	}
}

template<class Desc>
inline void save(const Variant & variant, Database<Desc> & database)
{
	const Database<Desc>::Storage & storage = database.getStorage();
	for(Database<Desc>::Storage::const_iterator itS = database.begin();
		itS != storage.end();
		++itS)
	{
		save(itS->second, variant[itS->first]);
	}
}

template<class Desc>
inline void load(	const char * dir, const char * ext,
					const Parser::Load & parser, const char * table,
					Database<Desc> & database)
{
	boost::filesystem::path path(dir);
	if(	boost::filesystem::exists(dir) &&
		boost::filesystem::is_directory(dir))
	{
		for(boost::filesystem::directory_iterator it = boost::filesystem::directory_iterator(path);
			it != boost::filesystem::directory_iterator();
			++it)
		{
			const boost::filesystem::path & item = *it;
			if(	boost::filesystem::is_regular_file(item) &&
				item.extension() == ext)
			{
				Desc & desc = database.regDesc(item.stem());
				VariantRoot variant;
				loadDescFile(	item.stem().c_str(),
								dir, ext,
								parser, table,
								desc);
			}	
		}
	}
}

template<typename DescType>
class LoaderFile : public DescLoader<DescType>
{
public:
	LoaderFile( const char * dir, const char * ext, const Parser::Load & parser, const char * table) :
		dir_(dir), ext_(ext), parser_(parser), table_(table)
	{
	}
	virtual void load(const TypeId & typeId, DescType & desc) const
	{
		loadDescFile(typeId, dir_, ext_, parser_, table_, desc);
	}
private:
	const char * dir_;
	const char * ext_;
	Parser::Load parser_;
	const char * table_;
};

}
	
#endif _DatabaseUtil_hpp_