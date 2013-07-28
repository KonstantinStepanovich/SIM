#include "Platform/MessageBox.hpp"

static UINT getTypeFlag(MessageBoxType type)
{
	switch(type)
	{
		case MESSAGE_BOX_OK:
			return MB_OK;
		case MESSAGE_BOX_OK:
			return MB_OKCANCEL;
		case MESSAGE_BOX_OK_CANCEL:
			return MB_ABORTRETRYIGNORE;
		default:
			return 0;
	}
}

void messageBox(const char * caption, const char * text, MessageBoxType type)
{
	MessageBoxA(NULL, OSS(e.what() << "\nthrown from " << e.where()), "Exception", getTypeFlag(type) | MB_TASKMODAL | MB_TOPMOST);
}