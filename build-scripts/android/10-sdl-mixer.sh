#!/bin/sh

set -e

. $(dirname $0)/config.sh

ARCHIVE_NAME="SDL2_mixer-2.0.1.tar.gz"
ARCHIVE_MD5="c6c4f556d4415871f526248f5c9a627d"
DOWNLOAD_LINK="https://www.libsdl.org/projects/SDL_mixer/release/$ARCHIVE_NAME"
SDL_SOURCE_DIR="SDL2_mixer-2.0.1"

ARCHIVE=$(download_file "$DOWNLOAD_LINK" "$ARCHIVE_NAME" "$ARCHIVE_MD5")

cd $CACHE

if [ -d "$SDL_SOURCE_DIR" ]
then
    cd "$SDL_SOURCE_DIR"
else
    tar xf "$ARCHIVE"
    cd "$SDL_SOURCE_DIR"
    patch -p0 < "$PATCHES"/sdl-mixer.diff
fi

ndk_build

cp SDL_mixer.h $INSTALL_PREFIX/include/SDL2/
cp obj/local/$ANDROID_ABI/libSDL2_mixer.a $INSTALL_PREFIX/lib/

set_shell_variable \
    ANDROID_SDL_MIXER_LIBRARY $INSTALL_PREFIX/lib/libSDL2_mixer.a
