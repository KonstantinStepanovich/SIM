local debug = false

bodyEmmiter = 
debug and 
{
	type = 'BodyEmmiter',
	pos = {-40.0, 0.0, 0.0},
	euler = {0, 0, math.pi / 5},
	size = {0.1, 0.1, 0.1},
	minVel = 25.0,
	maxVel = 25.0,
	maxAngle = 0.0,
	minBodySize = 3.0,
	maxBodySize = 3.0,
	bodyDensity = 100.0,
	bodyRate = 0.1,
	planeColor = {1, 0, 0, 1},
	bodyColorMin = {
		1.0, 0.0, 0.0, 1.0
	},
	bodyColorMax = {
		1.0, 0.0, 0.0, 1.0
	}
}
or
{
	type = 'BodyEmmiter',
	pos = {-40.0, 0.0, 0.0},
	euler = {0, 0, math.pi / 4},
	size = {0.0, 20.0, 20.0},
	minVel = 20.0,
	maxVel = 25.0,
	maxAngle = math.pi / 12,
	minBodySize = 1.0,
	maxBodySize = 3.0,
	bodyDensity = 100.0,
	bodyRate = 0.5,
	planeColor = {1, 0, 0, 1},
	bodyColorMin = {
		1.0, 0.0, 0.0, 1.0
	},
	bodyColorMax = {
		1.0, 0.0, 0.0, 1.0
	}
}

--[[
bodyEmmiter.bodyTypes = {
	{
		typeId = 'Box',
		I = {
			{0.0, 1.0 / 12.0, 1.0 / 12.0},
			{1.0 / 12.0, 0.0, 1.0 / 12.0},
			{1.0 / 12.0, 1.0 / 12.0, 0.0},
		},
		sizeArgs = {
			{
				index = 0,
				factor = 1.0
			},
			{
				index = 1,
				factor = 1.0
			},
			{
				index = 2,
				factor = 1.0
			}
		}
	},
	{
		typeId = 'Cylinder',
		I = {
			{1.0 / 4.0, 1.0 / 12.0},
			{1.0 / 2.0, 0.0},
			{1.0 / 4.0, 1.0 / 12.0}
		},		
		sizeArgs = {
			{
				index = 0,
				factor = 0.333
			},
			{
				index = 1,
				factor = 1.0
			}
		}
	},
	{
		typeId = 'Sphere',
		I = {
			{2.0 / 5.0},
			{2.0 / 5.0},
			{2.0 / 5.0},
		},		
		sizeArgs = {
			{
				index = 0,
				factor = 1.0
			}
		}
	}
}
--]]
	
	
scene = {
	Physics = {
		type = 'Physics',
		gravity = { 0.0, -9.8, 0.0 },
	},
	MovingBodyStorage = {
		type = 'MovingBodyStorage',
		worldSize = {
			min = {-70, -70, -70},
			max = {70, 70, 70}
		}
	},
	bodyEmmiter1 = bodyEmmiter,
	bodyInterceptor = {
		type = 'BodyInterceptor',
		pos = {30.0, 0.0, 0.0},
		projectileVelAbs = 30.0,
		projectileSize = 2.0,
		projectileDensity = 300.0,
		detectionRadius = 60.0,
		defendingRadius = 25.0,
		engagementRadius = 60.0,
		defendingAreaColor = {0, 0, 1, 1},
		fireRate = 5,
		projectileColor = {
			0.0, 0.0, 1.0, 1.0
		}
	},
	Camera = {
		type = "Camera",
		camType = "TYPE_TRACK_WORLD_POINT",
		distance = 180.0,
		center = {
			0.0,
			0.0,
			0.0
		},
		euler = {
			0.0,
			math.pi / 2,
			-math.pi / 20
		},
	}
}