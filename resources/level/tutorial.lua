-----------------------
--- Tutorial Level  ---
-----------------------

-- Header of the Level file
levelResourceName = "tutorial"
levelResourcePath = "level/"..levelResourceName.."/"

-- Import all imporant files for this level
dofile("../resources/objects/index.lua")
dofile("../resources/ai/index.lua")

-- Set Player properties:
player = {
    position = vec3(-58.7911, 1, 55.9402),
    lookAt   = vec3(-24.7897,-42.5088,1.65117),
    speed    = 5.5
}

skybox = { file="../resources/texture/skybox/Daylight Box", ext=".jpg"}

objects = {
    statics.tutorial_ground(vec3(0,0,0), vec4(0,0.707107,0,-0.707107)),
    statics.house_1(vec3(-72.7361,0,65.5897), vec4(0,-0.997505,0,-0.070596)),
    statics.house_2(vec3(-34.3156,0,64.1813), vec4(0,0.998308,0,-0.058153)),
    statics.house_3(vec3(-61.7535,0,25.6816), vec4(0,0.7365,0,-0.676438)),
    statics.coliseum(vec3(181.036,40.5924,-15.9153), vec4(0,-0.073578,0,-0.073578)),

    -- make a question mark or so ...
    StaticObject("question_mark", "standard", vec3(-50.1786, 1, 45.2672),  0.2, vec4(0,0,0,1), { BoxShape(vec3(0,0,0), vec4(0,0,0,1), 0, vec3(0.1/2, 0.1/2, 0.1/2)) }),

}

scriptedObjects = {}
particlesystem = {
    { position=vec3(-72.7361,5,65.5897), number=45, particle=particles.explosion },
    { position=vec3(-34.3156,5,64.1813), number=45, particle=particles.explosion },
    { position=vec3(-61.7535,5,25.6816), number=45, particle=particles.explosion }
}

entities = {
    dynamics.turret("Broken turret", 3,  vec3(5,5,5), vec4(0.707107/2,0.707107/2,0,0.707107), behavior.doNothing())
}

triggers = {
    -- Trigger at spawn location
    Trigger(vec3(-58.7911, 1, 55.9402), 1, {
        left = function() level:hideTextbox() end,
        entered = function() level:showTextbox(
            [[Hi there Warrior,
            use the W,A,S,D keys to move around and follow the path to the big white building over there.
            With the mouse you can also look around a bit and admire the chaos around you, but watch out
            for some nasty turrets on the way, they will most likely want to kill you.]]
        )
        end
    }),

    -- Trigger at question mark
    Trigger(vec3(-50.1786, 1, 45.2672), 3, {
        left = function() level:hideTextbox() end,
        entered = function() level:showTextbox(
            [[A short introduction into fighting:
            Use your right mouse button to block any enemy projectile, but watch out, blocking uses some
            endurance and you might run out of it if you need it the most!
            To attack an enemy use your left mouse button.
            ]]
        )
        end
    })
}

lights = {
    -- First Light gets manupulated by the Game settings
    PointLight(vec3(0,0,0), vec3(0.6,0.6,0.6), vec3(0,0,0), vec3(0,0,0), 0),

    -- Light for fires:
    PointLight(vec3(-72.7361,0,65.5897), vec3(0.0,0.0,0.0), vec3(0.9,0.1,0.1), vec3(0.3,0,0), 80),
    PointLight(vec3(-34.3156,0,64.1813), vec3(0.0,0.0,0.0), vec3(0.9,0.1,0.1), vec3(0.3,0,0), 80),
    PointLight(vec3(-61.7535,0,25.6816), vec3(0.0,0.0,0.0), vec3(0.9,0.1,0.1), vec3(0.3,0,0), 80),

    -- Light for Question mark 1:
    PointLight(vec3(-50.1786, 3.5, 45.2672), vec3(0.0,0.0,0.0), vec3(0.3,0.3,0.3), vec3(0.2,0.2,0.2), 25)
}