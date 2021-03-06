#ifndef _Events_hpp_
#define _Events_hpp_

namespace Sim
{

namespace Interfaces
{

enum Events
{
	EVENT_POSITION_CHANGED = 1 << 2,
	EVENT_ROTATION_CHANGED = 1 << 3,
	EVENT_BODY_CHANGED = EVENT_POSITION_CHANGED | EVENT_ROTATION_CHANGED,
	EVENT_DYNAMIC_BODY_CHANGED = 1 << 4,
	EVENT_SHAPE_CHANGED = 1 << 5,
	EVENT_MASS_CHANGED = 1 << 6,
	EVENT_TYPE_ID_CHANGED = 1 << 7,
	EVENT_ANIMATION_CHANGED = 1 << 8,
	EVENT_OBB_CHANGED = 1 << 9,
	EVENT_WINDOW_CHANGED = 1 << 10,
	EVENT_AIRPLANE_CHANGED = 1 << 11,
	EVENT_CAMERA_CHANGED = 1 << 12,
	EVENT_DEBUG_OBJECT_CHANGED = 1 << 13
};

}

}

#endif _Events_hpp_