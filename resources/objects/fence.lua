statics.fence =
function (position, rotation)
    return StaticObject(
            "fence",
            {"outlines", "standard"},
            position,
            3.19442/2,
            rotation,
            {
                BoxShape(vec3(position:x()+0.355438,position:y()+0.851993,position:z()), rotation, 0, vec3(3.19442/2, 1.92295/2, 0.337023/2)),
            },
            {
                [1] = levelResourcePath.."House_1-Lightmap.png"
            }
    )
end
