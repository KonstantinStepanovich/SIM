#ifndef _Factory_hpp_
#define _Factory_hpp_

namespace Sim
{

template<typename Type, class Product>
class Factory
{
	Factory();
public:
	static Factory<Product> & instance()
	{
		static Factory<Product> factory;
		return factory;
	}
	Product * create(const Type & type)
	{
		CreateMap::iterator itCM = ñreateMap_.find(type);
		if(itCM != ñreateMap_.end())
			THROW_EXCEPTION(Exception, OSS("Unknown product type \"" << type << "\""));
		Product * product = (*itCM->second)();
		return product;
	}
private:
	typedef Product * (*Create)();
	typedef std::map<Type, Create> CreateMap;
	CreateMap createMap_;
};

}

#endif _Factory_hpp_