{
	"geometry" :
	{
		"type" : "Transform",
		"name" : "resize",
		"children":
		[
			{
				"type" : "Cylinder",
				"name" : "Cylinder",
				"radius" : 1.0,
				"height" : 1.0
			}
		]
	},
	"obb" :
	{
		"min": [-1.0, -1.0, -0.5],
		"max": [1.0, 1.0, 1.0]
	},
	"animation" :
	[
		[
			{
				"type" : "TransformScale",				
				"transformName" : "resize",
				"axis": 0
			},
			{
				"type" : "TransformScale",				
				"transformName" : "resize",
				"axis": 1
			}
		],
		[
			{
				"type" : "TransformScale",				
				"transformName" : "resize",
				"axis": 2
			}
		],
		[
		],
		[
			{
				"type": "ShapeColor",
				"shapeName": "Cylinder",
				"color": 0
			}
		],
		[
			{
				"type": "ShapeColor",
				"shapeName": "Cylinder",
				"color": 1
			}
		],
		[
			{
				"type": "ShapeColor",
				"shapeName": "Cylinder",
				"color": 2
			}
		]
	]
}