--
-- Gloabal configuration file for Stage Fighter
--      (Values might not be typechecked and range checked by the runtime, so only change stuff if you know
--       what you do!)

-- Set window properties, some of these proeprties are irgnored when invalid
window = {
    width=1280,         -- Width  of Window in px
    height=768,         -- Height of Window in px
    vsync=false,         -- true to enable vsync false to disable

    -- Set Fullscreen Mode:
    --      -2      Windowed Fullscreen (Experimental)
    --      -1      Normal Window mode
    --       0  >=  Fullscreen mode, invalid width & height may be ignored or app might crash
    --              Opens Fullscreen on Monitor with this index
    fullscreen=-1,

    -- Set Refreshrate in Hz (does only work in Fullscreen mode, invalid modes are ignored)
    --      -1      Default Monitor refresh rate
    --      Typical values are: 30,60,75, or sometimes even 23,24,26,27,29
    refreshRate=-1,

    -- Set Gamma value of Monitor (Only works in Fullscreen Mode)
    --  It uses the glfwSetGamma function which does rely on WinAPI or X11 functions to set Gamma
    --  Must be a float value > 0.0, otherwise values might be ignored or App might crash
    gamma=1.0
}

-- Set Camera properties:
camera = {
    fov=45
}

-- Set Debug properties, should be disabled if not debugging one of these subsystems:
--      true to enable debugging or false to disable debuggiing
debug = {
    bullet=false,        -- Create a Bullet Debug Context and draw all the Bounding Boxes as Lines to the screen
    opengl=false          -- Debug OpenGL Context and enable error outputs for OpenGL
}