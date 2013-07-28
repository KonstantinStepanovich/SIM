{
	"parameters":
	[
		{
			"id": "Tree",
			"parameters":
			{
				"editable": true
			}
		},
		{
			"id": "Plot",
			"parameters":
			{
				"editable": true,
				"caption": "cx0(M)",
				"X":
				{
					"caption": "M",
					"type": null
				},
				"Y":
				{
					"caption": "cx0",
					"type": null
				},
				"singleCurves":
				[
					{
						"caption": "cx0(M)",
						"array": [0, 4, 0, 0],
						"arg": [0],
						"val": [1, 0],
						"Xmin": 0.0
					}
				]
			}
		},
		{
			"id": "Plot",
			"parameters":
			{
				"editable": true,
				"caption": "dCydAlpha(M)",
				"X":
				{
					"caption": "M",
					"type": null
				},
				"Y":
				{
					"caption": "dCydAlpha",
					"type": null
				},
				"singleCurves":
				[
					{
						"caption": "dCydAlpha(M)",
						"array": [0, 4, 0, 0],
						"arg": [0],
						"val": [1, 1],
						"Xmin": 0.0
					}
				]
			}
		},
		{
			"id": "Plot",
			"parameters":
			{
				"editable": true,
				"caption": "CyMax(M)",
				"X":
				{
					"caption": "M",
					"type": null
				},
				"Y":
				{
					"caption": "CyMax",
					"type": null
				},
				"singleCurves":
				[
					{
						"caption": "CyMax(M)",
						"array": [0, 4, 0, 0],
						"arg": [0],
						"val": [1, 2],
						"Xmin": 0.0
					}
				]
			}
		},
		{
			"id": "Plot",
			"parameters":
			{
				"editable": true,
				"caption": "BPol(M)",
				"X":
				{
					"caption": "M",
					"type": null
				},
				"Y":
				{
					"caption": "BPol",
					"type": null
				},
				"singleCurves":
				[
					{
						"caption": "BPol(M)",
						"array": [0, 4, 0, 0],
						"arg": [0],
						"val": [1, 3],
						"Xmin": 0.0
					}
				]
			}
		},
		{
			"id": "Plot",
			"parameters":
			{
				"editable": true,
				"caption": "BPol4(M)",
				"X":
				{
					"caption": "M",
					"type": null
				},
				"Y":
				{
					"caption": "BPol4",
					"type": null
				},
				"singleCurves":
				[
					{
						"caption": "BPol4(M)",
						"array": [0, 4, 0, 0],
						"arg": [0],
						"val": [1, 4],
						"Xmin": 0.0
					}
				]
			}
		},			
		{
			"id": "Plot",
			"parameters":
			{
				"editable": true,
				"caption": "Pmax(H,M)",
				"X":
				{
					"caption": "M",
					"type": null
				},
				"Y":
				{
					"caption": "Pmax",
					"type": "Pt"
				},
				"multiCurves":
				[
					{
						"Z":
						{
							"caption": "H",
							"type": "D"
						},
						"root": [0, 5, 1],
						"parameter": [0],
						"array": [1],
						"arg": [0],
						"val": [1, 0, 0]
					}
				]
			}
		},
		{
			"id": "Plot",
			"parameters":
			{
				"condition": [0, 5, 0],
				"editable": true,
				"caption": "Paft(H,M)",
				"X":
				{
					"caption": "M",
					"type": null
				},
				"Y":
				{
					"caption": "Paft",
					"type": "Pt"
				},
				"multiCurves":
				[
					{
						"Z":
						{
							"caption": "H",
							"type": "D"
						},
						"root": [0, 5, 1],
						"parameter": [0],
						"array": [1],
						"arg": [0],
						"val": [1, 0, 1]
					}
				]
			}
		},
		{
			"id": "Plot",
			"parameters":
			{
				"editable": true,
				"caption": "Ce max(H,M)",
				"X":
				{
					"caption": "M",
					"type": null
				},
				"Y":
				{
					"caption": "Ce max",
					"type": "Ce"
				},
				"multiCurves":
				[
					{
						"Z":
						{
							"caption": "H",
							"type": "D"
						},
						"root": [0, 5, 1],
						"parameter": [0],
						"array": [1],
						"arg": [0],
						"val": [1, 1, 0]
					}
				]
			}
		},
		{
			"id": "Plot",
			"parameters":
			{
				"condition": [0, 5, 0],
				"editable": true,
				"caption": "Ce aft(H,M)",
				"X":
				{
					"caption": "M",
					"type": null
				},
				"Y":
				{
					"caption": "Ce aft",
					"type": "Ce"
				},
				"multiCurves":
				[
					{
						"Z":
						{
							"caption": "H",
							"type": "D"
						},
						"root": [0, 5, 1],
						"parameter": [0],
						"array": [1],
						"arg": [0],
						"val": [1, 1, 1]
					}
				]
			}
		}
	],
	"tests":
	{
		"VyMax":
		{
			"parameters":
			[
				{
					"id": "Tree",
					"parameters":
					{
						"editable": true,
						"refEditable": true
					}
				}
			],
			"result":
			[
				{
					"id": "Plot",
					"parameters":
					{
						"editable": false,
						"refEditable": true,
						"caption": "VyMax(V)",
						"X":
						{
							"path": [1],
							"variants":
							[
								{
									"caption": "M",
									"type": null
								},
								{
									"caption": "TAS",
									"type": "V"
								},
								{
									"caption": "IAS",
									"type": "V"
								}
							]
						},
						"Y":
						{
							"caption": "VyMax",
							"type": "Vy"
						},
						"singleCurves":
						[
							{
								"caption": "VyMax(V)",
								"array": [0],
								"arg": [0],
								"val": [1]
							}
						]
					}
				}
			]
		},
		"VminVmax":
		{
			"parameters":
			[
				{
					"id": "Tree",
					"parameters":
					{
						"editable": true,
						"refEditable": true
					}
				}
			],
			"result":
			[
				{
					"id": "Plot",
					"parameters":
					{
						"editable": false,
						"refEditable": true,
						"caption": "Vmin(H), Vmax(H)",
						"X":
						{
							"path": [2],
							"variants":
							[
								{
									"caption": "M",
									"type": null
								},
								{
									"caption": "TAS",
									"type": "V"
								},
								{
									"caption": "IAS",
									"type": "V"
								}
							]
						},
						"Y":
						{
							"caption": "H",
							"type": "D"
						},
						"singleCurves":
						[
							{
								"caption": "Vmin(H)",
								"array": [0],
								"arg": [0],
								"val": [1],
								"pen":
								{
									"color":
									{
										"red": 0,
										"green": 0,
										"blue": 0
									}
								}
							},
							{
								"caption": "Vmax(H)",
								"array": [1],
								"arg": [0],
								"val": [1],
								"pen":
								{
									"color":
									{
										"red": 64,
										"green": 64,
										"blue": 64
									}
								}
							}
						]
					}
				}
			]
		},
		"TurnRate":
		{
			"parameters":
			[
				{
					"id": "Tree",
					"parameters":
					{
						"editable": true,
						"refEditable": true
					}
				}
			],
			"result":
			[
				{
					"id": "Plot",
					"parameters":
					{
						"editable": false,
						"refEditable": true,
						"caption": "Ny(H,V)",
						"X":
						{
							"path": [1],
							"variants":
							[
								{
									"caption": "M",
									"type": null
								},
								{
									"caption": "TAS",
									"type": "V"
								},
								{
									"caption": "IAS",
									"type": "V"
								}
							]
						},
						"Y":
						{
							"caption": "Ny",
							"type": null
						},						
						"parameter":
						{
							"caption": "H",
							"type": "D"
						},
						"multiCurves":
						[
							{
								"Z":
								{
									"caption": "H",
									"type": "D"
								},
								"root": [0],
								"parameter": [0],
								"array": [1],
								"arg": [0],
								"val": [1]
							}
						]
					}
				}
			]
		},
		"AoA":
		{
			"parameters":
			[
				{
					"id": "Tree",
					"parameters":
					{
						"editable": true,
						"refEditable": true
					}
				}
			],
			"result":
			[
				{
					"id": "Plot",
					"parameters":
					{
						"editable": false,
						"refEditable": true,
						"caption": "AoA(V)",
						"X":
						{
							"path": [1],
							"variants":
							[
								{
									"caption": "M",
									"type": null
								},
								{
									"caption": "TAS",
									"type": "V"
								},
								{
									"caption": "IAS",
									"type": "V"
								}
							]
						},
						"Y":
						{
							"caption": "AoA",
							"type": "angle"
						},
						"singleCurves":
						[
							{
								"caption": "AoA(V)",
								"array": [0],
								"arg": [0],
								"val": [1]
							}
						]
					}
				}
			]
		},
		"MxMy":
		{
			"parameters":
			[
				{
					"id": "Tree",
					"parameters":
					{
						"editable": true,
						"refEditable": true
					}
				}
			],
			"result":
			[
				{
					"id": "Plot",
					"parameters":
					{
						"editable": false,
						"refEditable": true,
						"caption": "Mx(AoS), My(AoS)",
						"X":
						{
							"caption": "AoS",
							"type": "angle"
						},
						"Y":
						{
							"caption": "M",
							"type": null
						},
						"singleCurves":
						[
							{
								"caption": "Mx(AoS)",
								"array": [0],
								"arg": [0],
								"val": [1],
								"pen":
								{
									"color":
									{
										"red": 255,
										"green": 0,
										"blue": 0
									}
								}
							},
							{
								"caption": "My(AoS)",
								"array": [1],
								"arg": [0],
								"val": [1],
								"pen":
								{
									"color":
									{
										"red": 0,
										"green": 0,
										"blue": 255
									}
								}
							}
						]
					}
				}
			]
		},
		"OmegaX":
		{
			"parameters":
			[
				{
					"id": "Tree",
					"parameters":
					{
						"editable": true,
						"refEditable": true
					}
				}
			],
			"result":
			[
				{
					"id": "Plot",
					"parameters":
					{
						"editable": false,
						"refEditable": true,
						"caption": "OmegaX(V)",
						"X":
						{
							"path": [1],
							"variants":
							[
								{
									"caption": "M",
									"type": null
								},
								{
									"caption": "TAS",
									"type": "V"
								},
								{
									"caption": "IAS",
									"type": "V"
								}
							]
						},
						"Y":
						{
							"caption": "OmegaX",
							"type": "omega"
						},
						"singleCurves":
						[
							{
								"caption": "OmegaX(V)",
								"array": [0],
								"arg": [0],
								"val": [1]
							}
						]
					}
				}
			]
		},
		"Mz":
		{
			"parameters":
			[
				{
					"id": "Tree",
					"parameters":
					{
						"editable": true,
						"refEditable": true
					}
				}
			],
			"result":
			[
				{
					"id": "Plot",
					"parameters":
					{
						"editable": false,
						"refEditable": true,
						"caption": "Mz(AoA)",
						"X":
						{
							"caption": "AoA",
							"type": "angle"
						},
						"Y":
						{
							"caption": "Mz",
							"type": null
						},						
						"singleCurves":
						[
							{
								"caption": "Mz(AoA)",
								"array": [0],
								"arg": [0],
								"val": [1]
							}
						]
					}
				}
			]
		},
	}
}