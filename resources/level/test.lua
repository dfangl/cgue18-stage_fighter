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

objects = {
    statics.coliseum(vec3(0,0,0), vec4(0,0,0,1)),
    statics.wall(vec3(-42.8338, -42.5088,       0), vec4(0,0,0,1), "north"),
    statics.wall(vec3(-57.9427, -42.5088,       0), vec4(0,0,0,1), "south"),
    statics.wall(vec3(-50.08  , -42.5088, 8.18353), vec4(0,0.707107,0,0.707107), "east"),
    statics.wall(vec3(-50.08  , -42.5088,-8.18353), vec4(0,0.707107,0,0.707107), "west"),

    -- Light Cube:
    StaticObject("cube", "light", vec3(-50.188 , -35 , 0.0),  0.2, vec4(0,0,0,1), { BoxShape(vec3(0,0,0), vec4(0,0,0,1), 0, vec3(0.1/2, 0.1/2, 0.1/2)) })
}

scriptedObjects = {
    ScriptedObject("cube", "light", vec3(-42.8338, -32.5088,       00),  0.2, vec4(0,0,0,1), BoxShape(vec3(0,0,0), vec4(0,0,0,1), 8000, vec3(0.1/2, 0.1/2, 0.1/2)), {}, 0, {

        zero = vec3(0,0,0),

        think = function(this, delta)
            local pos = this.getPosition();
            if pos:y() < -40.0 then
                this.dir = this.up
            end
            if pos:y() > -20.0 then
                this.dir = this.down
            end

            this.setLinearVelocity(this.dir)
            this.setGravity(this.zero)
            this.setAngularFactor(this.zero)
        end,

        init = function(this)
            local speed = 5.0
            this.up     = vec3(0.0, speed, 0.0)
            this.down   = vec3(0.0, -speed, 0.0)
            this.dir    = this.down

            this.spawnParticleSystem(particles.smoke, vec3(0,-0.8,0), 100)
        end
    })
}

entities = {
    dynamics.turret("Turret #1", 10,  vec3(-24.7897,-42.5088,1.65117), vec4(0,0,0,1), behavior.shootAtPlayer(projectile.rocket, 200)),
    dynamics.turret("Turret #2", 10,  vec3(-50.2492,-42.5088,-25.1359), vec4(0,0.707107,0,0.707107), behavior.shootCircle(projectile.rocket, 800, 36)),
    dynamics.turret("Turret #4", 5,  vec3(-50.2492,-32.5088,5), vec4(0.707107/2,0.707107/2,0,0.707107), behavior.doNothing())
}

lights = {
    PointLight(
        vec3(-50.188 , -35 , 0.0), -- Position
        vec3(0.6,0.6,0.6), -- Ambient
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