#include <tmb_logs/logging.h>
#include <tmb_logs/exception.h>
#include <tmb_logs/colors.h>

#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>

#include <fmt/chrono.h>

namespace NLogging {

////////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

auto Logger = NLogging::TLogger{"Logger"};

} // namespace 

////////////////////////////////////////////////////////////////////////////////////////////////////

TLoggerPipes::TLoggerPipes() = default;

TLoggerPipes::~TLoggerPipes() = default;

inline TLoggerPipes* TLoggerPipes::GetInstance() {
    if (!Instance_) {
        Instance_ = new TLoggerPipes();
    }

    return Instance_;
}

void TLoggerPipes::InitPipe(std::ostream* output, const std::vector<TFilter>& filters) {
    auto& pipe = OutputPipes_.emplace_back();
    pipe.OutputStream_ = output;
    for (const auto& filter : filters) {
        std::vector<std::string> sources;
        if (filter.sources.size() == 0) {
            sources = {""};
        } else {
            sources = std::vector<std::string>(filter.sources);
        }
        
        for (const auto& source : sources) {
            if (pipe.Filter_.contains(source) && pipe.Filter_[source].empty()) {
                continue;
            }

            pipe.Filter_[source];
            for (const auto& level : filter.levels) {
                pipe.Filter_[source].emplace(level);
            }
        }
    }
}

void TLoggerPipes::InitFilePipe(const std::string& path, const std::vector<TFilter>& filters) {
    auto guard = std::lock_guard(Mutex_);

    std::filesystem::path fpath = path;
    std::filesystem::create_directories(fpath.parent_path());

    THROW_ERROR_UNLESS(
        std::filesystem::exists(fpath.parent_path()),
        "Failed to create log directory (Path: %v)",
        std::string(std::filesystem::absolute(fpath)));

    auto ptr = std::make_shared<std::fstream>(path, std::ios::app);
    OutputStreams_.push_back(ptr);
    InitPipe(ptr.get(), filters);
}

void TLoggerPipes::InitStdout(const std::vector<TFilter>& filters) {
    auto guard = std::lock_guard(Mutex_);
    InitPipe(&std::cout, filters);
}

void TLoggerPipes::InitStderr(const std::vector<TFilter>& filters) {
    auto guard = std::lock_guard(Mutex_);
    InitPipe(&std::cerr, filters);
}

void TLoggerPipes::SetLevelStyle(const std::string& level, const std::string& style) {
    LevelToStyle_[level] = style;
}

void TLoggerPipes::Print(
    const std::string& message,
    const std::string& source,
    const std::string& level)
{

    auto m = fmt::format(
        "{:%F %T}\t[{}{}\033[0m]\t{}\t{}",
        fmt::localtime(std::time(nullptr)),
        LevelToStyle_[level],
        level,
        source,
        message);

    std::optional<std::string> uncoloredMessage;
    auto guard = std::lock_guard(Mutex_);
    for (auto& pipe : OutputPipes_) {
        bool is_filtered = false;
        is_filtered |= pipe.Filter_.contains("") && (pipe.Filter_.at("").empty() || pipe.Filter_.at("").contains(level));
        is_filtered |= pipe.Filter_.contains(source) && (pipe.Filter_.at(source).empty() || pipe.Filter_.at(source).contains(level));
        if (!is_filtered) {
            continue;
        }

        if (NColors::IsColorized(*pipe.OutputStream_)) {
            *pipe.OutputStream_ << m << std::endl;
        } else {
            if (!uncoloredMessage) {
                uncoloredMessage = NColors::EraseEscapeSymbols(m);
            }
            *pipe.OutputStream_ << *uncoloredMessage << std::endl;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

TLogger::TLogger(const std::string& source) 
    : Source_(source)
{}

void TLogger::Print(const std::string& level, const std::string& message) const {
    auto* loggerPipes = TLoggerPipes::GetInstance();
    loggerPipes->Print(message, Source_, level);
}

} // namespace NLogging
