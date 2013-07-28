#ifndef _InputOIS_InputEventHandler_hpp_
#define _InputOIS_InputEventHandler_hpp_

#include <vector>
#include <map>

namespace OIS
{
enum KeyCode;
class Keyboard;
class JoyStick;
class KeyEvent;
}

namespace Sim
{

class Variant;

namespace InputOIS
{

enum Command;
class Scene;

class KeyboardEventHandler
{
public:
	enum KeyEvent {
		KEY_EVENT_PRESSED,
		KEY_EVENT_RELEASED,
		KEY_EVENT_MAX
	};
	KeyboardEventHandler(Scene * scene, OIS::Keyboard * keyboard, const Variant & variant);
	~KeyboardEventHandler();
	void load(const Variant & variant);
	bool onKeyEvent(OIS::KeyCode keyCode, KeyEvent keyEvent);
private:
	Scene * scene_;

	struct KeyCommands
	{
		Command command[KEY_EVENT_MAX];
	};
	typedef std::vector<KeyCommands> KeyboardCommands;
	KeyboardCommands keyboardCommands_;
	
	typedef std::map<std::string, OIS::KeyCode> KeyMap;
	KeyMap keyMap_;

	class KeyListener;
	std::auto_ptr<KeyListener> keyListener_;
};

class JoyStickEventHandler
{
public:
	enum ButtonEvent { 
		EVENT_BUTTON_PRESSED,
		EVENT_BUTTON_RELEASED,
		EVENT_BUTTON_MAX
	};
	JoyStickEventHandler(Scene * scene, OIS::JoyStick * joyStick, const Variant & variant);
	~JoyStickEventHandler();
	void load(const Variant & variant);
	bool onButtonEvent(int button, ButtonEvent event);
	bool onAxisEvent(int axis, int value);
	//bool onPOVEvent(int pov, );	
private:
	Scene * scene_;
	const std::string vendor_;

	struct ButtonCommands
	{
		Command command[EVENT_BUTTON_MAX];
	};
	typedef std::vector<ButtonCommands> ButtonsCommands;
	ButtonsCommands buttonsCommands_;

	typedef std::vector<Command> AxisCommands;
	AxisCommands axisCommands_;

	class JoyStickListener;
	std::auto_ptr<JoyStickListener> joyStickListener_;
};

}

}

#endif _InputOIS_InputEventHandler_hpp_