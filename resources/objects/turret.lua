dynamics.turret =
function (name, health, position, rotation, behavior)
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
        behavior
    )
end