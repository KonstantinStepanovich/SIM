#ifndef _InputOIS_Scene_hpp_
#define _InputOIS_Scene_hpp_

#include "ISystem.hpp"

#include <windows.h>

#include "Core/ModelTime/ActivityScheduler.hpp"

namespace OIS
{
class InputManager;
class Keyboard;
class Mouse;
class JoyStick;
}

namespace Sim
{

using namespace Framework;

namespace InputOIS
{

class System;
class Task;
class Object;
enum Command;
struct AxisCommand;
class KeyboardEventHandler;
class JoyStickEventHandler;

class Scene : public ISystemScene
{
public:
	Scene(System * system);
	~Scene();
	virtual void load(const char * scene);

	virtual ISystem * getSystem();
	virtual ISystemTask * getSystemTask();

	virtual ISystemObject * createObject(const ObjectName & objectName, const ObjectClass & objectClass);
	virtual void destroyObject(ISystemObject * systemObject);

	virtual void onEvent(ISubject* subject, Event event);
	virtual Event getEventsOfInterest() const;
	virtual Event getPotentialEvents() const;

	void update(Time time, ModelTime modelTime);
	void onCommand(Command command);
	void onAxisCommand(const AxisCommand & axisCommand);

	inline void addActivity(ModelTime time, Activity * activity) { scheduler_.schedule(time, activity); }
	inline void removeActivity(Activity * activity) { scheduler_.remove(activity); }
private:
	void createInputManager_(HWND hWnd);
	void destroyInputManager_();
	void processEvents_();

	Object * addObject_(Object * object);

	System*				system_;
	std::auto_ptr<Task>	task_;
	HWND				hWnd_;
	typedef std::list<Object*> Objects;
	Objects				objects_;

	enum { JOYSTICK_COUNT_MAX = 4 };

	std::auto_ptr<KeyboardEventHandler> keyboardEventHandler_;
	std::auto_ptr<JoyStickEventHandler> joyStickEventHandler[JOYSTICK_COUNT_MAX];

	OIS::InputManager*	inputManager_;
	OIS::Keyboard*		keyboard_;
	OIS::Mouse*			mouse_;	
	OIS::JoyStick*		joyStick_[JOYSTICK_COUNT_MAX];

	Sim::ActivityScheduler		scheduler_;
};

}

}

#endif _InputOIS_Scene_hpp_