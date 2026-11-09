# Bundled Windows GCC dependencies

These static libraries and public headers are bundled to build SchOrbit SpaceSim on 64-bit Windows with MinGW-w64 GCC and the UCRT runtime. SDL3 and SDL3_ttf were taken from the MSYS2 UCRT64 package set available when this bundle was prepared (SDL3 3.4.18 and SDL3_ttf 3.2.2). Their static dependencies are included in `lib/`; corresponding license texts are in `licenses/`.

The bundled archive files are target-specific binaries. They are not interchangeable with Cygwin, 32-bit MinGW, or MinGW-w64 builds using the legacy MSVCRT runtime. Use an x86_64 MinGW-w64 UCRT GCC toolchain.

The direct build is `build.bat` in the project root. It needs `gcc` on `PATH`; CMake is optional. The link uses static dependency archives, while Windows API libraries and the UCRT are supplied by the target Windows installation/toolchain.

Upstream projects:

- SDL3: https://github.com/libsdl-org/SDL
- SDL_ttf: https://github.com/libsdl-org/SDL_ttf
- FreeType: https://gitlab.freedesktop.org/freetype/freetype
- HarfBuzz: https://github.com/harfbuzz/harfbuzz
- GLib: https://gitlab.gnome.org/GNOME/glib
- libpng: http://www.libpng.org/pub/png/libpng.html
- zlib: https://zlib.net/
- Brotli: https://github.com/google/brotli
- Graphite: https://github.com/silnrsi/graphite
- PCRE2: https://github.com/PCRE2Project/pcre2
- bzip2: https://sourceware.org/bzip2/
- GNU gettext and libiconv: https://www.gnu.org/software/gettext/
