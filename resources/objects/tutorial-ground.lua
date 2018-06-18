statics.tutorial_ground =
function (position, rotation)
    return StaticObject(
        "level/tutorial/ground",
        {"standard-splat"},
        position,
        156.80/2,
        rotation,
        {
            BoxShape(vec3(position:x(),position:y(),position:z()), vec4(0,0,0,1), 0, vec3(156.804/2, 2.0/2, 156.804/2)),
        },
        {
            [0] = "__gen_marble",
            [1] = "ground10.jpg",
            [2] = "ground02.jpg",
            [3] = "none.png",
            [4] = levelResourcePath.."Ground_Splat.png",
            [5] = levelResourcePath.."Ground_Lightmap.png",
        }
    )
end

statics.tutorial_ground =
function (position, rotation)
    return StaticObject(
            "level/tutorial/ground",
            {"standard-splat"},
            position,
            156.80/2,
            rotation,
            {
                BoxShape(vec3(position:x(),position:y(),position:z()), vec4(0,0,0,1), 0, vec3(156.804/2, 2.0/2, 156.804/2)),
            },
            {
                [0] = "__gen_marble",
                [1] = "ground10.jpg",
                [2] = "ground02.jpg",
                [3] = "none.png",
                [4] = levelResourcePath.."Ground_Splat.png",
                [5] = levelResourcePath.."Ground_Lightmap.png",
            }
    )
end

