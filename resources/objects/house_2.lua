statics.house_2 =
function (position, rotation)
    return StaticObject(
        "House_2",
        "standard",
        position,
        15.388/2,
        rotation,
        {
            BoxShape(vec3(position:x(),position:y()+5.01659,position:z()), vec4(0,0,0,1), 0, vec3(15.388/2, 10.2203/2, 15.388/2)),
        },
        {
        }
    )
end
