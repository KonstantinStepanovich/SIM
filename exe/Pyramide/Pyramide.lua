scene = {
	Physics = {
		type = 'Physics',
		gravity = { 0.0, -9.8, 0.0 },
--		geometryPath = 'Common/Geometry'
	},
	MovingBodyStorage = {
		type = 'MovingBodyStorage',
		worldSize = {
			min = {-70, -70, -70},
			max = {70, 70, 70}
		}
	},
	bodyEmmiter = {
		type = 'BodyEmmiter',
		pos = {0.0, 30.0, 0.0},
		euler = {0, 0, -math.pi / 2},
		size = {20.0, 20.0, 20.0},
		minVel = 1.0,
		maxVel = 4.0,
		maxAngle = math.pi / 8,
		minBodySize = 1.0,
		maxBodySize = 4.0,
		bodyDensity = 100.0,
		bodyRate = 0.5,
		planeColor = {1, 1, 1, 1},
		bodyColorMin = {
			0.0, 0.0, 0.0, 1.0
		},
		bodyColorMax = {
			1.0, 1.0, 1.0, 1.0
		}	
	},
	pyramide = {
		type = 'StaticBody',
		pos = {0.0, -20.0, 0.0},
		typeId = "Pyramide"
	},
	Camera = {
		type = "Camera",
		camType = "TYPE_TRACK_WORLD_POINT",
		distance = 150.0,
		center = {
			0.0,
			0.0,
			0.0
		},
		euler = {
			0.0,
			0.0,
			-math.rad(20.0),
		},
	}
}