set(INCROOT "${PROJECT_SOURCE_DIR}/include/tmb_logs")
set(SRCROOT "${PROJECT_SOURCE_DIR}/src/tmb_logs")

set(SRC
    ${SRCROOT}/logging.cpp
    ${SRCROOT}/exception.cpp

    ${INCROOT}/logging.h
    ${INCROOT}/exception.h
    ${INCROOT}/colors.h
    ${INCROOT}/string_builder.h
)

if (NOT "${SRC}" STREQUAL "")
    message(STATUS "Building tmb_logs lib...")
    add_library(tmb_logs ${SRC})
    target_link_libraries(tmb_logs PUBLIC fmt termcolor)
    target_include_directories(tmb_logs PUBLIC ${PROJECT_SOURCE_DIR}/include)
    set_target_properties(tmb_logs PROPERTIES LINKER_LANGUAGE CXX)
else()
    message(WARNING "Tmb tools lib was not built")
endif()
