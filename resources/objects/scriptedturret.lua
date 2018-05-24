dynamics.sturret =
function (name, health, position, rotation)
    return ScriptedEntity(
        name,
        health,
        position,
        rotation,
        "turrent",
        30.0,
        BoxShape(
            vec3(0,-0.898103/2-2.23527/2,0),
            vec4(0,0,0,1),
            0,
            vec3(2.37508/2, 2.52301/2, 2.23527/2)
        ),
        {
            spawnTime = 200,

            think = function(env, delta)
                env.spawnTime = env.spawnTime - delta
                if (env.spawnTime < 0) then
                    env.spawnTime = 200

                    playerPos  = level:getPlayerPos();
                    emitterPos = env.getSpawnPosFor(playerPos)

                    level:spawn(projectiles["bullet"], emitterPos, playerPos)
                end
            end
        }
    )
end