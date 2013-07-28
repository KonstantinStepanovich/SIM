#include "stdafx.h"
#include "SyncChangeController.hpp"

using namespace Sim::Framework;

void SyncChangeController::registerObserver(ISubject * subject, IObserver * observer)
{
	subject->setObserver(observer);
}

void SyncChangeController::unregisterObserver(ISubject * subject, IObserver * observer)
{
	subject->setObserver(NULL);
}

void SyncChangeController::execute()
{
	
}