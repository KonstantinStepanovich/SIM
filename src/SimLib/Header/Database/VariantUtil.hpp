#ifndef _VariantUtil_hpp_
#define _VariantUtil_hpp_

#include "Database/Variant.hpp"

namespace Sim
{

//Load

template<typename T>
inline void load(const Variant & variant, T & t)
{
	t = variant.get<T>();
}

template<typename T>
inline void load(const Variant & variant, T * t, size_t size)
{
	for(size_t i = 0; i < size; i++)
		load(variant.index(i), t[i]);
}

template<typename T, size_t S>
inline void load(const Variant & variant, T * t)
{
	load(variant, t, S);
}

template<typename T, size_t S>
inline void load(const Variant & variant, T(&t)[S])
{
	load(variant, t, S);
}

template<typename T, size_t S1, size_t S2>
inline void load(const Variant & variant, T(&t)[S1][S2])
{
	for(size_t i = 0; i < S1; i++)
		load(variant.index(i + 1), t[i]);
}

//load from table

template<typename T, typename L>
inline void loadFromTable(const Variant & variant, unsigned int tableIndex, T & t, L loader)
{
	loader(variant.index(tableIndex), t);
}

template<typename T, typename L>
inline void loadFromTable(const Variant & variant, const char * tableName, T & t, L loader)
{
	loader(variant.key(tableName), t);
}

template<typename T, typename L>
inline bool tryLoadFromTable(const Variant & variant, unsigned int tableIndex, T & t, L loader)
{
	Variant element;
	if((element = variant.tryIndex(tableIndex)) != Variant::Null)
	{
		loader(element, t);
		return true;
	}
	else
		return false;
}

template<typename T, typename L>
inline bool tryLoadFromTable(const Variant & variant, const char * tableName, T & t, L loader)
{
	if(Variant element = variant.tryKey(tableName))
	{
		loader(element, t);
		return true;
	}
	else
		return false;
}

template<typename T>
struct Loader
{
	inline void operator()(const Variant & variant, T & t)
	{
		load(variant, t);
	}
};

template<typename T>
inline void loadFromTable(const Variant & variant, unsigned int tableIndex, T & t)
{
	loadFromTable(variant, tableIndex, t, Loader<T>());
}

template<typename T>
inline void loadFromTable(const Variant & variant, const char * tableName, T & t)
{
	loadFromTable(variant, tableName, t, Loader<T>());
}

template<typename T>
inline bool tryLoadFromTable(const Variant & variant, unsigned int tableIndex, T & t)
{
	return tryLoadFromTable(variant, tableIndex, t, Loader<T>());
}

template<typename T>
inline bool tryLoadFromTable(const Variant & variant, const char * tableName, T & t)
{
	return tryLoadFromTable(variant, tableName, t, Loader<T>());
}

//Save

template<typename T>
inline void save(Variant & variant, const T & t)
{
	variant = t;
}

template<typename T>
inline void save(Variant & variant, const T * t, size_t size)
{
	for(size_t i = 0; i < size; i++)
		save(variant[i], t[i]);
}

template<typename T, size_t S>
inline void save(Variant & variant, const T * t)
{
	save(variant, t, S);
}

template<typename T, size_t S>
inline void save(Variant & variant, const T(&t)[S])
{
	save(variant, t, S);
}

template<typename T, size_t S1, size_t S2>
inline void save(Variant & variant, const T(&t)[S1][S2])
{
	for(size_t i = 0; i < S1; i++)
		save(variant[i], t[i]);
}

//save to table

template<typename T, typename S>
inline void saveToTable(Variant & variant, unsigned int tableIndex, const T & t, S saver)
{
	saver(variant[tableIndex], t);
}

template<typename T, typename S>
inline void saveToTable(Variant & variant, const char * tableName, const T & t, S saver)
{
	saver(variant[tableName], t);
}

template<typename T>
struct Saver
{
	inline void operator()(Variant & variant, const T & t)
	{
		save(variant, t);
	}
};

template<typename T>
inline void saveToTable(Variant & variant, unsigned int tableIndex, const T & t)
{
	saveToTable(variant, tableIndex, t, Saver<T>());
}

template<typename T>
inline void saveToTable(Variant & variant, const char * tableName, const T & t)
{
	saveToTable(variant, tableName, t, Saver<T>());
}


}

#endif _VariantUtil_hpp_