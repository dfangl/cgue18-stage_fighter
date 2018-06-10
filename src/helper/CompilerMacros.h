//
// Created by Raphael on 10.06.2018.
//

#ifndef STAGE_FIGHTER_COMPILERMACROS_H
#define STAGE_FIGHTER_COMPILERMACROS_H

#ifdef __GNUC__
#  define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#else
#  define UNUSED(x) UNUSED_ ## x
#endif

#ifdef __GNUC__
#  define UNUSED_FUNCTION(x) __attribute__((__unused__)) UNUSED_ ## x
#else
#  define UNUSED_FUNCTION(x) UNUSED_ ## x
#endif

#define opengl_check_error(logger, msg) \
 { \
    GLenum error = glGetError(); \
    if(error != GL_NO_ERROR) { \
        (logger)->error("[{}] OpenGL Error Code: {}", msg, error); \
        /*__builtin_trap();*/ \
    } \
}; \


#endif //STAGE_FIGHTER_COMPILERMACROS_H
