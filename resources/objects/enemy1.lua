dynamics.enemy1 =
function (name, health, spawnTime, position, rotation)
    return EnemyEntity(
        name,
        health,
        spawnTime,
        position,
        rotation,
        "enemy_1",
        30.0,
        BoxShape(
            vec3(0,0,0),
            vec4(0,0,0,1),
            0,
            vec3(1.87421/2, 2.04659/2, 1.87421/2)
        )
    )
end
