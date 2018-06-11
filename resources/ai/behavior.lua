--
-- Index File which does describe different behaviors for the enemies
--
behavior = {}

-- An empty behavior which does nothing
behavior.doNothing = function ()
    return {
        mustBeKilled = false,
        think = function(this, delta) end
    }
end

behavior.shootAtPlayer = function(projectile, spawnSpeed)
    return {
        spawnTime = spawnSpeed,

        think = function(this, delta)
            this.spawnTime = this.spawnTime - delta

            if (this.spawnTime < 0) then
                this.spawnTime = spawnSpeed
                ai.shootAtPlayer(this, projectile)
            end
        end
    }
end

behavior.shootCircle = function(projectile, spawnSpeed, numofProjectiles)
    return {
        spawnTime = spawnSpeed,

        think = function(this, delta)
            this.spawnTime = this.spawnTime - delta

            if (this.spawnTime < 0) then
                -- Variable Spawn time
                this.spawnTime = math.random(spawnSpeed / 2, spawnSpeed * 2)

                origin = this.getPosition()

                -- Calculate cirlce and shoot at the positions
                for i=1, numofProjectiles do
                    x = origin:x() + 10 * math.cos(math.pi * 2 * i / numofProjectiles)
                    z = origin:z() + 10 * math.sin(math.pi * 2 * i / numofProjectiles)

                    ai.shootAtPosition(this, projectile, x, origin:y(), z)
                end
            end
        end
    }
end