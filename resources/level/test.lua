-----------------------
--- Test Level File ---

-- Header of the Level file
levelResourceName = "test"
levelResourcePath = "level/"..levelResourceName.."/"

-- Import all imporant files for this level
dofile("../resources/objects/index.lua")
dofile("../resources/ai/index.lua")

-- Set Player properties:
player = {
    position = vec3(-50.188, -42.5088, 0), -- -50.188, -42.5088, 0
    lookAt   = vec3(-24.7897,-42.5088,1.65117),
    speed    = 5.5
}

deathHeight = -60.0
skybox = { file="../resources/texture/skybox/Daylight Box", ext=".jpg"}

objects = {
    statics.coliseum(vec3(0,0,0), vec4(0,0,0,1)),
    statics.wall(vec3(-42.8338, -42.5088,       0), vec4(0,0,0,1), "north"),
    statics.wall(vec3(-57.9427, -42.5088,       0), vec4(0,0,0,1), "south"),
    statics.wall(vec3(-50.08  , -42.5088, 8.18353), vec4(0,0.707107,0,0.707107), "east"),
    statics.wall(vec3(-50.08  , -42.5088,-8.18353), vec4(0,0.707107,0,0.707107), "west"),

    -- Light Cube:
    StaticObject("cube", {"light"}, vec3(-50.188 , -35 , 0.0),  0.2, vec4(0,0,0,1), { BoxShape(vec3(0,0,0), vec4(0,0,0,1), 0, vec3(0.1/2, 0.1/2, 0.1/2)) })
}

scriptedObjects = {
    scripted.platformY(vec3(-42.8338, -31, 0), 2.0, -40.0, -20.0),
    scripted.platformX(vec3(-52     , -28, 0), 0.7, -58.0, -45.0)
}

entities = {
    dynamics.turret("Turret #1", 10,  vec3(-24.7897,-42.5088,1.65117), vec4(0,0,0,1), behavior.shootAtPlayer(projectile.rocket, 200)),
    dynamics.turret("Turret #2", 10,  vec3(-50.2492,-42.5088,-25.1359), vec4(0,0.707107,0,0.707107), behavior.shootCircle(projectile.rocket, 800, 36)),
    dynamics.turret("Broken turret", 5,  vec3(-50.2492,-32.5088,5), vec4(0.707107/2,0.707107/2,0,0.707107), behavior.doNothing())
}

triggers = {
    Trigger(vec3(-24.7897,-42.5088,1.65117), 5, {
        left = function() level:hideTextbox() end,
        entered = function() level:showTextbox("Hi,\nI'm a very special turret and will always shoot in your direction\n\nBtw pls don't kill me :/") end
    })
}

particlesystem = {
    { position=vec3(-48.2492,-40.5088,5), number=10, particle=particles.burning_turret }
}

lights = {
    -- First Light gets manupulated by the Game settings
    PointLight(vec3(0,0,0), vec3(0.6,0.6,0.6), vec3(0,0,0), vec3(0,0,0), 0),
    PointLight(
        vec3(-50.188 , -35 , 0.0), -- Position
        vec3(0.0,0.0,0.0), -- Ambient
        vec3(0.2,0.2,0.2), -- Diffuse
        vec3(0.5,0.5,0.5), -- Specular
        20.0               -- Light power
    ),
    PointLight(
            vec3(-24.7897,-32.5088,1.65117), -- Position
            vec3(0.0,0.0,0.0), -- Ambient
            vec3(0.4,0.0,0.0), -- Diffuse
            vec3(0.4,0.0,0.0), -- Specular
            100.0               -- Light power
    ),
    PointLight(
            vec3(-50.2492,-32.5088,-25.1359), -- Position
            vec3(0.0,0.0,0.0), -- Ambient
            vec3(0.0,0.4,0.0), -- Diffuse
            vec3(0.0,0.4,0.0), -- Specular
            100.0               -- Light power
    ),
    PointLight(
            vec3(-50.2492,-32.5088, 5), -- Position
            vec3(0.0,0.0,0.0), -- Ambient
            vec3(0.0,0.0,0.4), -- Diffuse
            vec3(0.0,0.0,0.4), -- Specular
            100.0               -- Light power
    )
}

function show()
    music = level:play2DSound("/home/raphael/Downloads/The-Life-Aquatic_15616_audiocabin.wav", true)
end

function hide()
    music:destroy()
end

function victory() end
function lost() end