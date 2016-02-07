#!/bin/sh

set -e

. $(dirname $0)/config.sh

cd $ASGP_APK_ROOT/java

mkdir --parents src/org/libsdl/app/
sed 's|loadLibraries();||g' \
    $SDL_SOURCE_DIR/android-project/src/org/libsdl/app/SDLActivity.java \
    > src/org/libsdl/app/SDLActivity.java

BUILD_MODE=

for ARG in $@
do
    if [ "$ARG" = "--apk-release" ]
    then
        BUILD_MODE=release
    fi
done

if [ "$BUILD_MODE" = "release" ]
then
    ant release

    ORIGINAL_APK=bin/ASGP-release-unsigned.apk
    TARGET_APK=bin/ASGP.apk

    jarsigner -sigalg SHA1withRSA -digestalg SHA1 \
        -keystore ~/.keytool/stuffomatic.keystore \
        $ORIGINAL_APK gplays

    zipalign 4 $ORIGINAL_APK $TARGET_APK
else
    ant debug
    TARGET_APK=bin/ASGP-debug.apk
fi

set_shell_variable TARGET_APK "$PWD/$TARGET_APK"
