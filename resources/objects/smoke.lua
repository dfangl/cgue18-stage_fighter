particles.smoke = ParticleSystem(1.0, "particlesystem", "smoke_red.png", {

    init = function(this)
        this.setSize(0.7, 0.7)
        this.nz1 = 29.2938648971363;
        this.nz2 = 24.1293682342339;
        this.nz3 = 39.3745902239486;
    end,

    generateParticles = function(this, count)
        for i=0, count do
            local nx = i / count * 1.128128364;
            local ny = i / count * 0.923865875;

            local speed = noise.stb_perlin_ridge_noise3(18 * nx, 18 * ny, 5 * this.nz1, 2.0, 0.5, 1.0, 24, 18, 18, 64) * 2000
            local ttl   = math.random(1000, 1499)

            local noiseX = ( 0.5 - noise.stb_perlin_ridge_noise3(9 * nx, 9 * ny, this.nz1, 2.0, 0.52734, 1.0, 24, 9, 9, 64) ) / speed
            local noiseY = ( 0.5 - noise.stb_perlin_ridge_noise3(9 * nx, 9 * ny, this.nz2, 1.9, 0.49237, 1.0, 24, 9, 9, 64) ) / speed
            local noiseZ = ( 0.5 - noise.stb_perlin_ridge_noise3(8 * nx, 9 * ny, this.nz3, 1.8, 0.51278, 1.0, 24, 9, 9, 64) ) / speed

            local x = math.cos(math.random(0, 360)) / 50.4
            local y = math.sin(math.random(0, 360)) / 50.4
            local z = math.tan(math.random(0, 360)) / 50.4

            this.push_particle(
                vec3(x, y, z),
                vec3(noiseX, noiseY, noiseZ),
                ttl - math.random(1000, 1499),
                ttl
            )
        end
    end
})