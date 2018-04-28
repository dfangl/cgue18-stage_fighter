Level File Format
==================
This file does specify the Level file format:

The File must contain a `player` object, and `objects`, `entities` and `lights` lists which do
describe the current level.

## player Object
The file must contain a `player` object which does define following properties:
* `position`: from type `vec3` which sets the X,Y,Z Coordinates of the Player
* `lookAt`: from type `vec3` which describes the X,Y,Z position to which the camera should look
* `speed`: from type `number` which describes the speed, should be positive otherwise controls are inverted

## objects List
The Objects list must only contain `StaticObject` instances:
```lua
StaticObject(
	"model",		-- Model File without .gltf
	"standard",     -- Shadername
	vec3(X,Y,Z),    -- Position
	vec4(X,Y,Z,W),  -- Rotation
	{}              -- List of Bullet primitives
)
```
A StaticObject without a list of Bullet primitives is valid but than there is no interaction with the
player or other entities !

## entities List
Currently only a generic Enemy class is supported by the Level format:
```lua
EnemyEntity(
	"Enemy #1",				-- Name of the Entity which is displayed in the HUD				
	50,						-- Max Health of the Entity
	5000,					-- Time in ms between projectiles are emitted
	vec3(X,Y,Z),			-- Position
	vec4(X,Y,Z,W),			-- Rotation
	"enemy_1",				-- Model File without .gltf
	30.0,					-- Mass of the Entity
	BoxShape(				-- Collision Box (generic Shape is also possible)
		vec3(X,Y,Z),			-- Position
		vec4(X,Y,Z,W),			-- Rotation
		0,						-- Mass (ignored)
		vec3(W/2, H/2, D/2)		-- Sizes from the middle point
	)
)
```


## lights List 	
Only Point Lights are supported: 
```lua
PointLight(
	vec3(X,Y,Z),       -- Position
	vec3(0.4,0.4,0.4), -- Ambient
	vec3(0.7,0.7,0.7), -- Diffuse
	vec3(0.5,0.5,0.5)  -- Specular
					   -- Light strength (currently not implemented)
)
```

## Sample level.lua File:	
```lua
player = {}
player.position = vec3(X, Y, Z)
player.lookAt   = vec3(X, Y, Z)
player.speed    = Speed

-- List which contains all static Objects
objects = {}

-- List which contains all Entites 
entities = {}

-- List with all Light sources
lights = {}
```