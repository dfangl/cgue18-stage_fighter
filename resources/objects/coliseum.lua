statics.coliseum =
function (position, rotation)
    return StaticObject(
        "coliseum",     -- Model File
        "standard-splat",     -- Shader
        position,       -- Position
        rotation,  -- Rotation
        {               -- List of Bullet primitives
            BoxShape(vec3(position:x()-34.3794,position:y()-49.5088,position:z()+0      ), vec4(0,        0,0,        1), 0, vec3(236.465/2, 15.7829/2, 324.826/2)),
            BoxShape(vec3(position:x()-11.1967,position:y()-20.3659,position:z()-73.7768), vec4(0,-0.269593,0, 0.962974), 0, vec3(32.5442/2, 45.0000/2, 24.0786/2)),
            BoxShape(vec3(position:x()-39.4035,position:y()-20.3659,position:z()-83.3338), vec4(0,-0.050905,0, 0.998703), 0, vec3(25.5098/2, 45.0000/2, 24.0301/2)),
            BoxShape(vec3(position:x()-67.8871,position:y()-20.3659,position:z()-80.2732), vec4(0, 0.148316,0, 0.98894 ), 0, vec3(29.0254/2, 45.0000/2, 24.3611/2)),
            BoxShape(vec3(position:x()-94.0301,position:y()-20.3659,position:z()-65.7971), vec4(0, 0.343032,0, 0.939324), 0, vec3(29.0254/2, 45.0000/2, 24.3611/2)),
            BoxShape(vec3(position:x()-113.868,position:y()-20.3659,position:z()-41.1359), vec4(0, 0.509213,0, 0.860641), 0, vec3(31.6023/2, 45.0000/2, 25.3552/2)),
            BoxShape(vec3(position:x()-123.089,position:y()-20.3659,position:z()-9.80413), vec4(0, 0.687045,0, 0.726615), 0, vec3(34.2994/2, 45.0000/2, 25.3574/2)),
            BoxShape(vec3(position:x()-120.741,position:y()-20.3659,position:z()+23.624 ), vec4(0, 0.783052,0, 0.621956), 0, vec3(34.2994/2, 45.0000/2, 25.3574/2)),
            BoxShape(vec3(position:x()-107.543,position:y()-20.3659,position:z()+53.418 ), vec4(0, 0.880312,0, 0.474395), 0, vec3(34.2994/2, 45.0000/2, 25.3574/2)),
            BoxShape(vec3(position:x()-84.4599,position:y()-20.3659,position:z()+73.5872), vec4(0, 0.961243,0, 0.275703), 0, vec3(34.2994/2, 45.0000/2, 25.3574/2)),
            BoxShape(vec3(position:x()-44.8333,position:y()-20.3659,position:z()+83.483 ), vec4(0, 0.999852,0, 0.017187), 0, vec3(54.1044/2, 45.0000/2, 25.3797/2)),
            BoxShape(vec3(position:x()-1.79641,position:y()-20.3659,position:z()+66.0733), vec4(0, 0.954363,0,-0.298647), 0, vec3(51.1271/2, 45.0000/2, 25.9137/2)),
            BoxShape(vec3(position:x()+12.5873,position:y()-20.3659,position:z()-49.1656), vec4(0,-0.480589,0, 0.876946), 0, vec3(32.5442/2, 45.0000/2, 24.0786/2)),
            BoxShape(vec3(position:x()+26.4389,position:y()-20.3659,position:z()-19.7768), vec4(0,-0.587641,0, 0.809121), 0, vec3(35.7598/2, 45.0000/2, 24.3414/2)),
            BoxShape(vec3(position:x()+29.0218,position:y()-20.3659,position:z()+13.3486), vec4(0,-0.754691,0, 0.65608 ), 0, vec3(35.8593/2, 45.0000/2, 24.1294/2)),
            BoxShape(vec3(position:x()+21.7294,position:y()-20.3659,position:z()+38.8318), vec4(0,-0.870369,0, 0.4924  ), 0, vec3(32.5442/2, 45.0000/2, 24.0786/2)),
        },
        {
            [1] = "__gen_marble",
            [3] = levelResourcePath.."coliseum-lightmap.png"
        }
    )
end
