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

deathHeight = -5.0
skybox = { file="../resources/texture/skybox/Daylight Box", ext=".jpg"}

objects = {
    statics.tutorial_ground(vec3(0,0,0), vec4(0,0.707107,0,0.707107)),
    statics.house_1(vec3(-65.8838,0,71.5784), vec4(0,-0.999674,0,0.025531)),
    statics.house_3(vec3(-25.6816,0,61.7535), vec4(0,0.085469,0,0.996341)),
    statics.house_2(vec3(-64.1813,0,34.3156), vec4(0,0.818018,0,0.575193)),
    statics.coliseum(vec3(181.036,40.5924,-15.9153), vec4(0,-0.073578,0,-0.073578)),

    statics.fence(vec3(-34.4248,0,43.0125), vec4(0,-0.510005,0,0.860171)),
    statics.fence(vec3(-32.0085,0,44.677 ), vec4(0, 0.113551,0,0.993532)),
    statics.fence(vec3(-26.8121,0,35.942 ), vec4(0, 0.54663,0,0.837375)),


    -- make a question mark or so ...
    StaticObject("question_mark", {"outlines", "standard"}, vec3(-50.1786, 1, 45.2672),  0.2, vec4(0,0,0,1), { BoxShape(vec3(0,0,0), vec4(0,0,0,1), 0, vec3(0.1/2, 0.1/2, 0.1/2)) }),
    -- make a test ball for cel shading
    StaticObject("ball", {"outlines", "standard"}, vec3(-55.1786, 1, 45.2672),  0.2, vec4(0,0,0,1), { BoxShape(vec3(-55.1786, 2.14664, 45.2672), vec4(0,0,0,1), 0, vec3(0.576059/2, 2.14664/2, 1.96413/2)) } ),

}

scriptedObjects = {}

particlesystem = {
    { position=vec3(-65.8838,5,71.5784), number=45, particle=particles.explosion },
    { position=vec3(-64.1813,5,34.3156), number=45, particle=particles.explosion },
    { position=vec3(-25.6816,5,61.7535), number=45, particle=particles.explosion }
}

entities = {
    dynamics.turret("Broken turret", 5,  vec3(5,5,5), vec4(0.707107/2,0.707107/2,0,0.707107), behavior.doNothing())
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

    -- Light for Question mark 1:
    PointLight(vec3(-50.1786, 3.5, 45.2672), vec3(0.0,0.0,0.0), vec3(0.3,0.3,0.3), vec3(0.2,0.2,0.2), 25),
    PointLight(vec3(-54.1786, 3.5, 45.2672), vec3(0.0,0.0,0.0), vec3(0.3,0.3,0.3), vec3(0.2,0.2,0.2), 25)
}