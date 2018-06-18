--
-- Index File which does describe the objects in this folder
--

-- Use these Arrays to get the objects
dynamics    = {}
statics     = {}
particles   = {}
projectiles = {}
projectile  = {}
scripted    = {}

-- Path of the objects directory
root = "../resources/objects/"

-- load all entities
dofile(root.."turret.lua")

-- load all static objects
dofile(root.."wall.lua")
dofile(root.."coliseum.lua")
dofile(root.."tutorial-ground.lua")
dofile(root.."house_1.lua")
dofile(root.."house_2.lua")
dofile(root.."house_3.lua")

-- load all particle systems
dofile(root.."rocket-smoke.lua")
dofile(root.."smoke.lua")
dofile(root.."explosion.lua")

-- Create projectiles & projectile mapping table
projectile.rocket = 1; projectiles[1] = Projectile("bullet", {"standard-instanced"}, 5.0, 0.286695/2, 0.00001, vec3(0.174505/2, 0.174505/2, 0.286695/2), particles.rocket)


-- Load all scripted objects:
dofile(root.."platformX.lua")
dofile(root.."platformY.lua")