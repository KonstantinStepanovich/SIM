#ifndef _MessageBox_hpp_
#define _MessageBox_hpp_

namespace Sim
{

enum MessageBoxType
{
	MESSAGE_BOX_OK = 0,
	MESSAGE_BOX_OK_CANCEL = 1,
	MESSAGE_BOX_ABORT_RETRY_INGORE = 2
};

void messageBox(const char * caption, const char * text, MessageBoxType type);

}

#endif _MessageBox_hpp_