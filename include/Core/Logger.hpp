//
// Created by Sam on 2/16/2022.
//

#ifndef ELYS_LOGGER_HPP
#define ELYS_LOGGER_HPP

#include <memory>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Elys {
    class Log {
      public:
        static void Init();

        inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return sCoreLogger; }
        inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return sClientLogger; }

      private:
        static std::shared_ptr<spdlog::logger> sCoreLogger;
        static std::shared_ptr<spdlog::logger> sClientLogger;
    };
} // namespace Elys

#define DEBUG 1

#if DEBUG
#define ELYS_CORE_TRACE(...)    SPDLOG_LOGGER_TRACE(::Elys::Log::GetCoreLogger(), __VA_ARGS__)
#define ELYS_CORE_INFO(...)     ::Elys::Log::GetCoreLogger()->info(__VA_ARGS__)
#else
#define ELYS_CORE_TRACE(...)
#define ELYS_CORE_INFO(...)
#endif

#define ELYS_CORE_WARN(...)     ::Elys::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define ELYS_CORE_ERROR(...)    ::Elys::Log::GetCoreLogger()->error(__VA_ARGS__)
#define ELYS_CORE_FATAL(...)    ::Elys::Log::GetCoreLogger()->critical(__VA_ARGS__)

#endif // ELYS_LOGGER_HPP
