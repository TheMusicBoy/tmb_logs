cmake_minimum_required(VERSION 3.15)
project(TmbLogs)

# Export compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Build settings
set(CMAKE_CXX_STANDARD 20)

option(DEBUG "" OFF)

if (${DEBUG})
    message(STATUS "Building with debug...")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

# Building ---
add_subdirectory(thirdparty)
add_subdirectory(src)

option(BUILD_TESTS "" OFF)

option(TMP_LOGS_COMPILE_COMMANDS "" OFF)

if (${TMP_LOGS_COMPILE_COMMANDS})
    add_custom_target(tmb_logs_built_finished ALL
        COMMENT "Tmb logs build almost finished..."
    )
    add_custom_command(TARGET tmb_logs_built_finished POST_BUILD
        COMMAND cp ${PROJECT_BINARY_DIR}/compile_commands.json ${PROJECT_SOURCE_DIR}/
    )
    message("Compile commands exported...")
endif()
