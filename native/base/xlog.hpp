//
// Created by x on 2021/8/24.
//

#pragma once

#include <mutex>
#include <spdlog/spdlog.h>

namespace xlab::base
{
    class XLog final
    {
        static inline std::mutex LoggerLock;
        static inline const char *ConsoleLoggerName = "console";
        static inline const char *TextLoggerName = "text";
        static inline std::string TextLoggerPath = "/sdcard/XLive/log/sdk/sdk.log";
        static inline spdlog::level::level_enum LevelValue = spdlog::level::level_enum::info;

    public:
        using ELevel = spdlog::level::level_enum;
        using logger = spdlog::logger;

        ~XLog()
        {
            //spdlog::shutdown();
        }

        static void setTextPath(std::string path)
        {
            TextLoggerPath = path;
        }

        static XLog &getInstance()
        {
            static XLog inst;
            return inst;
        }

        static const std::shared_ptr<spdlog::logger> console()
        {
            static auto console = xlab::base::XLog::get();
            return console;
        }

        static const std::shared_ptr<spdlog::logger> text()
        {
            static auto text = xlab::base::XLog::get(TextLoggerName, TextLoggerPath);
            return text;
        }

        template <typename... Args>
        void log(spdlog::level::level_enum lvl, fmt::format_string<Args...> fmt, Args &&...args)
        {
            console()->log(lvl, fmt, std::forward<Args>(args)...);
            text()->log(lvl, fmt, std::forward<Args>(args)...);
        }

    private:
        XLog() = default;

        XLog(const XLog &) = delete;

        XLog &operator=(const XLog &) = delete;

        static std::shared_ptr<spdlog::logger> get();

        static std::shared_ptr<spdlog::logger> get(std::string logger_name, std::string path);

        static std::shared_ptr<spdlog::logger> CreateConsoleLog(std::string logger_name);

        ///logger_name 不能重名
        static std::shared_ptr<spdlog::logger> CreateTextLog(std::string logger_name, std::string path);
    };

}

///打印指针用fmt::ptr()转换
#ifndef __FILE_NAME__
#include <libgen.h>
#define __FILE_NAME__ (basename((char*)(__FILE__)))
#endif

#ifndef xlog
#define xlog xlab::base::XLog::getInstance()
#endif

#ifndef XLevel
#define XLevel xlab::base::XLog::ELevel
#endif

#ifndef xlogt
#define xlogt(fmt, ...) xlab::base::XLog::getInstance().log(spdlog::level::trace, "[{}:{}] [{}] : " fmt, __FILE_NAME__, __LINE__, __PRETTY_FUNCTION__, ##__VA_ARGS__)
#endif

#ifndef xlogd
#define xlogd(fmt, ...) xlab::base::XLog::getInstance().log(spdlog::level::debug, "[{}:{}] [{}] : " fmt, __FILE_NAME__, __LINE__, __PRETTY_FUNCTION__, ##__VA_ARGS__)
#endif

#ifndef xlogi
#define xlogi(fmt, ...) xlab::base::XLog::getInstance().log(spdlog::level::info, "[{}:{}] [{}] : " fmt, __FILE_NAME__, __LINE__, __PRETTY_FUNCTION__, ##__VA_ARGS__)
#endif

#ifndef xlogw
#define xlogw(fmt, ...) xlab::base::XLog::getInstance().log(spdlog::level::warn, "[{}:{}] [{}] : " fmt, __FILE_NAME__, __LINE__, __PRETTY_FUNCTION__, ##__VA_ARGS__)
#endif

#ifndef xloge
#define xloge(fmt, ...) xlab::base::XLog::getInstance().log(spdlog::level::err, "[{}:{}] [{}] : " fmt, __FILE_NAME__, __LINE__, __PRETTY_FUNCTION__, ##__VA_ARGS__)
#endif

#ifndef xlogc
#define xlogc(fmt, ...) xlab::base::XLog::getInstance().log(spdlog::level::critical, "[{}:{}] [{}] : " fmt, __FILE_NAME__, __LINE__, __PRETTY_FUNCTION__, ##__VA_ARGS__)
#endif
