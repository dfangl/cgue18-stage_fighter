particles.rocket = ParticleSystem(1.0, "particlesystem", "explosion.png", {
    -- Only a magic number for the speed
    speed = 0.025 * 1.0 / 60.0,
    particles = 8,

    init = function(this)
        this.setSize(0.2, 0.2)
    end,

    generateParticles = function(this, count)
        local ttl = math.random(500, 750)
        local cLife = ttl - math.random(500, 750)

        local spawn = vec3(
                -this.direction:x() * this.hitbox:x()/2.0,
                -this.direction:y() * this.hitbox:y()/2.0,
                -this.direction:z() * this.hitbox:z()/2.0
        )

        local velocity = vec3(
                -this.direction:x() * this.speed,
                -this.direction:y() * this.speed,
                -this.direction:z() * this.speed
        )

        for i=0, count do
            this.push_particle(spawn, velocity, cLife, ttl)
        end
    end
})