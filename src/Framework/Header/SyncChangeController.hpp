#ifndef _SyncChangeController_hpp_
#define _SyncChangeController_hpp_

#include "Synchronization.hpp"

namespace Sim
{

namespace Framework
{

class SyncChangeController : public IChangeController
{
public:
	virtual void registerObserver(ISubject * subject, IObserver * observer);
	virtual void unregisterObserver(ISubject * subject, IObserver * observer);
	virtual void execute();
};

}

}

#endif _SyncChangeController_hpp_