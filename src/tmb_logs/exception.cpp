#include <tmb_logs/exception.h>

namespace NException {

////////////////////////////////////////////////////////////////////////////////////////////////////

TError::TError(uint32_t code, const std::string& message)
    : Code_(code), Message_(message)
{}

TError::TError(const std::string& message)
    : Code_(0), Message_(message)
{}

TError::TError()
    : TError("")
{}

uint32_t TError::Code() const {
    return Code_;
}

const std::string& TError::Message() const {
    return Message_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

const uint32_t TErrorException::code() const noexcept {
    return Code();
}

const char* TErrorException::what() const noexcept {
    return Message().c_str();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace NException
