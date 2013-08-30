#!/bin/sh

# This is the directory containing this script
SOURCE_ROOT=$(echo $PWD/$0 | sed 's:\(.\+/\)[^/]\+:\1:')../../

cd "$SOURCE_ROOT"

# Search the c++ compiler for Android
CXX_COMPILER=

if [ -z "$ANDROID_CXX" ]
then
    echo "ANDROID_CXX is not set. Could not find Android's C++ compiler." 1>&2
    exit 1;
else
    CXX_COMPILER="$ANDROID_CXX"
fi

export CXX="$CXX_COMPILER"

C_COMPILER=

if [ -z "$ANDROID_CC" ]
then
    echo "ANDROID_CC is not set. Could not find Android's C compiler." 1>&2
    exit 1;
else
    CC_COMPILER="$ANDROID_CC"
fi

export CC="$CC_COMPILER"

ANDROID_SYSROOT="$ANDROID_TOOLCHAIN_ROOT/sysroot"
ANDROID_INCLUDE="$ANDROID_SYSROOT/usr/include"
ANDROID_LIB="$ANDROID_SYSROOT/usr/lib"

[ -z "$ANDROID_GNU_INCLUDE_DIR" ] \
    && ANDROID_GNU_INCLUDE_DIR="$ANDROID_PROGRAM_FILES/GnuWin32/include"

[ -z "$ANDROID_GNU_BIN_DIR" ] \
    && ANDROID_GNU_BIN_DIR="$ANDROID_PROGRAM_FILES/GnuWin32/bin"

[ -z "$ANDROID_GNU_LIB_DIR" ] \
    && ANDROID_GNU_LIB_DIR="$ANDROID_PROGRAM_FILES/GnuWin32/lib"

[ -z "$ANDROID_BOOST_DIR" ] \
    && ANDROID_BOOST_DIR="$ANDROID_SYSROOT"

#[ -z "$ANDROID_SDL_DIR" ] \
#    && ANDROID_SDL_DIR="$ANDROID_SYSROOT"

[ -z "$ANDROID_SDL_INCLUDE_DIR" ] \
    && ANDROID_SDL_INCLUDE_DIR="$ANDROID_SYSROOT/include"

[ -z "$ANDROID_SDL_MIXER_INCLUDE_DIR" ] \
    && ANDROID_SDL_MIXER_INCLUDE_DIR="$ANDROID_SDL_DIR/SDL"

# Libraries paths
[ -z "$ANDROID_LIBCLAW_DIR" ] \
    && ANDROID_LIBCLAW_DIR="$ANDROID_SYSROOT/usr"

[ -z "$ANDROID_LIBCLAW_CMAKE_DIR" ] \
    && ANDROID_LIBCLAW_CMAKE_DIR="$ANDROID_SYSROOT/usr/share/cmake/libclaw/"

[ -z "$ANDROID_FREETYPE_INCLUDE_DIR" ] \
    && ANDROID_FREETYPE_INCLUDE_DIR="$ANDROID_INCLUDE/freetype2"

[ -z "$ANDROID_FREETYPE_LIBRARY" ] \
    && ANDROID_FREETYPE_LIBRARY="$ANDROID_LIB/libfreetype.a"

[ -z "$ANDROID_JPEG_INCLUDE_DIR" ] \
    && ANDROID_JPEG_INCLUDE_DIR="$ANDROID_INCLUDE"

[ -z "$ANDROID_JPEG_LIBRARY_PATH" ] \
    && ANDROID_JPEG_LIBRARY_PATH="$ANDROID_LIB/libjpeg.a"

[ -z "$ANDROID_PNG_INCLUDE_DIR" ] \
    && ANDROID_PNG_INCLUDE_DIR="$ANDROID_INCLUDE"

[ -z "$ANDROID_PNG_LIBRARY_PATH" ] \
    && ANDROID_PNG_LIBRARY_PATH="$ANDROID_LIB/libpng.a"

[ -z "$ANDROID_ZLIB_LIBRARY_PATH" ] \
    && ANDROID_ZLIB_LIBRARY_PATH="$ANDROID_LIB/libz.so"

[ -z "$ANDROID_BOOST_INCLUDE_DIR" ] \
    && ANDROID_BOOST_INCLUDE_DIR="$ANDROID_INCLUDE"

[ -z "$ANDROID_SDL_LIBRARY_PATH" ] \
    && ANDROID_SDL_LIBRARY_PATH="$ANDROID_LIB/libSDL2.so"

[ -z "$ANDROID_SDL_MIXER_LIBRARY_PATH" ] \
    && ANDROID_SDL_MIXER_LIBRARY_PATH="$ANDROID_LIB/libSDL2_mixer.so"

# Programs
[ -z "$GETTEXT_MSGMERGE_PATH" ] \
    && GETTEXT_MSGMERGE_PATH="msgmerge"

[ -z "$GETTEXT_MSGFMT_PATH" ] \
    && GETTEXT_MSGFMT_PATH="msgfmt"

# Compilation flags
[ -z "$BUILD_TYPE" ] && BUILD_TYPE=release

[ -z "$INSTALL_PREFIX" ] && INSTALL_PREFIX=/tmp/install/android

[ -z "$BEAR_BUILD_TAG" ] && BEAR_BUILD_TAG="$(date --rfc-2822)"

# Compilation

cmake . \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DCLAW_ROOT_DIRECTORY:PATH="$ANDROID_LIBCLAW_DIR" \
    -Dlibclaw_DIR:PATH="$ANDROID_LIBCLAW_CMAKE_DIR" \
    -DJPEG_INCLUDE_DIR:PATH="$ANDROID_JPEG_INCLUDE_DIR" \
    -DJPEG_LIBRARY:FILEPATH="$ANDROID_JPEG_LIBRARY_PATH" \
    -DPNG_PNG_INCLUDE_DIR:PATH="$ANDROID_PNG_INCLUDE_DIR" \
    -DPNG_LIBRARY:FILEPATH="$ANDROID_PNG_LIBRARY_PATH" \
    -DZLIB_LIBRARY:FILEPATH="$ANDROID_ZLIB_LIBRARY_PATH" \
    -DBoost_INCLUDE_DIR:PATH="$ANDROID_BOOST_INCLUDE_DIR" \
    -DSDL_INCLUDE_DIR:PATH="$ANDROID_SDL_INCLUDE_DIR" \
    -DSDL_LIBRARY:FILEPATH="$ANDROID_SDL_LIBRARY_PATH" \
    -DSDL_MIXER_INCLUDE_DIR:PATH="$ANDROID_SDL_MIXER_INCLUDE_DIR" \
    -DSDL_MIXER_LIBRARY:FILEPATH="$ANDROID_SDL_MIXER_LIBRARY_PATH" \
    -DGETTEXT_MSGMERGE_EXECUTABLE:FILEPATH="$GETTEXT_MSGMERGE_PATH" \
    -DGETTEXT_MSGFMT_EXECUTABLE:FILEPATH="$GETTEXT_MSGFMT_PATH" \
    -DFREETYPE_LIBRARY:FILEPATH="$ANDROID_FREETYPE_LIBRARY" \
    -DFREETYPE_INCLUDE_DIRS:PATH="$ANDROID_FREETYPE_INCLUDE_DIR" \
    -DBEAR_EDITORS_ENABLED=0 \
    -DBEAR_ENGINE_NO_MANPAGES=1 \
    -DROLLER_PAINTING_ENABLED=1 \
    -DRP_NO_MANPAGES=1 \
    -DBEAR_ENGINE_CORE_LINK_TYPE=STATIC \
    -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX" \
    -DBEAR_BUILD_TAG="$BEAR_BUILD_TAG" \
    -DBUILD_PLATFORM="android"
    $CMAKE_ARGS
