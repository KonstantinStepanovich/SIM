local blackColor = {0, 0, 0, 0}

function makeAirplane(length, width, height, wingSpan, color, harpointCount, mass, inertiaTensor)
	local wingPos = -0.05 * length
	local wingWidth = wingSpan / 8.0
	local wingHeight = 0.2
	local aileronPos = 0.8 * 0.5 * wingSpan
	local aileronLength = 0.4 * 0.5 * wingSpan
	local aileronWidth = 0.2 * wingWidth
	
	local tailPlaneSpan = wingSpan / 2.5
	local tailPlaneWidth = wingWidth / 1.5
	local tailPlaneHeight = 0.1
	local tailPlanePos = -0.5 * (length - tailPlaneWidth)
	local elevatorWidth = 0.5 * tailPlaneWidth

	local finHeight = length / 8.0
	local finLength = finHeight / 1.5
	local finThickness = 0.1
	local finPos = -0.5 * (length - finLength)
	local rudderLength = 0.5 * finLength
	
	local flapWidth = wingWidth / 4
	local flapLength = 0.6 * wingSpan / 2
	local flapPos = 0.3 * 0.5 * wingSpan
	
	local noseGearPos = 0.4 * length
	local mainGearPos = wingPos
	local mainGearGap = 0.2 * wingSpan
	local gearBase = mainGearPos - noseGearPos
	
	local gearSturtRadius = 0.07
	local gearSturtHeight = 1
	
	local wheelRadius = 0.35
	
	local tbl = {
		geometry = {
			type = 'Group',
			children = {
				{
					type = 'Box',
					name = 'Fuselage',
					pos = {
						0,
						0,
						0
					},
					quat = {
						0,
						0,
						0,
						1
					},
					size = { length, height, width },
					color = color
				},
				{
					type = 'Box',
					name = 'Wing',
					pos = {
						wingPos,
						0,
						0
					},
					quat = {
						0,
						0,
						0,
						1
					},
					type = 'Box',
					size = { wingWidth, wingHeight, wingSpan},
					color = color
				},
				{
					type = "Transform",
					name = "Left Aileron Servo",
					children =
					{
						{
							type = 'Box',
							name = 'Left aileron',
							pos = {
								-0.5 * aileronWidth,
								0,
								0
							},
							size = { aileronWidth, wingHeight, aileronLength },
							color = color
						}
					}
				},
				{
					type = "Transform",
					name = "Right Aileron Servo",
					children =
					{
						{
							type = 'Box',
							name = 'Right aileron',
							pos = {
								-0.5 * aileronWidth,
								0,
								0
							},
							size = { aileronWidth, wingHeight, aileronLength  },
							color = color
						}
					}
				},		
				{
					type = 'Box',
					name = 'Tail plane',
					pos = {
						tailPlanePos,
						0,
						0
					},
					quat = {
						0,
						0,
						0,
						1
					},
					size = { tailPlaneWidth, tailPlaneHeight, tailPlaneSpan},
					color = color
				},
				{
					type = "Transform",
					name = "Elevator Servo",
					children = {
						{
							type = 'Box',
							name = 'Elevator',
							pos = {
								-0.5 * elevatorWidth,
								0,
								0
							},
							quat = {
								0,
								0,
								0,
								1
							},
							size = { elevatorWidth, tailPlaneHeight, tailPlaneSpan},
							color = color
						}
					}
				},
				{
					type = 'Box',
					name = 'Fin',
					pos = {
						finPos,
						0.5 * (height + finHeight),
						0
					},
					quat = {
						0,
						0,
						0,
						1
					},
					size = { finLength, finHeight, finThickness },
					color = color
				},
				{
					type = 'Transform',
					name = 'Rudder Servo',
					children = {
						{
							type = 'Box',
							name = 'Rudder',
							pos = {
								-0.5 * rudderLength,
								0,
								0
							},
							quat = {
								0,
								0,
								0,
								1
							},
							size = { rudderLength, finHeight, finThickness },
							color = color
						},	
					}
				},
				{
					type = 'Transform',
					name = 'Flaps Servo',
					children = {
						{
							type = 'Box',
							name = 'Left Flap',
							pos = {
								-0.5 * flapWidth,
								0,
								-flapPos
							},
							size = {
								flapWidth, wingHeight, flapLength
							},
							color = color
						},
						{
							type = 'Box',
							name = 'Right Flap',
							pos = {
								-0.5 * flapWidth,
								0,
								flapPos
							},
							size = {
								flapWidth, wingHeight, flapLength
							},
							color = color
						}
					}
				},
				{
					type = 'Transform',
					name = 'Nose Gear Servo',
					children = {
						{
							type = 'Cylinder',
							name = 'Nose gear',
							pos = {
								0.0,
								-0.5 * (height + gearSturtHeight),
								0.0,
							},
							rot = {
								axis = {
									1,
									0,
									0
								},
								angle = math.rad(90)
							},
							radius = gearSturtRadius,
							height = gearSturtHeight,
							color = color
						},
						{
							name = 'Nose wheel',
							type = 'Sphere',
							pos = {
								0.0,
								-0.5 * height - gearSturtHeight - wheelRadius,
								0.0,
							},
							radius = wheelRadius,
							color = blackColor
						}
					}
				},
				{
					type = 'Transform',
					name = 'Left Gear Servo',
					children = {
						{
							type = 'Cylinder',
							name = 'Left Gear',
							pos = {
								0.0,
								-0.5 * (wingHeight + gearSturtHeight + 0.5 * (height - wingHeight)),
								0.0,
							},
							rot = {
								axis = {
									1,
									0,
									0
								},
								angle = 1.57
							},
							radius = gearSturtRadius,
							height = gearSturtHeight + 0.5 * (height - wingHeight),
							color = color
						},
						{
							type = 'Sphere',
							name = 'Left wheel',
							pos = {
								0.0,
								-0.5 * height - gearSturtHeight - wheelRadius,
								0.0,
							},
							radius = wheelRadius,
							color = blackColor
						},
					}
				},
				{
					type = 'Transform',
					name = 'Right Gear Servo',
					children = {
						{
							type = 'Cylinder',
							name = 'Right Gear',
							pos = {
								0.0,
								-0.5 * (wingHeight + gearSturtHeight + 0.5 * (height - wingHeight)),
								0.0,
							},
							rot = {
								axis = {
									1,
									0,
									0
								},
								angle = 1.57
							},
							radius = gearSturtRadius,
							height = gearSturtHeight + 0.5 * (height - wingHeight),
							color = color
						},
						{
							type = 'Sphere',
							name = 'Right wheel',
							pos = {
								0.0,
								-0.5 * height - gearSturtHeight - wheelRadius,
								0.0,
							},
							radius = wheelRadius,
							color = blackColor
						},
					}
				}
			}
		},
		animation = {
			{
				{
					type = 'TransformPath',
					transformName = 'Left Aileron Servo',
					path = {
						{
							phase = -1.1,
							transform = {
								pos = {
									wingPos - 0.5 * wingWidth,
									0,
									-aileronPos
								},
								rot = { axis = {0, 0, 1}, angle = math.rad(-20.0) }
							}
						},
						{
							phase = 1.1,
							transform = {
								pos = {
									wingPos - 0.5 * wingWidth,
									0,
									-aileronPos
								},							
								rot = { axis = {0, 0, 1}, angle = math.rad(20.0) }
							}
						}
					}
				},
				{
					type = 'TransformPath',
					transformName = 'Right Aileron Servo',
					path = {
						{
							phase = -1.1,
							transform = {
								pos = {
									wingPos - 0.5 * wingWidth,
									0,
									aileronPos
								},							
								rot = { axis = {0, 0, 1}, angle = math.rad(20.0) }
							}
						},
						{
							phase = 1.1,
							transform = {
								pos = {
									wingPos - 0.5 * wingWidth,
									0,
									aileronPos
								},							
								rot = { axis = {0, 0, 1}, angle = math.rad(-20.0) }
							}
						}
					}
				}		
			},
			{
				{
					type = 'TransformPath',
					transformName = 'Elevator Servo',
					path = {
						{
							phase = -1.1,
							transform = {
								pos = { tailPlanePos - 0.5 * tailPlaneWidth, 0, 0 },
								rot = { axis = {0, 0, 1}, angle = math.rad(20.0) }
							}
						},
						{
							phase = 1.1,
							transform = {
								pos = { tailPlanePos - 0.5 * tailPlaneWidth, 0, 0 },
								rot = { axis = {0, 0, 1}, angle = math.rad(-20.0) }
							}
						}
					}
				},
			},
			{
				{
					type = 'TransformPath',
					transformName = 'Rudder Servo',
					path = {
						{
							phase = -1.1,
							transform = {
								pos = { finPos - 0.5 * finLength, 0.5 * (height + finHeight), 0 },
								rot = { axis = {0, 1, 0}, angle = math.rad(-20.0) }
							}
						},
						{
							phase = 1.1,
							transform = {
								pos = { finPos - 0.5 * finLength, 0.5 * (height + finHeight), 0 },
								rot = { axis = {0, 1, 0}, angle = math.rad(20.0) }
							}
						}
					}
				}
			},
			{
				{
					type = 'TransformPath',
					transformName = 'Flaps Servo',
					path = {
						{
							phase = -0.1,
							transform = {
								pos = {
									wingPos - 0.5 * wingWidth + (flapWidth - aileronWidth),
									0,
									0
								},	
								rot = { axis = {0, 0, 1}, angle = math.rad(0.0) }
							}
						},
						{
							phase = 1.1,
							transform = {
								pos = {
									wingPos - 0.5 * wingWidth,
									0,
									0
								},
								rot = { axis = {0, 0, 1}, angle = math.rad(30.0) }
							}
						}
					}
				}
			},
			{
				{
					type = 'TransformPath',
					transformName = 'Nose Gear Servo',
					path = {
						{
							phase = 0.0,
							transform = {
								pos = { noseGearPos, 0, 0 },
								rot = { axis = {0, 0, 1}, angle = math.rad(0.0) }
							}
						},
						{
							phase = 1.0,
							transform = {
								pos = { noseGearPos, 0, 0 },
								rot = { axis = {0, 0, 1}, angle = -math.rad(90.0) }
							}
						}
					}
				},
				{
					type = 'TransformPath',
					transformName = 'Left Gear Servo',
					path = {
						{
							phase = 0.0,
							transform = {
								pos = {
									mainGearPos,
									0.0,
									-0.5 * mainGearGap,
								},
								rot = { axis = {1, 0, 0}, angle = math.rad(0.0) }
							}
						},
						{
							phase = 1.0,
							transform = {
								pos = {
									mainGearPos,
									0.0,
									-0.5 * mainGearGap,
								},							
								rot = { axis = {1, 0, 0}, angle = -math.rad(90.0) }
							}
						}
					}
				},
				{
					type = 'TransformPath',
					transformName = 'Right Gear Servo',
					path = {
						{
							phase = 0.0,
							transform = {
								pos = {
									mainGearPos,
									0.0,
									0.5 * mainGearGap,
								},							
								rot = { axis = {1, 0, 0}, angle = math.rad(0.0) }
							}
						},
						{
							phase = 1.0,
							transform = {
								pos = {
									mainGearPos,
									0.0,
									0.5 * mainGearGap,
								},
								rot = { axis = {1, 0, 0}, angle = math.rad(90.0) }
							}
						}
					}
				}
			}
		}
	}
		
	if harpointCount ~= nil then	
		local hardPointSize = {0.6 * wingWidth, 0.3, 0.1}
		local distFromFirstHardpointToCenter = 1.0
		local distFromLastHardpointToWingTip = 2.0		
		local hardPointInterval = (wingSpan - 2 * (distFromFirstHardpointToCenter + distFromLastHardpointToWingTip)) / (2 * harpointCount - 1)
				
		for hardPointIndex = -harpointCount, harpointCount  do
			
			local side = hardPointIndex > 0 and 1 or -1
		
			local pos = {
				wingPos,
				-0.5 * wingHeight - 0.5 * hardPointSize[2],
				distFromFirstHardpointToCenter * side + hardPointIndex * hardPointInterval 				
			}
			
			local hardPoint = {
				type = 'Box',
				name = 'Hardpoint '..tostring(hardPointIndex),
				pos = pos,
				quat = {
					0,
					0,
					0,
					1
				},
				size = { hardPointSize[1], hardPointSize[2], hardPointSize[3] },
				color = color
			}
			table.insert(tbl.geometry.children, hardPoint)
		end
	end
	
	local function moveGeometry(list)
		for geometryIndex, geometry in pairs(list) do
			if geometry.pos ~= nil then
				geometry.pos[1] = geometry.pos[1] + wingPos + 0.1 * gearBase
			end
			if geometry.children ~= nil then
				moveGeometry(geometry.children)
			end
		end
	end
	
	--moveGeometry(tbl.geometry.children)
	
	return tbl
end