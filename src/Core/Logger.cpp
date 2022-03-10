//
// Created by Sam on 2/16/2022.
//

#include "Core/Logger.hpp"

namespace Elys {
    std::shared_ptr<spdlog::logger> Log::sCoreLogger;
    std::shared_ptr<spdlog::logger> Log::sClientLogger;

    void Log::Init() {
        spdlog::set_pattern("%^[%T] %n::%! %l: %v%$");
        sCoreLogger = spdlog::stdout_color_mt("ELYS");
        sCoreLogger->set_level(spdlog::level::trace);

        sClientLogger = spdlog::stdout_color_mt("APP");
        sClientLogger->set_level(spdlog::level::trace);
    }
}