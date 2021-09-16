//
// Created by x on 2021/8/30.
//

#include "xlog.hpp"

#include <spdlog/fmt/ostr.h> // must be included
#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/android_sink.h>
#include <spdlog/sinks/stdout_sinks.h>

namespace xlab::base {

std::shared_ptr<spdlog::logger> XLog::get() {
    std::lock_guard<std::mutex> Locker(LoggerLock);
    std::shared_ptr<spdlog::logger> log = spdlog::get(ConsoleLoggerName);
    if (log == nullptr) {
        log = CreateConsoleLog(ConsoleLoggerName);
    }
    return log;
}

std::shared_ptr<spdlog::logger> XLog::get(std::string logger_name, std::string path) {
    std::lock_guard<std::mutex> Locker(LoggerLock);
    std::shared_ptr<spdlog::logger> log = spdlog::get(logger_name);
    if (log == nullptr) {
        log = CreateTextLog(logger_name, path);
    }
    return log;
}

std::shared_ptr<spdlog::logger> XLog::CreateConsoleLog(std::string logger_name) {
#ifdef ANDROID
    auto console_logger = spdlog::android_logger_mt(logger_name, "sdk");
    console_logger->set_pattern("%v");
    console_logger->set_level(LevelValue);
    console_logger->flush_on(LevelValue);
    return console_logger;
#else
    auto console_logger = spdlog::stdout_logger_mt(logger_name);
    console_logger->set_pattern("[%L] [%H:%M:%S.%e] [thread %t] %v");
    console_logger->set_level(LevelValue);
    console_logger->flush_on(LevelValue);
    return console_logger;
#endif
};

///logger_name 不能重名
std::shared_ptr<spdlog::logger> XLog::CreateTextLog(std::string logger_name, std::string path) {
    spdlog::init_thread_pool(8192, 1); // 队列有 8k 个项目和 1 个支持线程。
    auto async_file = spdlog::daily_logger_mt<spdlog::async_factory>(logger_name, path, 2, 30);
    async_file->set_pattern("[%L] [%H:%M:%S.%e] [thread %t] %v");
    async_file->set_level(LevelValue);
    async_file->flush_on(LevelValue);
    return async_file;
}


}
