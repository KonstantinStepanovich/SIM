COMMAND_PAUSE = 0
COMMAND_DECELLERATE_MODEL_TIME = 1
COMMAND_ACELLERATE_MODEL_TIME = 2
COMMAND_NORMALIZE_MODEL_TIME = 3
COMMAND_RELOAD = 4

COMMAND_AIRPLANE_AILERONS = 5
COMMAND_AIRPLANE_RUDDER = 6
COMMAND_AIRPLANE_ELEVATOR = 7
COMMAND_AIRPLANE_THROTTLE = 8

COMMAND_AIRPLANE_AILERONS_LEFT = 9
COMMAND_AIRPLANE_AILERONS_LEFT_RELEASE = 10
COMMAND_AIRPLANE_AILERONS_RIGHT = 11
COMMAND_AIRPLANE_AILERONS_RIGHT_RELEASE = 12
COMMAND_AIRPLANE_RUDDER_LEFT = 13
COMMAND_AIRPLANE_RUDDER_LEFT_RELEASE = 14
COMMAND_AIRPLANE_RUDDER_RIGHT = 15
COMMAND_AIRPLANE_RUDDER_RIGHT_RELEASE = 16
COMMAND_AIRPLANE_ELEVATOR_UP = 17
COMMAND_AIRPLANE_ELEVATOR_UP_RELEASE = 18
COMMAND_AIRPLANE_ELEVATOR_DOWN = 19
COMMAND_AIRPLANE_ELEVATOR_DOWN_RELEASE = 20
COMMAND_AIRPLANE_THROTTLE_INCREASE = 21
COMMAND_AIRPLANE_THROTTLE_INCREASE_RELEASE = 22
COMMAND_AIRPLANE_THROTTLE_DECREASE = 23
COMMAND_AIRPLANE_THROTTLE_DECREASE_RELEASE = 24

COMMAND_AIRPLANE_FLAPS = 25
COMMAND_AIRPLANE_GEARS = 26

COMMAND_CAMERA_WORLD_POINT = 50
COMMAND_CAMERA_OBJECT_POSITION = 51
COMMAND_CAMERA_OBJECT_TRANSFORM = 52

input = {
	keyboard = {
		{ key = 'P', 			action = 'press', command = COMMAND_PAUSE },
		{ key = '-', 			action = 'press', command = COMMAND_DECELLERATE_MODEL_TIME },
		{ key = '=', 			action = 'press', command = COMMAND_ACELLERATE_MODEL_TIME },
		{ key = 'Backspace', 	action = 'press', command = COMMAND_NORMALIZE_MODEL_TIME },
		{ key = 'R',			action = 'press', command = COMMAND_RELOAD },
		
		{ key = 'Left', 		action = 'press', 	command = COMMAND_AIRPLANE_AILERONS_LEFT },
		{ key = 'Left', 		action = 'release', command = COMMAND_AIRPLANE_AILERONS_LEFT_RELEASE },
		{ key = 'Right', 		action = 'press', 	command = COMMAND_AIRPLANE_AILERONS_RIGHT },
		{ key = 'Right', 		action = 'release', command = COMMAND_AIRPLANE_AILERONS_RIGHT_RELEASE },
		{ key = 'Z', 			action = 'press', 	command = COMMAND_AIRPLANE_RUDDER_LEFT },
		{ key = 'Z', 			action = 'release', command = COMMAND_AIRPLANE_RUDDER_LEFT_RELEASE },
		{ key = 'X', 			action = 'press', 	command = COMMAND_AIRPLANE_RUDDER_RIGHT },
		{ key = 'X', 			action = 'release', command = COMMAND_AIRPLANE_RUDDER_RIGHT_RELEASE },	
		{ key = 'Up', 			action = 'press', 	command = COMMAND_AIRPLANE_ELEVATOR_UP },
		{ key = 'Up', 			action = 'release', command = COMMAND_AIRPLANE_ELEVATOR_UP_RELEASE },
		{ key = 'Down', 		action = 'press', 	command = COMMAND_AIRPLANE_ELEVATOR_DOWN },
		{ key = 'Down', 		action = 'release', command = COMMAND_AIRPLANE_ELEVATOR_DOWN_RELEASE },
		{ key = ']', 			action = 'press', 	command = COMMAND_AIRPLANE_THROTTLE_INCREASE },
		{ key = ']', 			action = 'release', command = COMMAND_AIRPLANE_THROTTLE_INCREASE_RELEASE },
		{ key = '[', 			action = 'press', 	command = COMMAND_AIRPLANE_THROTTLE_DECREASE },
		{ key = '[', 			action = 'release', command = COMMAND_AIRPLANE_THROTTLE_DECREASE_RELEASE },		
		
		{ key = 'F', 			action = 'press', 	command = COMMAND_AIRPLANE_FLAPS },
		{ key = 'G', 			action = 'press', 	command = COMMAND_AIRPLANE_GEARS },
		
		{ key = 'F1',			action = 'press', 	command = COMMAND_CAMERA_WORLD_POINT },
		{ key = 'F2',			action = 'press', 	command = COMMAND_CAMERA_OBJECT_POSITION },
		{ key = 'F3',			action = 'press', 	command = COMMAND_CAMERA_OBJECT_TRANSFORM }
	},

	joyStick = {
		['Saitek X52 Flight Controller'] = {
			buttons = {
			},
			axes = {
				{ axis = 1, 	command = COMMAND_AIRPLANE_AILERONS },
				{ axis = 2, 	command = COMMAND_AIRPLANE_RUDDER },
				{ axis = 0, 	command = COMMAND_AIRPLANE_ELEVATOR },
				{ axis = 5, 	command = COMMAND_AIRPLANE_THROTTLE }
			}
		},
		['Saitek X52 Pro Flight Control System'] = {
			buttons = {
			},
			axes = {
				{ axis = 1, 	command = COMMAND_AIRPLANE_AILERONS },
				{ axis = 2, 	command = COMMAND_AIRPLANE_RUDDER },
				{ axis = 0, 	command = COMMAND_AIRPLANE_ELEVATOR },
				{ axis = 5, 	command = COMMAND_AIRPLANE_THROTTLE }
			}
		}
	}
}