@echo off
setlocal
cd /d "%~dp0"

if not exist build mkdir build

gcc -std=c17 -O2 -Wall -Werror -Wextra -Wpedantic ^
    -Iheaders ^
    -Ithird_party/mingw64-ucrt/include ^
    src/main.c ^
    src/app/game_state.c ^
    src/app/game.c ^
    src/app/event_handler.c ^
    src/platform/window.c ^
    src/platform/assets.c ^
    src/render/graphics.c ^
    src/render/design_window.c ^
    src/render/test_window.c ^
    src/render/statistics_window.c ^
    src/simulation/physics.c ^
    src/simulation/components.c ^
    src/ui/ui.c ^
    -Lthird_party/mingw64-ucrt/lib ^
    -static ^
    -lSDL3_ttf -lSDL3 ^
    -lm -liconv ^
    -lkernel32 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 ^
    -lversion -luuid -ladvapi32 -lsetupapi -lshell32 -ldinput8 ^
    -lfreetype -lbz2 -lpng16 -lm -lz -lharfbuzz -lm ^
    -lusp10 -lgdi32 -lrpcrt4 -luser32 -ldwrite -lglib-2.0 -lintl ^
    -lws2_32 -lole32 -lwinmm -lshlwapi -luuid -latomic -lm ^
    -lpcre2-8 -lgraphite2 -lbrotlidec -lbrotlicommon -lstdc++ ^
    -o build/orbit.exe

if errorlevel 1 exit /b 1

xcopy assets build\assets /E /I /Y >nul
if errorlevel 1 exit /b 1

echo Built build\orbit.exe
