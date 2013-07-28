scene = {
	ground = 	{
		type = "StaticBody",
		typeId = "FlatTerrain",
		pos = {0, 0, 0}
	},
	camera = {
		type = "Camera",
		name = "Camera",
		camType = "TYPE_TRACK_OBJECT_LOCAL",
		objectToTrack = "airplane",
		distance = 100.0,
		center = {
			0.0,
			0.0,
			0.0
		},
		euler = {
			0.0,
			0.0,
			0.0,
		},
	},
	
	airplane = 	{
		type = "Airplane",
		typeId = "Tu-154M",
		pos = {
			-1200.0,
			5.43,
			0
		},
		vel = {
			0, --700 / 3.6,
			0,
			0
		},
		rot = {
			axis = {
				0,
				0,
				1
			},
			angle = 0.0
		},
		angularVel = {
			0,
			0,
			0
		},
		fuel = 0.5,
		--[[
		payload = {
			typeId = "Su-25",
			count = 1,
			childs = {
				[3] = {
					typeId = 'B-8M1',
					child = {
						typeId = 'C-8',
						count = 20
					}
				},	
				[4] = {
					typeId = 'FAB-500',
					count = 1
				},
				[5] = {
					typeId = 'PTB-1150',
					child = {
						typeId = 'JetFuel',
						mass = 1150
					}
				},
				[6] = {
					typeId = 'PTB-1150',
					child = {
						typeId = 'JetFuel',
						mass = 1150
					}
				},	
				[7] = {
					typeId = 'FAB-500',
					count = 1
				},
				[8] = {
					typeId = 'B-8M1',
					child = {
						typeId = 'C-8',
						count = 20
					}
				}
			}
		},
		--]]
		dtime = 0.02,
		
		forceCoeffs = {1, 1, 1},
		torqueCoeffs = {1, 1, 1}
	}
}

local torqueOnly = false
if torqueOnly then
	scene.Physics = {
		type = 'Physics',
		gravity = {0, 0, 0 }
	}
	scene.StandardAtmosphere = 
	{
		type = "StandardAtmosphere",
		wind = { -200.0, 0.0, 0.0 }
	}
	--scene.camera.type = "TYPE_OBJECT_POSITION"
	--scene.camera.pos[1] = -100	
	scene.airplane.pos[2] = 30.0
	scene.airplane.forceCoeffs = {0, 0, 0}
	scene.airplane.torqueCoeffs = {1, 1, 1}
else
	scene.Physics = {
		type = 'Physics',
		gravity = {0, -9.8, 0 }
	}
end