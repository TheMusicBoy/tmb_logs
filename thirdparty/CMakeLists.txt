include(FetchContent)

# -- Google Test --

FetchContent_Declare(googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip)

FetchContent_MakeAvailable(googletest)
FetchContent_GetProperties(googletest)

if (NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif()

# -- Fmt --

# add_subdirectory(fmt)

FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG master
)

FetchContent_MakeAvailable(fmt)
FetchContent_GetProperties(fmt)

if (NOT fmt_POPULATED)
    FetchContent_Populate(fmt)
    add_subdirectory(${fmt_SOURCE_DIR} ${fmt_BINARY_DIR})
endif()

# -- Term color --

# add_subdirectory(termcolor)

FetchContent_Declare(termcolor
  GIT_REPOSITORY https://github.com/ikalnytskyi/termcolor.git
  GIT_TAG master
)

FetchContent_MakeAvailable(termcolor)
FetchContent_GetProperties(termcolor)

if (NOT termcolor_POPULATED)
    FetchContent_Populate(termcolor)
    add_subdirectory(${termcolor_SOURCE_DIR} ${termcolor_BINARY_DIR})
endif()
