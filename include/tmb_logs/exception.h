#pragma once

#include <tmb_logs/logging.h>


namespace NException {

////////////////////////////////////////////////////////////////////////////////////////////////////

class TError {
 public:
    TError();

    TError(const std::string& message);

    TError(uint32_t code, const std::string& message);

    uint32_t Code() const;

    const std::string& Message() const;

 private:
    uint32_t Code_;
    std::string Message_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class TErrorException
    : public TError, public std::exception
{
 public:
    using TError::TError;

    const uint32_t code() const noexcept;

    const char* what() const noexcept override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
    
#define THROW_ERROR(...) LOG_ERROR(__VA_ARGS__); throw NException::TErrorException(fmt::format(__VA_ARGS__))

#define THROW_ERROR_IF(cond, ...) if (cond) { THROW_ERROR(__VA_ARGS__); }

#define THROW_ERROR_UNLESS(cond, ...) THROW_ERROR_IF(!cond, __VA_ARGS__);

////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace NException
