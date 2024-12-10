#pragma once

#include <tmb_logs/colors.h>

#include <fmt/core.h>

#include <mutex>
#include <ostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>


namespace NLogging {

////////////////////////////////////////////////////////////////////////////////////////////////////

class TLoggerPipes {
 public:
    struct TFilter {
        std::initializer_list<std::string> sources;
        std::initializer_list<std::string> levels;
    };

    static TLoggerPipes* GetInstance();

    void InitFilePipe(const std::string& path, const std::vector<TFilter>& filters);

    void InitStdout(const std::vector<TFilter>& filters);

    void InitStderr(const std::vector<TFilter>& filters);

    void SetLevelStyle(const std::string& level, const std::string& style);

    void Print(
        const std::string& message,
        const std::string& source,
        const std::string& level);

 private:
    void InitPipe(std::ostream* output, const std::vector<TFilter>& filters);

    struct TOutputPipe_ {
        struct All {};

        std::unordered_map<std::string, std::unordered_set<std::string>> Filter_;
        std::ostream* OutputStream_;
    };

    TLoggerPipes();
    ~TLoggerPipes();

    static TLoggerPipes* Instance_;

    std::vector<TOutputPipe_> OutputPipes_;
    std::vector<std::shared_ptr<std::ostream>> OutputStreams_;

    std::unordered_map<std::string, std::string> LevelToStyle_;
    std::mutex Mutex_;
};

inline TLoggerPipes* TLoggerPipes::Instance_ = nullptr;

////////////////////////////////////////////////////////////////////////////////////////////////////

class TLogger {
 public:
    TLogger(const std::string& source);

    void Print(
        const std::string& level,
        const std::string& message) const;

 private:
    std::string Source_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#define LOG_EVENT(logger, level, ...) logger.Print(level, fmt::format(__VA_ARGS__))

#define LOG_INFO(...) LOG_EVENT(Logger, "INFO", __VA_ARGS__)

#define LOG_DEBUG(...) LOG_EVENT(Logger, "DEBUG", __VA_ARGS__)

#define LOG_WARNING(...) LOG_EVENT(Logger, "WARNING", __VA_ARGS__)

#define LOG_ERROR(...) LOG_EVENT(Logger, "ERROR", __VA_ARGS__)

////////////////////////////////////////////////////////////////////////////////////////////////////

struct DebugTag {};

template <typename TChar>
std::basic_ostream<TChar>& operator<<(std::basic_ostream<TChar>& os, DebugTag) {
    NColors::TColors colors(os);
    return os << '[' << colors.Green() << "DEBUG" << colors.Reset() << "] ";
}

struct InfoTag {};

template <typename TChar>
std::basic_ostream<TChar>& operator<<(std::basic_ostream<TChar>& os, InfoTag) {
    NColors::TColors colors(os);
    return os << '[' << colors.Cyan() << "INFO" << colors.Reset() << "] ";
}

struct WarningTag {};

template <typename TChar>
std::basic_ostream<TChar>& operator<<(std::basic_ostream<TChar>& os, WarningTag) {
    NColors::TColors colors(os);
    return os << '[' << colors.Yellow() << colors.Bold() << "WARNING" << colors.Reset() << "] ";
}

struct ErrorTag {};

template <typename TChar>
std::basic_ostream<TChar>& operator<<(std::basic_ostream<TChar>& os, ErrorTag) {
    NColors::TColors colors(os);
    return os << '[' << colors.Red() << colors.Bold() << "ERROR" << colors.Reset() << "] ";
}

////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace NLogging
