#include "stdafx.h"

#include "Implementation/InputEventHandler.hpp"
#include "Implementation/Commands.hpp"
#include "Scene.hpp"

#include <iostream>

#include "String/StringStream.hpp"
#include "Algorithm/Exception.hpp"
#include "Database/Variant.hpp"

#include "OIS/OISKeyboard.h"
#include "OIS/OISJoystick.h"
#include "OIS/OISEvents.h"

using namespace Sim;
using namespace Sim::InputOIS;

class KeyboardEventHandler::KeyListener : public OIS::KeyListener
{
public:
	KeyListener(KeyboardEventHandler * handler) : handler_(handler) {}
	~KeyListener() {}
	bool keyPressed( const OIS::KeyEvent &arg )
	{
		return handler_->onKeyEvent(arg.key, KeyboardEventHandler::KEY_EVENT_PRESSED);
	}
	bool keyReleased( const OIS::KeyEvent &arg )
	{
		return handler_->onKeyEvent(arg.key, KeyboardEventHandler::KEY_EVENT_RELEASED);
	}
private:
	KeyboardEventHandler * handler_;
};

KeyboardEventHandler::KeyboardEventHandler(Scene * scene, OIS::Keyboard * keyboard, const Variant & variant) : scene_(scene)
{
	for(int keyCode = OIS::KC_UNASSIGNED;
		keyCode < OIS::KC_MEDIASELECT;
		keyCode++)
	{
		keyMap_[keyboard->getAsString((OIS::KeyCode)keyCode)] = (OIS::KeyCode)keyCode;
	}
	KeyCommands keyCommands;
	keyCommands.command[KEY_EVENT_PRESSED] = keyCommands.command[KEY_EVENT_RELEASED] = COMMAND_VOID;
	keyboardCommands_.resize(OIS::KC_MEDIASELECT - OIS::KC_UNASSIGNED + 1, keyCommands);
	keyListener_.reset(new KeyListener(this));
	load(variant);
	keyboard->setEventCallback(keyListener_.get());
}

KeyboardEventHandler::~KeyboardEventHandler()
{
}

void KeyboardEventHandler::load(const Variant & variant)
{
	std::cout << "\nLoading keyboard profile ...";
	for(Variant::ConstIterator itV = variant.begin();
		itV != variant.end();
		++itV)
	{
		Variant vCommand = *itV;
		std::string key = vCommand.key("key");
		std::string action = vCommand.key("action");
		Command command = vCommand.key("command").get<Command>();
		KeyMap::const_iterator itK = keyMap_.find(key);
		if(itK != keyMap_.end())
		{
			if(command < COMMAND_NULL || command >= COMMAND_MAX)
				THROW_EXCEPTION(Exception, OSS("Unknown command " << command));
			KeyCommands & keyCommands = keyboardCommands_[itK->second];
			if(action == "press")
				keyCommands.command[KEY_EVENT_PRESSED] = command;
			else if(action == "release")
				keyCommands.command[KEY_EVENT_RELEASED] = command;
			else
				THROW_EXCEPTION(Exception, OSS("Unknown keyboard action \"" << action << "\""));
		}
		else
			THROW_EXCEPTION(Exception, OSS("Unknown key \"" << key << "\""));
	}
}

bool KeyboardEventHandler::onKeyEvent(OIS::KeyCode keyCode, KeyEvent keyEvent)
{
	const Command command = keyboardCommands_[keyCode].command[keyEvent];
	if(command != COMMAND_VOID)
		scene_->onCommand(command);
	return command != COMMAND_VOID;
}

class JoyStickEventHandler::JoyStickListener : public OIS::JoyStickListener
{
public:
	JoyStickListener(JoyStickEventHandler * handler) : handler_(handler) {}
	~JoyStickListener() {}
	bool buttonPressed( const OIS::JoyStickEvent &arg, int button )
	{
		return handler_->onButtonEvent(button, JoyStickEventHandler::EVENT_BUTTON_PRESSED);
	}
	bool buttonReleased( const OIS::JoyStickEvent &arg, int button )
	{
		return handler_->onButtonEvent(button, JoyStickEventHandler::EVENT_BUTTON_PRESSED);
	}
	bool axisMoved( const OIS::JoyStickEvent &arg, int axis )
	{
		return handler_->onAxisEvent(axis, arg.state.mAxes[axis].abs);
	}
	bool povMoved( const OIS::JoyStickEvent &arg, int pov )
	{
		return true;
	}
	bool vector3Moved( const OIS::JoyStickEvent &arg, int index)
	{
		return true;
	}
private:
	JoyStickEventHandler * handler_;
};

JoyStickEventHandler::JoyStickEventHandler(Scene * scene, OIS::JoyStick * joyStick, const Variant & variant) : scene_(scene), vendor_(joyStick->vendor())
{
	ButtonCommands buttonCommands;
	buttonCommands.command[EVENT_BUTTON_PRESSED] = buttonCommands.command[EVENT_BUTTON_RELEASED] = COMMAND_VOID;
	buttonsCommands_.resize(joyStick->getNumberOfComponents(OIS::OIS_Button), buttonCommands);
	axisCommands_.resize(joyStick->getNumberOfComponents(OIS::OIS_Axis), COMMAND_VOID);
	joyStickListener_.reset(new JoyStickListener(this));
	load(variant);
	joyStick->setEventCallback(joyStickListener_.get());
}

JoyStickEventHandler::~JoyStickEventHandler()
{
}

void JoyStickEventHandler::load(const Variant & variant)
{
	try
	{
		if(const Variant vendor = variant.tryKey(vendor_.c_str()))
		{
			std::cout << "\nLoading \"" << vendor_.c_str() << "\" joystick profile ...";
			if(const Variant vButtons = vendor.tryKey("buttons"))
			{
				for(Variant::ConstIterator itB = vButtons.begin();
					itB != vButtons.end();
					++itB)
				{
					const Variant vButton = *itB;
					unsigned int button = vButton.key("button");
					std::string action = vButton.key("action");
					Command command = vButton.key("command").get<Command>();
					if(button >= buttonsCommands_.size())
						THROW_EXCEPTION(Exception, OSS("Unknown button " << button));

					if(command < COMMAND_NULL || command >= COMMAND_MAX)
						THROW_EXCEPTION(Exception, OSS("Unknown command " << command));

					ButtonCommands & keyCommands = buttonsCommands_[button];
					if(action == "press")
						buttonsCommands_[button].command[EVENT_BUTTON_PRESSED] = command;
					else if(action == "release")
						buttonsCommands_[button].command[EVENT_BUTTON_RELEASED] = command;
					else
						THROW_EXCEPTION(Exception, OSS("Unknown action \"" << action << "\""));
				}
			}
			if(const Variant vAxes = vendor.tryKey("axes"))
			{
				for(Variant::ConstIterator itA = vAxes.begin();
					itA != vAxes.end();
					++itA)
				{
					const Variant vAxis = *itA;
					unsigned int axis = vAxis.key("axis");
					Command command = vAxis.key("command").get<Command>();
					if(axis >= axisCommands_.size())
						THROW_EXCEPTION(Exception, OSS("Unknown axis " << axis));

					if(command < COMMAND_NULL || command >= COMMAND_MAX)
						THROW_EXCEPTION(Exception, OSS("Unknown command " << command));

					axisCommands_[axis] = command;
				}
			}
		}
		else
		{
			THROW_EXCEPTION(Exception, OSS("Joystick \"" << vendor_ << " is not supported"));
		}
	}
	catch (Exception & e)
	{
		std::cerr << e.what();
	}
}

bool JoyStickEventHandler::onButtonEvent(int button, ButtonEvent event)
{
	const Command command = buttonsCommands_[button].command[event];
	if(command != COMMAND_VOID)
		scene_->onCommand(command);
	return command != COMMAND_VOID;
}

bool JoyStickEventHandler::onAxisEvent(int axis, int value)
{
	const Command command = axisCommands_[axis];
	if(command != COMMAND_VOID)
		scene_->onAxisCommand(AxisCommand(command, float(value) / MAXSHORT));
	return command != COMMAND_VOID;
}