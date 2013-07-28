#ifndef _StringStream_hpp_
#define _StringStream_hpp_

#include <sstream>

#define OSSTR(expression)	((std::ostringstream&)((std::ostringstream&)std::ostringstream() << expression)).str()
#define OSS(expression)		((std::ostringstream&)((std::ostringstream&)std::ostringstream() << expression)).str().c_str()

#endif _StringStream_hpp_