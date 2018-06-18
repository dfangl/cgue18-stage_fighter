scripted.platformX = function(position, speed, min, max)
    return ScriptedObject("platform", {"standard"}, position,  1, vec4(0.707107,0,0.707107,0), BoxShape(vec3(0,0,0), vec4(0,0,0,1), 8000, vec3(1, 0.449138/2, 1)), {}, 0, {
        zero = vec3(0,0,0),
        rot  = vec4(0.707107,0,0.707107,0),
        sticky = true,
        kind = 5,

        think = function(this, delta)
            local pos = this.getPosition();
            if pos:x() < min then
                this.dir = this.right
            end
            if pos:x() > max then
                this.dir = this.left
            end

            this.setLinearVelocity(this.dir)
            this.setGravity(this.zero)
            this.setAngularFactor(this.zero)
            this.setRotation(this.rot)
        end,

        init = function(this)
            this.left   = vec3(-speed,0 , 0)
            this.right  = vec3( speed, 0, 0)
            this.dir    = this.left

            this.spawnParticleSystem(particles.smoke, vec3(0,-0.3,0), 25)
            this.disableDeactivation()
        end
    })
end