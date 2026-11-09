cmake_minimum_required(VERSION 3.21)
project(SchOrbitSpaceSim VERSION 0.1.0 LANGUAGES C)

include(GNUInstallDirs)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if(NOT MINGW OR NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "This bundled dependency set supports 64-bit Windows MinGW GCC (UCRT).")
endif()

set(ORBIT_VENDOR_DIR "${PROJECT_SOURCE_DIR}/third_party/mingw64-ucrt")
set(ORBIT_VENDOR_INCLUDE_DIR "${ORBIT_VENDOR_DIR}/include")
set(ORBIT_VENDOR_LIBRARY_DIR "${ORBIT_VENDOR_DIR}/lib")

function(add_orbit_object_module target)
    add_library(${target} OBJECT ${ARGN})
    target_include_directories(${target} PRIVATE
        "${PROJECT_SOURCE_DIR}/headers"
        "${ORBIT_VENDOR_INCLUDE_DIR}"
    )

    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

# Keep major responsibilities in separate build modules. Object libraries
# avoid static-link ordering problems while preserving a single portable app.
add_orbit_object_module(orbit_simulation
    src/simulation/physics.c
    src/simulation/components.c
)

add_orbit_object_module(orbit_state
    src/app/game_state.c
)

add_orbit_object_module(orbit_ui
    src/ui/ui.c
)

add_orbit_object_module(orbit_render
    src/platform/assets.c
    src/render/graphics.c
    src/render/design_window.c
    src/render/test_window.c
    src/render/statistics_window.c
)

add_orbit_object_module(orbit_platform
    src/platform/window.c
)

add_orbit_object_module(orbit_application
    src/app/game.c
    src/app/event_handler.c
)

add_executable(orbit
    src/main.c
    $<TARGET_OBJECTS:orbit_simulation>
    $<TARGET_OBJECTS:orbit_state>
    $<TARGET_OBJECTS:orbit_ui>
    $<TARGET_OBJECTS:orbit_render>
    $<TARGET_OBJECTS:orbit_platform>
    $<TARGET_OBJECTS:orbit_application>
)
target_link_libraries(orbit PRIVATE
    "${ORBIT_VENDOR_LIBRARY_DIR}/libSDL3_ttf.a"
    "${ORBIT_VENDOR_LIBRARY_DIR}/libSDL3.a"
    m iconv
    kernel32 user32 gdi32 winmm imm32 ole32 oleaut32 version uuid
    advapi32 setupapi shell32 dinput8
    freetype bz2 png16 m z harfbuzz m usp10 gdi32 rpcrt4 user32 dwrite
    glib-2.0 intl ws2_32 ole32 winmm shlwapi uuid atomic m pcre2-8
    graphite2 brotlidec brotlicommon stdc++
)
target_link_directories(orbit PRIVATE "${ORBIT_VENDOR_LIBRARY_DIR}")
target_link_options(orbit PRIVATE -static)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(orbit PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Keep runtime assets beside the executable so launching from another working
# directory does not break font loading.
add_custom_command(TARGET orbit POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
        "${PROJECT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:orbit>/assets"
    VERBATIM
)

install(TARGETS orbit
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)
install(DIRECTORY assets/
    DESTINATION "${CMAKE_INSTALL_BINDIR}/assets"
)
