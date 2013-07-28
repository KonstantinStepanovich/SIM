#ifndef _DataModelAPI_h_
#define _DataModelAPI_h_

//The API represents an abstract hierarchic data structure.
//Each node of such tree can be one of 4 types: integer, real, array and table
//Value of integer and real nodes can be changed
//Array can be modified
//Table structure is immutable
//Written in orthodox C

#define dmIndex unsigned int
#define dmInteger int
#define dmReal double

#define dmValueHandle void*

struct dmItemTable
{
	const char * field;
	dmValueHandle handle;
};

struct dmItemTableC
{
	const char * field;
	const dmValueHandle handle;
};

#define LIMIT_INT_TYPE_MIN_MAX 0
#define LIMIT_INT_TYPE_VARIANTS 1
#define LIMIT_INT_TYPE_BOOL 2

struct dmLimitsInt
{
	unsigned int type;
	struct
	{
		struct {
			dmInteger min;
			dmInteger max;
		} minMax;
		struct
		{
			dmIndex size;
			const char ** name;
		} variants;
	} data;
};

struct dmLimitsReal
{
	dmReal min;
	dmReal max;
};

struct dmValue
{
	unsigned int		(*getType)		(const dmValueHandle handle);
	dmInteger 			(*getInteger)	(const dmValueHandle handle);
	const dmLimitsInt *	(*getLimitsInt)	(const dmValueHandle handle);
	void				(*setInteger)	(dmValueHandle handle, dmInteger val);
	dmReal				(*getReal)		(const dmValueHandle handle);
	const dmLimitsReal *(*getLimitsReal)(const dmValueHandle handle);
	void				(*setReal)		(dmValueHandle handle, dmReal val);	
	dmIndex				(*getSize)		(const dmValueHandle handle);
	dmValueHandle		(*index)		(dmValueHandle handle, dmIndex index);
	dmValueHandle		(*indexArray)	(dmValueHandle handle, dmIndex index);
	const dmValueHandle	(*indexArrayC)	(const dmValueHandle handle, dmIndex index);
	dmValueHandle		(*addItem)		(dmValueHandle handle, dmIndex index);
	void				(*removeItem)	(dmValueHandle handle, dmIndex index);
	dmItemTable			(*indexTable)	(dmValueHandle handle, dmIndex index);
	dmItemTableC		(*indexTableC)	(const dmValueHandle handle, dmIndex index);
	void				(*notifyChange)	(dmValueHandle handle);
};

struct DataModelAPI
{
	dmValue value;
};

#endif _DataModelAPI_h_