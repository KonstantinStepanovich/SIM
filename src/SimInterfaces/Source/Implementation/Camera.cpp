#include "stdafx.h"

#include "Implementation/Camera.hpp"

using namespace Sim;
using namespace Sim::Interfaces;

Camera::Camera()
{
	data.type = TYPE_VOID;
}

void Camera::getCamera(Data & dataOut) const
{
	dataOut = data;
}