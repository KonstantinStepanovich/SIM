#ifndef _Synchronization_hpp_
#define _Synchronization_hpp_

namespace Sim
{

namespace Framework
{

typedef unsigned long long Event;

const Event NO_EVENT = 0;

class ISubject;

class IObserver
{
public:
	virtual ~IObserver() {;}
	virtual Event getEventsOfInterest() const = 0;
	virtual void onEvent(ISubject * subject, Event event) = 0;
};

class ISubject
{
public:
	ISubject() : observer_(NULL) {;}
	virtual ~ISubject() {;}
	//inline Event getEventsOfInterest() const
	//{
	//	return (observer_ != NULL) ? observer_->getEventsOfInterest() : NoEvent;
	//}
	virtual Event getPotentialEvents() const = 0;
	inline void setObserver(IObserver * observer) { observer_ = observer; }
	inline void notifyEvent(Event event)
	{
		Event eventToProcess;
		if(	observer_ != NULL &&
			(eventToProcess = (observer_->getEventsOfInterest() & event)) != NO_EVENT)
			observer_->onEvent(this, eventToProcess);
	}
private:
	IObserver * observer_;
};

class IChangeController
{
public:
	virtual ~IChangeController() {;}
	virtual void registerObserver(ISubject * subject, IObserver * observer) = 0;
	virtual void unregisterObserver(ISubject * subject, IObserver * observer) = 0;
	virtual void execute() = 0;
};

}

}

#endif _Synchronization_hpp_