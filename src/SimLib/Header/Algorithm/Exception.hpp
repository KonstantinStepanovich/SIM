#ifndef _Exception_hpp_
#define _Exception_hpp_

#include <exception>
#include "String/StringStream.hpp"

#include <iostream>

namespace Sim
{

class Exception : public std::exception
{
public:
	Exception(const char * const message, const char * place) : std::exception(message), place_(place) {;}
	const char * where() const { return place_.c_str(); }
private:
	std::string place_;
};

#define DECLARE_EXCEPTION(SubException, BaseException) class SubException : public Sim::Exception\
{\
public:\
	SubException(const char * const message, const char * place) : Exception(message, place) {;}\
};

#define THROW_EXCEPTION(ExceptionType, message) throw ExceptionType(message, OSS(__FILE__ << ": " << __LINE__))

inline void printException(const Exception & exception)
{
	std::cerr << "Exception: " << exception.what() << "\nthrown from " << exception.where() << "\n";
}

inline void printException(const std::exception & exception)
{
	std::cerr << "Exception: " << exception.what() << "\n";
}

}

#endif _Exception_hpp_