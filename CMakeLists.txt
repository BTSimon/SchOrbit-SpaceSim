
cmake_minimum_required(VERSION 3.31)
project(orbit LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# SDL3
find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)

add_executable(orbit
    src/main.c
    src/app/game_state.c
    src/app/game.c
    src/app/event_handler.c

    src/platform/window.c
    src/platform/assets.c
    src/render/graphics.c
    
    src/simulation/physics.c
    src/simulation/components.c

    src/ui/ui.c
)

target_link_libraries(orbit PRIVATE 
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf
)

install(TARGETS orbit
    RUNTIME DESTINATION .
)

install(CODE [[
    cmake_policy(SET CMP0207 NEW)

    set(_executable "${CMAKE_INSTALL_PREFIX}/orbit.exe")

    file(GET_RUNTIME_DEPENDENCIES
        EXECUTABLES "${_executable}"
        DIRECTORIES "C:/msys64/ucrt64/bin"
        RESOLVED_DEPENDENCIES_VAR _resolved_dependencies
        UNRESOLVED_DEPENDENCIES_VAR _unresolved_dependencies
        PRE_EXCLUDE_REGEXES
            "api-ms-win-.*"
            "ext-ms-win-.*"
        POST_EXCLUDE_REGEXES
            ".*[Ww][Ii][Nn][Dd][Oo][Ww][Ss].*"
    )

    foreach(_dependency IN LISTS _resolved_dependencies)
        message(STATUS "Deploying DLL: ${_dependency}")
        file(INSTALL
            DESTINATION "${CMAKE_INSTALL_PREFIX}"
            TYPE SHARED_LIBRARY
            FILES "${_dependency}"
        )
    endforeach()

    if(_unresolved_dependencies)
        message(WARNING
            "Unresolved runtime dependencies: ${_unresolved_dependencies}"
        )
    endif()
]])
