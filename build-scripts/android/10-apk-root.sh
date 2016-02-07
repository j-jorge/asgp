#!/bin/sh

set -e

. $(dirname $0)/config.sh

ASGP_APK_ROOT=$CACHE/asgp-apk

rm -fr "$ASGP_APK_ROOT"
cp -r "$SOURCE_ROOT/android/" "$ASGP_APK_ROOT"

mkdir --parents $ASGP_APK_ROOT/java/src/org/libsdl/app/
sed 's|loadLibraries();||g' \
    $SDL_SOURCE_DIR/android-project/src/org/libsdl/app/SDLActivity.java \
    > $ASGP_APK_ROOT/java/src/org/libsdl/app/SDLActivity.java

cp $SDL_SOURCE_DIR/src/main/android/SDL_android_main.c \
   $ASGP_APK_ROOT/lib/src/code/

set_shell_variable ASGP_APK_ROOT "$ASGP_APK_ROOT"
