#pragma once

#include <tmb_logs/string_builder.h>

#include <stdio.h>
#include <ostream>
#include <iostream>

// Detect target's platform and set some macros in order to wrap platform
// specific code this library depends on.
#if defined(_WIN32) || defined(_WIN64)
#   define TERMCOLOR_TARGET_WINDOWS
#elif defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
#   define TERMCOLOR_TARGET_POSIX
#endif

// If implementation has not been explicitly set, try to choose one based on
// target platform.
#if !defined(TERMCOLOR_USE_ANSI_ESCAPE_SEQUENCES) && !defined(TERMCOLOR_USE_WINDOWS_API) && !defined(TERMCOLOR_USE_NOOP)
#   if defined(TERMCOLOR_TARGET_POSIX)
#       define TERMCOLOR_USE_ANSI_ESCAPE_SEQUENCES
#       define TERMCOLOR_AUTODETECTED_IMPLEMENTATION
#   elif defined(TERMCOLOR_TARGET_WINDOWS)
#       define TERMCOLOR_USE_WINDOWS_API
#       define TERMCOLOR_AUTODETECTED_IMPLEMENTATION
#   endif
#endif

// These headers provide isatty()/fileno() functions, which are used for
// testing whether a standard stream refers to the terminal.
#if defined(TERMCOLOR_TARGET_POSIX)
#   include <unistd.h>
#elif defined(TERMCOLOR_TARGET_WINDOWS)
#   include <io.h>
#   include <windows.h>
#endif

namespace NColors {

#if defined(TERMCOLOR_TARGET_POSIX)

template <typename TChar>
size_t EscapeSymbolsCount(const std::basic_string<TChar>& string) {
    size_t count = 0;
    size_t pos = string.find('\033');

    while (pos != string.npos) {
        count += string.find('m', pos + 1) - pos + 1;
        pos = string.find('\033', pos + 1);
    }

    return count;
}

template <typename TChar>
std::basic_string<TChar> EraseEscapeSymbols(const std::basic_string<TChar>& string) {
    std::basic_string<TChar> result;
    result.reserve(string.size() - EscapeSymbolsCount(string));
    size_t prev = 0;
    size_t pos = string.find('\033');

    while (pos != string.npos) {
        result.insert(result.end(), string.data() + prev, string.data() + pos);
        prev = string.find('m', pos) + 1;
        pos = string.find('\033', prev);
    }

    result.insert(result.end(), std::next(string.begin(), prev), string.end());

    return result;
}

#endif

template <typename TChar>
struct TColorMode {
    std::basic_string<TChar> code;

    operator const std::basic_string<TChar>&() {
        return code;
    }
};

template <typename TChar>
std::basic_ostream<TChar>& operator<<(std::basic_ostream<TChar>& os, const TColorMode<TChar>& colorMode) {
    #if defined(TERMCOLOR_USE_ANSI_ESCAPE_SEQUENCES)
        os << colorMode.code;
    // #elif defined(TERMCOLOR_USE_WINDOWS_API)
    #endif
    return os;
}

template <typename TChar>
TColorMode<TChar> operator+(const TColorMode<TChar>& lhs, const TColorMode<TChar>& rhs) {
    return TColorMode<TChar>{ lhs.code + rhs.code };
}

template <typename TChar>
bool IsColorized(const std::basic_ostream<TChar>& os) {
    if (&os == &std::cout) {
        return isatty(STDOUT_FILENO);
    }

    if (&os == &std::cerr) {
        return isatty(STDERR_FILENO);
    }

    return false;
}

template <typename TChar>
class TColors {
 public:
    TColors()
        : isColorized(true)
    {}

    TColors(const std::basic_ostream<TChar>& stream)
        : isColorized(IsColorized(stream))
    {}

	std::basic_ostream<TChar> Colorize(std::basic_ostream<TChar>& stream)
    {
        stream.iword(TColors::colorize_index()) = 1L;
        return stream;
    }

	std::basic_ostream<TChar> NoColorize(std::basic_ostream<TChar>& stream)
    {
        stream.iword(TColors::colorize_index()) = 0L;
        return stream;
    }

    TColorMode<TChar> Reset() {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[00m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> Bold() {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[1m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> Dark() {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[2m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> Italic() {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[3m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> Underline() {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[4m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> Blink() {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[5m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> Reverse() {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[7m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> Concealed() {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[8m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> Crossed() {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[9m" };
        }
        return TColorMode<TChar>{""};
    }
    
    TColorMode<TChar> Color(uint32_t code) {
        if (isColorized) {
            return TColorMode<TChar>{ TStringBuilder() << "\033[38;5;" << code << "m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> OnColor(uint8_t code) {
        if (isColorized) {
            return TColorMode<TChar>{ TStringBuilder() << "\033[48;5;" << +code << "m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> Color(uint8_t r, uint8_t g, uint8_t b) {
        if (isColorized) {
            return TColorMode<TChar>{ TStringBuilder() << "\033[38;2;" << +r << ";" << +g << ";" << +b << "m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> OnColor(uint8_t r, uint8_t g, uint8_t b) {
        if (isColorized) {
            return TColorMode<TChar>{ TStringBuilder() << "\033[48;2;" << +r << ";" << +g << ";" << +b << "m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> Grey() {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[30m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> Red() {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[31m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> Green() {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[32m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> Yellow() {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[33m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> Blue() {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[34m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> Magenta() {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[35m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> Cyan() {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[36m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> White() {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[37m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> BrightGrey() {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[90m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> BrightRed() {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[91m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> BrightGreen() {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[92m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> BrightYellow() {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[93m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> BrightBlue() {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[94m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> BrightMagenta() {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[95m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> BrightCyan() {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[96m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> BrightWhite() {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[97m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> OnGrey() {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[40m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> OnRed() {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[41m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> OnGreen() {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[42m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> OnYellow() {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[43m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> OnBlue() {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[44m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> OnMagenta() {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[45m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> OnCyan() {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[46m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> OnWhite() {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[47m" };
        }
        return TColorMode<TChar>{""};
    }


	TColorMode<TChar> OnBrightGrey()
    {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[100m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> OnBrightRed() {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[101m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> OnBrightGreen() {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[102m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> OnBrightYellow() {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[103m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> OnBrightBlue() {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[104m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> OnBrightMagenta() {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[105m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> OnBrightCyan() {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[106m" };
        }
        return TColorMode<TChar>{""};
    }

	TColorMode<TChar> OnBrightWhite() {
        if (isColorized) {
            return TColorMode<TChar>{ "\033[107m" };
        }
        return TColorMode<TChar>{""};
    }

    void Enable();

    void Disable();

 private:
    bool isColorized;

    // an index to be used to access a private storage of i/o streams. see
    // colorize / nocolorize i/o manipulators for details. due to the fact
    // that static variables ain't shared between translation units, inline
    // function with local static variable is used to do the trick and share
    // the variable value between translation units.
    inline int colorize_index()
    {
        static int colorize_index = std::ios_base::xalloc();
        return colorize_index;
    }

#if defined(termcolor_target_windows)

    //! same hack as used in get_standard_stream function, but for windows with `std::ostream`
    inline handle get_terminal_handle(std::ostream& stream)
    {
        if (&stream == &std::cout)
            return getstdhandle(std_output_handle);
        else if (&stream == &std::cerr || &stream == &std::clog)
            return getstdhandle(std_error_handle);
        return nullptr;
    }
    
    //! same hack as used in get_standard_stream function, but for Windows with `std::wostream`
    inline HANDLE get_terminal_handle(std::wostream& stream)
    {
        if (&stream == &std::wcout)
            return GetStdHandle(STD_OUTPUT_HANDLE);
        else if (&stream == &std::wcerr || &stream == &std::wclog)
            return GetStdHandle(STD_ERROR_HANDLE);
        return nullptr;
    }
    
    //! Change Windows Terminal colors attribute. If some
    //! parameter is `-1` then attribute won't changed.
    template <typename TChar>
    void win_change_attributes(std::basic_ostream<TChar>& stream, int foreground, int background)
    {
        // yeah, i know.. it's ugly, it's windows.
        static WORD defaultAttributes = 0;

        // Windows doesn't have ANSI escape sequences and so we use special
        // API to change Terminal output color. That means we can't
        // manipulate colors by means of "std::stringstream" and hence
        // should do nothing in this case.
        if (!_internal::is_atty(stream))
            return;

        // get terminal handle
        HANDLE hTerminal = INVALID_HANDLE_VALUE;
        hTerminal = get_terminal_handle(stream);

        // save default terminal attributes if it unsaved
        if (!defaultAttributes)
        {
            CONSOLE_SCREEN_BUFFER_INFO info;
            if (!GetConsoleScreenBufferInfo(hTerminal, &info))
                return;
            defaultAttributes = info.wAttributes;
        }

        // restore all default settings
        if (foreground == -1 && background == -1)
        {
            SetConsoleTextAttribute(hTerminal, defaultAttributes);
            return;
        }

        // get current settings
        CONSOLE_SCREEN_BUFFER_INFO info;
        if (!GetConsoleScreenBufferInfo(hTerminal, &info))
            return;

        if (foreground != -1)
        {
            info.wAttributes &= ~(info.wAttributes & 0x0F);
            info.wAttributes |= static_cast<WORD>(foreground);
        }

        if (background != -1)
        {
            info.wAttributes &= ~(info.wAttributes & 0xF0);
            info.wAttributes |= static_cast<WORD>(background);
        }

        SetConsoleTextAttribute(hTerminal, info.wAttributes);
    }
#endif // TERMCOLOR_TARGET_WINDOWS

}; // class TColors

#undef TERMCOLOR_TARGET_POSIX
#undef TERMCOLOR_TARGET_WINDOWS

#if defined(TERMCOLOR_AUTODETECTED_IMPLEMENTATION)
#   undef TERMCOLOR_USE_ANSI_ESCAPE_SEQUENCES
#   undef TERMCOLOR_USE_WINDOWS_API
#endif

} // namespace NColors
