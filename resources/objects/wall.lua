statics.wall =
function (position, rotation)
    return StaticObject(
        "wall",
        "standard",
        position,
        rotation,
        {
            BoxShape(
                vec3(position:x(),position:y()+2.14664/2,position:z()),
                rotation,
                0,
                vec3(0.576059/2, 2.14664/2, 5.96413/2)
            ),
        }
    )
end
