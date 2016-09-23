#!/bin/sh

set -e

. $(dirname $0)/config.sh

ARCHIVE_NAME="SDL2-2.0.4.tar.gz"
ARCHIVE_MD5="44fc4a023349933e7f5d7a582f7b886e"
DOWNLOAD_LINK="https://www.libsdl.org/release/$ARCHIVE_NAME"
SDL_SOURCE_DIR="SDL2-2.0.4"

ARCHIVE=$(download_file "$DOWNLOAD_LINK" "$ARCHIVE_NAME" "$ARCHIVE_MD5")

cd $CACHE

if [ -d "$SDL_SOURCE_DIR" ]
then
    cd "$SDL_SOURCE_DIR"
else
    tar xf "$ARCHIVE"
    cd "$SDL_SOURCE_DIR"
    patch -p0 < "$PATCHES/sdl-android-locale.diff"
    patch -p0 < "$PATCHES/sdl-events-after-resume.diff"
fi

ndk_build libSDL2.a

[ -d $INSTALL_PREFIX/include/SDL2/ ] || mkdir $INSTALL_PREFIX/include/SDL2/
cp --update include/* $INSTALL_PREFIX/include/SDL2/
cp --update obj/local/$ANDROID_ABI/libSDL2.a $INSTALL_PREFIX/lib/

set_shell_variable \
    SDL_SOURCE_DIR $CACHE/$SDL_SOURCE_DIR
set_shell_variable \
    ANDROID_SDL_INCLUDE_DIR $INSTALL_PREFIX/include/
set_shell_variable \
    ANDROID_SDL_LIBRARY $INSTALL_PREFIX/lib/libSDL2.a
