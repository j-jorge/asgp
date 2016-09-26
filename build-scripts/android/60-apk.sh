#!/bin/sh

set -e

. $(dirname $0)/config.sh

cd $ASGP_APK_ROOT/java

rm -fr bin

APP_NAME="straining-coasters"
BUILD_MODE=debug

for ARG in $@
do
    if [ "$ARG" = "--apk-release" ]
    then
        BUILD_MODE=release
    fi
done

TARGET_APK="$PWD/build/outputs/apk/$APP_NAME-$BUILD_MODE.apk"

if [ "$BUILD_MODE" = "release" ]
then
    gradle assembleRelease

    GENERATED_APK="$PWD/build/outputs/apk/java-release-unsigned.apk"
    jarsigner -verbose -sigalg SHA1withRSA -digestalg SHA1 \
              -keystore ~/.keytool/release.keystore \
              "$GENERATED_APK" play-store

    UNALIGNED_APK="$PWD/build/outputs/apk/java-release-unaligned.apk"
    mv "$GENERATED_APK" "$UNALIGNED_APK"

    BUILD_TOOLS_VERSION=$(grep buildToolsVersion build.gradle | cut -d'"' -f2)
    
    ALIGNED_APK="$PWD/build/outputs/apk/java-release-aligned.apk"
    
    $ANDROID_SDK/build-tools/$BUILD_TOOLS_VERSION/zipalign -v -f 4 \
        "$UNALIGNED_APK" "$ALIGNED_APK"

    GENERATED_APK="$ALIGNED_APK"
else
    gradle assembleDebug

    GENERATED_APK="$PWD/build/outputs/apk/java-debug.apk"
fi

mv "$GENERATED_APK" "$TARGET_APK"
set_shell_variable TARGET_APK "$TARGET_APK"
