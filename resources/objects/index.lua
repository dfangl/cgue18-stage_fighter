--
-- Index File which does describe the objects in this folder
--

-- Use these Arrays to get the objects
dynamics = {}
statics  = {}

-- Path of the objects directory
root = "../resources/objects/"

-- load all needed objects
dofile(root.."enemy1.lua")
dofile(root.."turrent.lua")

dofile(root.."wall.lua")
dofile(root.."coliseum.lua")