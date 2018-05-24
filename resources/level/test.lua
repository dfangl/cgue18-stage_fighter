-- Test Level File --

-- Import all Objects which are available
dofile("../resources/objects/index.lua")

levelResourceName = "test"
levelResourcePath = "../resources/textures/"..levelResourceName.."/"

-- Set Player properties:
player = {
    position = vec3(-50.188, -42.5088, 0),
    lookAt   = vec3(-24.7897,-42.5088,1.65117),
    speed    = 5.5
}

projectiles = {
    ["bullet"] = Projectile("bullet", 5.0, 0.00001, BoxShape(vec3(0,0,0), vec4(0,0,0,1), 0, vec3(0.174505/2, 0.174505/2, 0.286695/2)))
}

behavior = {
    shootAtPlayer = function(spawnSpeed)
        return {
            spawnTime = spawnSpeed,

            think = function(this, delta)
                this.spawnTime = this.spawnTime - delta
                if (this.spawnTime < 0) then
                    this.spawnTime = spawnSpeed

                    playerPos  = level:getPlayerPos();
                    emitterPos = this.getSpawnPosFor(playerPos)

                    level:spawn(projectiles["bullet"], emitterPos, playerPos)
                end
            end
        }
    end,

    doNothing = {
        mustBeKilled = false,
        think = function(this, delta) end
    }
}

objects = {
    statics.coliseum(vec3(0,0,0), vec4(0,0,0,1)),
    statics.wall(vec3(-42.8338, -42.5088,       0), vec4(0,0,0,1)),
    statics.wall(vec3(-57.9427, -42.5088,       0), vec4(0,0,0,1)),
    statics.wall(vec3(-50.08  , -42.5088, 8.18353), vec4(0,0.707107,0,0.707107)),
    statics.wall(vec3(-50.08  , -42.5088,-8.18353), vec4(0,0.707107,0,0.707107)),

    -- Light Cube:
    StaticObject("cube", "light", vec3(-50.188 , -35 , 0.0),  vec4(0,0,0,1), { BoxShape(vec3(0,0,0), vec4(0,0,0,1), 0, vec3(0.1/2, 0.1/2, 0.1/2)) }),
}

entities = {
    dynamics.turret("Turret #1", 1,  vec3(-24.7897,-42.5088,1.65117), vec4(0,0,0,1), behavior.shootAtPlayer(200)),
    dynamics.turret("Turret #2", 1,  vec3(-50.2492,-42.5088,-25.1359), vec4(0,0.707107,0,0.707107), behavior.shootAtPlayer(200)),
    dynamics.turret("Turret #4", 5,  vec3(-50.2492,-32.5088,5), vec4(0.707107/2,0.707107/2,0,0.707107), behavior.doNothing)
}

lights = {
    PointLight(
        vec3(-50.188 , -35 , 0.0), -- Position
        vec3(0.6,0.6,0.6), -- Ambient
        vec3(0.7,0.7,0.7), -- Diffuse
        vec3(0.5,0.5,0.5), -- Specular
        80.0               -- Light power
    )
}