//
// Created by raphael on 06.03.18.
//

#ifndef LEARNOPENGL_LOGGER_H
#define LEARNOPENGL_LOGGER_H

#include <spdlog/spdlog.h>

namespace spd = spdlog;

class Logger {

protected:
    std::shared_ptr<spdlog::logger> logger;

    Logger(const std::string &name = std::string("console")) {
        this->logger = spd::get(name);
        if (logger == nullptr) {
            this->logger = spdlog::stdout_color_mt(name);
        }

        this->logger->flush_on(spd::level::err);
    }

};


#endif //LEARNOPENGL_LOGGER_H
