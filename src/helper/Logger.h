//
// Created by raphael on 06.03.18.
//

#ifndef LEARNOPENGL_LOGGER_H
#define LEARNOPENGL_LOGGER_H

#include <spdlog/spdlog.h>

namespace spd = spdlog;

class Logger {

protected:
    std::shared_ptr<spdlog::logger> console = spd::get("console");

};


#endif //LEARNOPENGL_LOGGER_H
