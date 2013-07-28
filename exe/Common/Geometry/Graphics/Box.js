{
	"geometry" :
	{
		"type" : "Transform",
		"name" : "resize",
		"children":
		[
			{
				"type": "Box",
				"name": "Box",
				"size": [ 1.0, 1.0, 1.0 ]
			}
		]
	},
	"obb" :
	{
		"min": [-1.0, -1.0, -1.0],
		"max": [1.0, 1.0, 1.0]
	},
	"animation" :
	[
		[
			{
				"type" : "TransformScale",				
				"transformName" : "resize",
				"axis": 0
			}
		],
		[
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
			{
				"type": "ShapeColor",
				"shapeName": "Box",
				"color": 0
			}
		],
		[
			{
				"type": "ShapeColor",
				"shapeName": "Box",
				"color": 1
			}
		],
		[
			{
				"type": "ShapeColor",
				"shapeName": "Box",
				"color": 2
			}
		]
	]
}