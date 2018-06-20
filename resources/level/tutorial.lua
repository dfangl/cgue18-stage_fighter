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
    statics.house_3(vec3(-20.5778,0,56.521), vec4(0,0.085469,0,0.996341)),
    statics.house_2(vec3(-64.1813,0,34.3156), vec4(0,0.818018,0,0.575193)),
    statics.coliseum(vec3(181.036,40.5924,-15.9153), vec4(0,-0.073578,0,-0.073578)),

    statics.fence(vec3(-34.4248,0,43.0125), vec4(0,-0.510005,0,0.860171)),
    statics.fence(vec3(-32.0085,0,44.677 ), vec4(0, 0.113551,0,0.993532)),
    statics.fence(vec3(-26.8121,0,35.942 ), vec4(0, 0.54663,0,0.837375)),
    statics.fence(vec3(-16.8626,0,40.319 ), vec4(0, 0.54663,0,0.837375)),
    statics.fence(vec3(-29.5511,0,42.5301), vec4(0, 0.54663,0,0.837375)),
    statics.fence(vec3(-28.0856,0,39.528 ), vec4(0, 0.54663,0,0.837375)),
    statics.fence(vec3(-19.0725,0,43.6492), vec4(0, 0.54663,0,0.837375)),
    statics.fence(vec3(-21.3895,0,47.6124), vec4(0, 0.54663,0,0.837375)),

    statics.house_1(vec3(-46.6528,0,35.1595), vec4(0,-0.63625,0,-0.771484)),
    statics.house_1(vec3(-20.5553,0,68.6483), vec4(0,-0.255659,0,0.966767)),

    statics.question_mark(vec3(-29.7691,1.5,45.6461)),

    -- make a test ball for cel shading
    --StaticObject("ball", {"outlines", "standard"}, vec3(-55.1786, 1, 45.2672),  0.2, vec4(0,0,0,1), { BoxShape(vec3(-55.1786, 2.14664, 45.2672), vec4(0,0,0,1), 0, vec3(0.576059/2, 2.14664/2, 1.96413/2)) } ),

}

scriptedObjects = {}

particlesystem = {
    { position=vec3(-65.8838,5,71.5784), number=35, particle=particles.explosion },
    { position=vec3(-64.1813,5,34.3156), number=35, particle=particles.explosion },
    { position=vec3(-20.5778,5,56.521), number=35, particle=particles.explosion }
}

entities = {
    dynamics.turret("Broken turret", 5,  vec3(5,1,5), vec4(0,0,0,1),
            behavior.shootAtPlayerInRange(projectile.rocket, 1000, 20)
    ),
    dynamics.turret("Broken turret", 5,  vec3(-9.48663,1,27.0495), vec4(0,0,0,1),
            behavior.shootAtPlayerInRange(projectile.rocket, 850, 25)
    )
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
    Trigger(vec3(-29.7691,1.5,45.6461), 3, {
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
    PointLight(vec3(-29.7691,1.5,45.6461), vec3(0.0,0.0,0.0), vec3(0.4,0.4,0.4), vec3(0.1,0.1,0.1), 15),

    PointLight(vec3(-15,5,27.0495), vec3(0.0,0.0,0.0), vec3(0.4,0.4,0.4), vec3(0.1,0.1,0.1), 15),

    PointLight(vec3(10,5,5), vec3(0.0,0.0,0.0), vec3(0.4,0.4,0.4), vec3(0.1,0.1,0.1), 15),

}

music = {}

function show()
    music = level:play2DSound(AUDIO_FILE, true)
    music:setVolume(0.5)
end

function hide()
    music:stop()
    music:destroy()
end

function victory() end
function lost() end