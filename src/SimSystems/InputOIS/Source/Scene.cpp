#include "stdafx.h"

#include <iostream>
#include <xutility>
#include <algorithm>

#include "Scene.hpp"
#include "Objects/Airplane.hpp"
#include "Objects/Camera.hpp"
#include "System.hpp"
#include "Task.hpp"
#include "Implementation/InputEventHandler.hpp"
#include "Implementation/Commands.hpp"

#include "IEnvironmentManager.hpp"
#include "Events.hpp"
#include "Interface/IWindow.hpp"

#include "Database/Variant.hpp"
#include "Database/ParserLua.hpp"

#include "OIS/OISInputManager.h"
#include "OIS/OISException.h"
#include "OIS/OISKeyboard.h"
#include "OIS/OISMouse.h"
#include "OIS/OISJoyStick.h"
#include "OIS/OISEvents.h"

using namespace Sim;
using namespace Sim::Framework;
using namespace Sim::InputOIS;

using namespace OIS;

namespace Sim
{

namespace InputOIS
{

extern Managers managers;

}

}

Scene::Scene(System * system) : system_(system),
								inputManager_(NULL),
								hWnd_(NULL)
{
	std::fill(joyStick_, joyStick_ + JOYSTICK_COUNT_MAX, (OIS::JoyStick*)NULL);
	task_.reset(new Task(this));
}

Scene::~Scene()
{
	assert(objects_.empty());
	destroyInputManager_();
}

void Scene::load(const char * scene)
{
}

ISystem * Scene::getSystem()
{
	return system_;
}

ISystemTask * Scene::getSystemTask()
{
	return task_.get();
}

ISystemObject * Scene::createObject(const ObjectName & objectName, const ObjectClass & objectClass)
{
	if(objectClass == "Airplane")
		return addObject_(new Airplane(objectName, this));
	else if(objectClass == "Camera")
		return addObject_(new Camera("Camera", this));
	else
		return NULL;
}

void Scene::destroyObject(ISystemObject * systemObject)
{
	objects_.remove(static_cast<Object*>(systemObject));
	delete systemObject;
}

void Scene::onEvent(ISubject* subject, Event event)
{
	if(event & Interfaces::EVENT_WINDOW_CHANGED)
	{
		Interfaces::IWindow* window = dynamic_cast<Interfaces::IWindow*>(subject);
		HWND newHWnd = window->getWindowHandle();
		if(newHWnd != hWnd_)
		{
			destroyInputManager_();
			createInputManager_(newHWnd);
			hWnd_ = newHWnd;
		}
	}
}

Event Scene::getEventsOfInterest() const
{
	return Interfaces::EVENT_WINDOW_CHANGED;
}

Event Scene::getPotentialEvents() const
{
	return NO_EVENT;
}

void Scene::update(Time time, ModelTime modelTime)
{
	processEvents_();
	scheduler_.run(modelTime);
}

void Scene::onCommand(Command command)
{
	switch(command)
	{
		case COMMAND_PAUSE:
			managers.environmentManager->setPause(!managers.environmentManager->isPause());
			break;
		case COMMAND_DECELLERATE_MODEL_TIME:
			if(managers.environmentManager->getTimeDecelleration() > 1)
				managers.environmentManager->setTimeDecelleration(managers.environmentManager->getTimeDecelleration() * 2);
			else if(managers.environmentManager->getTimeAccelleration() > 1)
				managers.environmentManager->setTimeAcceleration(managers.environmentManager->getTimeAccelleration() / 2);
			else
				managers.environmentManager->setTimeDecelleration(2);
			break;
		case COMMAND_ACELLERATE_MODEL_TIME:
			if(managers.environmentManager->getTimeAccelleration() > 1)
				managers.environmentManager->setTimeAcceleration(managers.environmentManager->getTimeAccelleration() * 2);
			else if(managers.environmentManager->getTimeDecelleration() > 1)
				managers.environmentManager->setTimeDecelleration(managers.environmentManager->getTimeDecelleration() / 2);
			else
				managers.environmentManager->setTimeAcceleration(2);				
			break;
		case COMMAND_NORMALIZE_MODEL_TIME:
			managers.environmentManager->setTimeNormal();
			break;
		case COMMAND_RELOAD:
			managers.environmentManager->setReloadScene();
			break;
		default:
			std::for_each(objects_.begin(), objects_.end(), std::bind2nd(std::mem_fun1(&Object::onCommand), command));
			break;
	}
}

void Scene::onAxisCommand(const AxisCommand & axisCommand)
{
	//std::for_each(objects_.begin(), objects_.end(), std::bind2nd(std::mem_fun1(&Object::onAxisCommand), axisCommand));
	for(Objects::iterator itO = objects_.begin();
		itO != objects_.end();
		itO++)
	{
		(*itO)->onAxisCommand(axisCommand);
	}
}

void Scene::createInputManager_(HWND hWnd)
{
	static const char *deviceType[6] = {	"OISUnknown", "OISKeyboard", "OISMouse", "OISJoyStick",
											"OISTablet", "OISOther"};

	//This never returns null.. it will raise an exception on errors
	inputManager_ = InputManager::createInputSystem((size_t)hWnd);

	//Lets enable all addons that were compiled in:
	inputManager_->enableAddOnFactory(InputManager::AddOn_All);

	//Print debugging information
	unsigned int v = inputManager_->getVersionNumber();
	std::cout	<< "OIS Version: " << (v>>16 ) << "." << ((v>>8) & 0x000000FF) << "." << (v & 0x000000FF)
				<< "\nRelease Name: " << inputManager_->getVersionName()
				<< "\nManager: " << inputManager_->inputSystemName()
				<< "\nTotal Keyboards: " << inputManager_->getNumberOfDevices(OISKeyboard)
				<< "\nTotal Mice: " << inputManager_->getNumberOfDevices(OISMouse)
				<< "\nTotal JoySticks: " << inputManager_->getNumberOfDevices(OISJoyStick)
				<< "\n";

	//List all devices
	DeviceList list = inputManager_->listFreeDevices();
	for( DeviceList::iterator i = list.begin(); i != list.end(); ++i )
		std::cout << "\n\tDevice: " << deviceType[i->first] << " Vendor: " << i->second;

	std::cout << "\nLoading input profiles ...";
	VariantRoot variant;
	loadLua("FlightModel/Config/Input.lua", "input", variant);

	keyboard_ = (Keyboard*)inputManager_->createInputObject( OISKeyboard, true );
	if(Variant vKeyboard = variant.tryKey("keyboard"))
		keyboardEventHandler_.reset(new KeyboardEventHandler(this, keyboard_, vKeyboard));

	mouse_ = (Mouse*)inputManager_->createInputObject( OISMouse, true );
	const MouseState &ms = mouse_->getMouseState();
	ms.width = 100;
	ms.height = 100;

	if(Variant joyStick = variant.tryKey("joyStick"))
	{
		try
		{
			//This demo uses at most 4 joysticks - use old way to create (i.e. disregard vendor)
			int numSticks = min(inputManager_->getNumberOfDevices(OISJoyStick), JOYSTICK_COUNT_MAX);
			for( int i = 0; i < numSticks; ++i )
			{
				joyStick_[i] = (JoyStick*)inputManager_->createInputObject( OISJoyStick, true );
				joyStickEventHandler[i].reset(new JoyStickEventHandler(this, joyStick_[i], joyStick));
				std::cout << "\n\nCreating Joystick " << (i + 1)
					<< "\n\tAxes: " << joyStick_[i]->getNumberOfComponents(OIS_Axis)
					<< "\n\tSliders: " << joyStick_[i]->getNumberOfComponents(OIS_Slider)
					<< "\n\tPOV/HATs: " << joyStick_[i]->getNumberOfComponents(OIS_POV)
					<< "\n\tButtons: " << joyStick_[i]->getNumberOfComponents(OIS_Button)
					<< "\n\tVector3: " << joyStick_[i]->getNumberOfComponents(OIS_Vector3);
			}
		}
		catch(OIS::Exception &ex)
		{
			std::cout << "\nException raised on joystick creation: " << ex.eText << std::endl;
		}
	}
}

void Scene::destroyInputManager_()
{
	if(inputManager_ != NULL)
	{
		InputManager::destroyInputSystem(inputManager_);
		inputManager_ = NULL;
	}
}

void Scene::processEvents_()
{
	if(inputManager_ == NULL)
		return;
	keyboard_->capture();
	//mouse_->capture();
	for( int i = 0; i < JOYSTICK_COUNT_MAX ; ++i )
	{
		if( joyStick_[i] )
		{
			joyStick_[i]->capture();
		}
	}
}

InputOIS::Object * Scene::addObject_(InputOIS::Object * object)
{
	objects_.push_back(object);
	return object;
}