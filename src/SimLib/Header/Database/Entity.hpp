#ifndef _Entity_hpp_
#define _Entity_hpp_

#include <string>
#include <map>

#include "Algorithm/Exception.hpp"
#include "String/StringStream.hpp"

namespace Sim
{

typedef std::string TypeId;

struct DescBase
{
	TypeId typeId;
};

template<typename DescType>
class DescLoader
{
public:
	virtual void load(const TypeId & typeId, DescType & desc) const
	{
	}
};

template<	typename DescType,
			template<typename> class LoaderType = DescLoader>
class Database
{
	Database() : loader_(NULL)
	{
	}
public:
	typedef DescType Desc;
	typedef LoaderType<DescType> Loader;
	typedef std::map<TypeId, DescType> Storage;
	void setLoader(const Loader * loader)
	{
		loader_ = loader;
	}
	inline DescType & regDesc(const TypeId & typeId) const
	{
		return storage_[typeId];
	}
	inline const DescType & getDesc(const TypeId & typeId) const
	{
		std::pair<Storage::iterator, bool> res = storage_.insert(Storage::value_type(typeId, Desc()));
		if(	res.second &&
			loader_ != NULL)
			loader_->load(typeId, res.first->second);
		return res.first->second;
	}
	static Database<DescType, LoaderType> & instance()
	{
		static Database<DescType, LoaderType> database;
		return database;
	}
	inline const Storage & getStorage() const { return storage_; }
private:
	mutable Storage storage_;
	const Loader * loader_;
};

}

#endif _Entity_hpp_