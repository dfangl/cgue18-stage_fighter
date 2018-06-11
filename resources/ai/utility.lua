--
-- Some utility functions which are helpful to reduce code size
-- and make AI more readable
--

ai = {}

-- Shoots directly from 'this' with the projectile to the player position
ai.shootAtPlayer = function(this, projectile)
    playerPos  = level:getPlayerPos();
    emitterPos = this.getSpawnPosFor(playerPos)

    level:spawn(projectile, emitterPos, playerPos)
end

-- Shoots to position x,y,z
ai.shootAtPosition = function(this, projectile, x, y, z)
    targetPos  = vec3(x, y, z)
    emitterPos = this.getSpawnPosFor(targetPos)

    level:spawn(projectile, emitterPos, vec3(x, origin:y(), z))
end