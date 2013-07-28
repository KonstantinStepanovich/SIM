{
	"geometry" :
	{
		"type" : "Transform",
		"name" : "resize",
		"children":
		[
			{
				"type": "Sphere",
				"name": "Shpere",
				"radius": 1.0
			}
		]
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
			},
			{
				"type" : "TransformScale",				
				"transformName" : "resize",
				"axis": 2
			}
		],
		[
		],
		[
		],
		[
			{
				"type": "ShapeColor",
				"shapeName": "Shpere",
				"color": 0
			}
		],
		[
			{
				"type": "ShapeColor",
				"shapeName": "Shpere",
				"color": 1
			}
		],
		[
			{
				"type": "ShapeColor",
				"shapeName": "Shpere",
				"color": 2
			}
		]
	]
}