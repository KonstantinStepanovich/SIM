namespace Sim
{

typedef unsigned long long DynConstType;

template<typename T>
struct DynConstTypeData
{
	inline static size_t getBitSize() { return sizeof(T); }
	inline static T setBit(T & val, unsigned int bit) { return val = (T)1 << bit; }
};

class DynConst
{
public:
	DynConst(const char * val) : val_(val), number_(0) {;}
	DynConstType operator unsigned int() const;
private:
	const char * val_;
	mutable DynConstType number_;
};

class DynConstFlag
{
public:
	DynConstFlag(const DynConst & dynConst) : dynConst_(dynConst), flag_(0) {;}
	DynConstType operator DynConstType() const;
private:
	DynConst dynConst_;
	mutable DynConstType flag_;
};
