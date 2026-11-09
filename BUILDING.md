#Build

Buildelés gcc-vel lehetséges, ha a rendszer környezeti változói között szerepel egy valid gcc útvonal.
Prefferáltan MinGW-Ucrt gcc installal.

A build.bat fájl az alábbi szkriptet tartalmazza a build leegyszerűsítése érdekében

```
@echo off
setlocal
cd /d "%~dp0"

if not exist build mkdir build

gcc -Wall -Werror^
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
    -lfreetype -lbz2 -lpng16 -lm -lz -lharfbuzz -lm ^
    -lusp10 -lgdi32 -lrpcrt4 -luser32 -ldwrite -lglib-2.0 -lintl ^
    -lws2_32 -lole32 -lwinmm -lshlwapi -luuid -latomic -lm ^
    -lpcre2-8 -lgraphite2 -lbrotlidec -lbrotlicommon -lstdc++ ^
    -o build/orbit.exe

if errorlevel 1 exit /b 1

xcopy assets build\assets /E /I /Y >nul
if errorlevel 1 exit /b 1

echo Built build\orbit.exe
```

A szkript, illetve a MinGW-ből kiexportált SDL fájlok lehetővé teszik hogy a program minden függőségét hordozhatóan elégítsük ki. A program lefordul ugyan minden olyan környezetben, amely lehetővé teszi az SDL3, SDL3_ttf elérését, de a build ebben a környezetben lett tesztelve, így a hibamentes futás érdekében ez ajánlott.