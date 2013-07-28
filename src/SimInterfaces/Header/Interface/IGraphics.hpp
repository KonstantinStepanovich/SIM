#ifndef _IGraphics_hpp_
#define _IGraphics_hpp_

#include "Math.hpp"

//Interface of simulator graphic engine
//This interface is a between graphic engine and all other systems

namespace Sim
{

namespace Interfaces
{

namespace Graphics
{

class IPositionSource
{
public:
	virtual ~IPositionSource() {;}
	virtual void getPosition(Time time, Position & position, Rotation & rotation) const = 0;
};

typedef unsigned int TypeID;
typedef TypeID TerrainID;
typedef unsigned int ObjectID;
class IObject;

class IScene
{
public:
	virtual ~IScene() {;}
	virtual void setTerrain(TerrainID) = 0;
	virtual IObject* addObject(TypeID) = 0;
	virtual void removeObject(IObject*) = 0;
};

typedef unsigned int AnimArgID;
typedef Real AnimArgVal;
typedef unsigned int SlotID;

class IObject
{
public:
	virtual ~IObject() {;}
	virtual void setPosition(const Position &, const Rotation &) = 0;
	virtual void setPositionSource(const IPositionSource * ) = 0;	
	virtual void setAnimArg(AnimArgID, AnimArgVal) = 0;
	virtual IObject* addChild(SlotID, TypeID) = 0;
	virtual void removeChild(SlotID) = 0;
	virtual IObject* getChild(SlotID) const = 0;
};

class ICamera
{
public:
	virtual ~ICamera() {;}
	virtual void setPosition(const Position &, const Rotation &) = 0;
	virtual void setPositionSource(const IPositionSource * ) = 0;
};

class IView
{
public:
	virtual ~IView() {;}
	virtual void setFullScreen(int screenNum = 0) = 0;
	virtual void setWindow(int x, int y, int width, int height, int screenNum = 0) = 0;
	virtual ICamera * getCamera() = 0;
	virtual IScene * getScene() = 0;
	virtual void render(Time) = 0;
};

extern IView* (*getView)();

}

}

}

#endif _IGraphics_hpp_