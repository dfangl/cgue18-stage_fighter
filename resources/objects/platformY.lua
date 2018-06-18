scripted.platformY = function(position, speed, min, max)
    return ScriptedObject("platform", {"standard"}, position,  1, vec4(0.707107,0,0.707107,0), BoxShape(vec3(0,0,0), vec4(0,0,0,1), 8000, vec3(1, 0.449138/2, 1)), {}, 0, {
        zero = vec3(0,0,0),
        rot  = vec4(0.707107,0,0.707107,0),
        think = function(this, delta)
            local pos = this.getPosition();
            if pos:y() < min then
                this.dir = this.up
            end
            if pos:y() > max then
                this.dir = this.down
            end

            this.setLinearVelocity(this.dir)
            this.setGravity(this.zero)
            this.setAngularFactor(this.zero)
            --this.setRotation(this.rot)
        end,

        init = function(this)
            this.up     = vec3(0, speed, 0)
            this.down   = vec3(0,-speed, 0)
            this.dir    = this.down

            this.spawnParticleSystem(particles.smoke, vec3(0,-0.3,0), 25)
            this.disableDeactivation()
        end
    })
end