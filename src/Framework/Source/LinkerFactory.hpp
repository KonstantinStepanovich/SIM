template<class SystemSceneSourceType, class SystemSceneDestType>
class TypedSystemSceneEventTranslator : public ISceneLinker
{
public:
	typedef SystemSceneSourceType SystemSceneSource;
	typedef SystemSceneDestType SystemSceneDest;
	TypedSystemSceneEventTranslator(const ISystemScene * source, ISystemScene * dest) :
		source_(static_cast<SystemSceneSourceType*>(source)),
		dest_(static_cast<SystemSceneDestType*>(dest))
	{
	}
protected:
	const SystemSceneSourceType * const source_;
	SystemSceneDestType * const dest_;
};

template<class SystemObjectSourceType, class SystemObjectDestType>
class TypedSystemObjectEventTranslator : public IObjectLinker
{
public:
	typedef SystemObjectSourceType SystemObjectSource;
	typedef SystemObjectDestType SystemObjectDest;
	TypedSystemObjectEventTranslator(const ISystemObject * source, ISystemObject * dest) :
		source_(static_cast<SystemObjectSourceType*>(source)),
		dest_(static_cast<SystemObjectDestType*>(dest))
	{
	}
protected:
	const SystemObjectSourceType * const source_;
	SystemObjectDestType * const dest_;
};


//-=SystemSceneEventTranslatorFactory=-

class SystemSceneEventTranslatorFactory
{
public:
	typedef ISystemSceneEventTranslator * (*Creator)(ISystemScene * source, ISystemScene * dest);
	static SystemSceneEventTranslatorFactory & instance();
	void reg(SystemName source, SystemName dest, Creator creator);
	ISystemSceneEventTranslator * create(ISystemScene * source, ISystemScene * dest);
private:
	SystemSceneEventTranslatorFactory() {;}
	typedef std::map<SystemName, Creator> CreatorsBySystemTo;
	typedef std::map<SystemName, CreatorsBySystemTo> Creators;
	Creators creators_;
};

SystemSceneEventTranslatorFactory & SystemSceneEventTranslatorFactory::instance()
{
	static SystemSceneEventTranslatorFactory systemSceneEventTranslatorFactory;
	return systemSceneEventTranslatorFactory;
}

void SystemSceneEventTranslatorFactory::reg(SystemName from, SystemName to, Creator creator)
{
	Creator & newCreator = creators_[from][to];
	assert(newCreator == NULL);
	newCreator = creator;
}

ISystemSceneEventTranslator * SystemSceneEventTranslatorFactory::create(ISystemScene * from, ISystemScene * to)
{
	Creators::const_iterator itC1 = creators_.find(from->getName());
	if(itC1 != creators_.end())
	{
		CreatorsBySystemTo::const_iterator itC2 = itC1->second.find(from->getName());
		if(itC2 != itC1->second.end())
		{
			return *(*itC2)(from, to);
		}
	}
	return NULL;
}

//-=SystemObjectEventTranslatorFactory=-

class SystemObjectEventTranslatorFactory
{
public:
	typedef ISystemSceneEventTranslator * (*Creator)(ISystemObject * source, ISystemObject * dest);
	static SystemObjectEventTranslatorFactory & instance();
	void reg(SystemName source, SystemName dest, ObjectClass objectClass, Creator creator);
	ISystemObjectEventTranslator * create(ISystemObject * source, ISystemObject * dest);
private:
	SystemObjectEventTranslatorFactory() {;}
	typedef std::map<ObjectClass, Creator> CreatorsByObjectClass;
	typedef std::map<SystemName, CreatorsByObjectClass> CreatorsBySystemTo;
	typedef std::map<SystemName, CreatorsBySystemTo> Creators;
	Creators creators_;
};

SystemObjectEventTranslatorFactory & SystemObjectEventTranslatorFactory::instance()
{
	static SystemObjectEventTranslatorFactory systemObjectEventTranslatorFactory;
	return systemObjectEventTranslatorFactory;
}

void SystemObjectEventTranslatorFactory::reg(SystemName from, SystemName to, ObjectClass objectClass, Creator creator)
{
	Creator & newCreator = creators_[from][to][objectClass];
	assert(newCreator == NULL);
	newCreator = creator;
}

ISystemObjectEventTranslator * SystemObjectEventTranslatorFactory::create(ISystemObject * from, ISystemObject * to)
{
	assert(from->getClass() == to->getClass());
	Creators::const_iterator itC1 = creators_.find(from);
	if(itC1 != creators_.end())
	{
		CreatorsBySystemTo::const_iterator itC2 = itC1->second.find(to);
		if(itC2 != creators_.end())
		{
			CreatorsByObjectClass::const_iterator itC3 = itC2->second.find(from->getClass());
			if(itC3 != creators_.end())
			{
				return (*itC3->second)(from, to);
			}
		}
	}
	return NULL;
}
