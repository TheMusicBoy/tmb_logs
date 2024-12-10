#pragma once

#include <string>
#include <sstream>

template <typename TChar>
class TBasicStringBuilder
    : public std::basic_ostringstream<TChar>
{
 public:
    using std::basic_ostringstream<TChar>::basic_ostringstream;

    operator std::basic_string<TChar>() const {
        return this->str();
    }
};

using TStringBuilder = TBasicStringBuilder<char>;
