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

BUILD_NUMBER_FILE="$CACHE/build-number"

if [ -f "$BUILD_NUMBER_FILE" ]
then
    BUILD_NUMBER=$(cat "$BUILD_NUMBER_FILE")
else
    BUILD_NUMBER=0
fi

sed 's|versionCode=".*"|versionCode="'$BUILD_NUMBER'"|' \
    -i $ASGP_APK_ROOT/java/AndroidManifest.xml
    
if [ $(build_mode $@) = debug ]
then
    sed 's|package=.*|package="com.stuffomatic.beta.coasters"|' \
        -i $ASGP_APK_ROOT/java/AndroidManifest.xml

    sed 's|com\.stuffomatic\.coasters\.R|com.stuffomatic.beta.coasters.R|' \
        -i $ASGP_APK_ROOT/java/src/com/stuffomatic/asgp/ASGP.java

    sed 's|Straining Coasters|Beta Coasters|' \
        -i $ASGP_APK_ROOT/java/res/values/strings.xml
else
    echo $(( $BUILD_NUMBER + 1 )) > $BUILD_NUMBER_FILE
fi

set_shell_variable ASGP_APK_ROOT "$ASGP_APK_ROOT"
