#!/bin/sh

APK_BUILD_TYPE=debug

if [ "$1" = "--release" ]
then
    APK_BUILD_TYPE=release
fi

# This is the directory containing this script
SOURCE_ROOT=$(echo $PWD/$0 | sed 's:\(.\+/\)[^/]\+:\1:')../../

cd "$SOURCE_ROOT"

. asgp/build-scripts/version.sh

ANDROID_ASGP_VERSION=$(xpath -q -e '/manifest/@android:versionName' asgp/android/java/AndroidManifest.xml \
    | cut -d'"' -f2)

if [ "$ASGP_VERSION" != "$ANDROID_ASGP_VERSION" ]
then
    echo "Version name in Android manifest ($ANDROID_ASGP_VERSION) is not the same than the source version ($ASGP_VERSION)." 1>&2
fi

# Search the c++ compiler for Android
CXX_COMPILER=

if [ -z "$ANDROID_CXX" ]
then
    echo "ANDROID_CXX is not set. Could not find Android's C++ compiler." 1>&2
    exit 1;
else
    CXX_COMPILER="$ANDROID_CXX"
fi

export CXX="$CXX_COMPILER --pedantic"

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

[ -z "$ANDROID_BOOST_DIR" ] \
    && ANDROID_BOOST_DIR="$ANDROID_SYSROOT"

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

# Programs
[ -z "$GETTEXT_MSGMERGE_PATH" ] \
    && GETTEXT_MSGMERGE_PATH="msgmerge"

[ -z "$GETTEXT_MSGFMT_PATH" ] \
    && GETTEXT_MSGFMT_PATH="msgfmt"

# Compilation flags
[ -z "$BUILD_TYPE" ] && BUILD_TYPE=release

[ -z "$INSTALL_PREFIX" ] && INSTALL_PREFIX="$PWD/asgp/android/java/"

[ -z "$BEAR_BUILD_TAG" ] && BEAR_BUILD_TAG="$(date --rfc-2822)"

# Compilation
DATA_DIR="assets"

rm -fr $INSTALL_PREFIX/$DATA_DIR/*

cmake . \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DCLAW_ROOT_DIRECTORY:PATH="$ANDROID_LIBCLAW_DIR" \
    -DCLAW_APPLICATION_LIB:PATH="$ANDROID_LIBCLAW_DIR/lib/libclaw_application.a" \
    -DCLAW_LOGGER_LIBRARIES:PATH="$ANDROID_LIBCLAW_DIR/lib/libclaw_logger.a" \
    -DCLAW_NET_LIB:PATH="$ANDROID_LIBCLAW_DIR/lib/libclaw_net.a" \
    -DCLAW_GRAPHIC_LIB:PATH="$ANDROID_LIBCLAW_DIR/lib/libclaw_graphic.a" \
    -DCLAW_TWEEN_LIB:PATH="$ANDROID_LIBCLAW_DIR/lib/libclaw_tween.a" \
    -DCLAW_DYNAMIC_LIBRARY_LIB:PATH="$ANDROID_LIBCLAW_DIR/lib/libclaw_dynamic_library.a" \
    -DCLAW_CONFIGURATION_FILES_LIBARIES:PATH="$ANDROID_LIBCLAW_DIR/lib/libclaw_configuration_file.a" \
    -Dlibclaw_DIR:PATH="$ANDROID_LIBCLAW_CMAKE_DIR" \
    -DJPEG_INCLUDE_DIR:PATH="$ANDROID_JPEG_INCLUDE_DIR" \
    -DJPEG_LIBRARY:FILEPATH="$ANDROID_JPEG_LIBRARY_PATH" \
    -DPNG_PNG_INCLUDE_DIR:PATH="$ANDROID_PNG_INCLUDE_DIR" \
    -DPNG_LIBRARY:FILEPATH="$ANDROID_PNG_LIBRARY_PATH" \
    -DZLIB_LIBRARY:FILEPATH="$ANDROID_ZLIB_LIBRARY_PATH" \
    -DBoost_INCLUDE_DIR:PATH="$ANDROID_BOOST_INCLUDE_DIR" \
    -DBoost_USE_STATIC_RUNTIME=ON \
    -DGETTEXT_MSGMERGE_EXECUTABLE:FILEPATH="$GETTEXT_MSGMERGE_PATH" \
    -DGETTEXT_MSGFMT_EXECUTABLE:FILEPATH="$GETTEXT_MSGFMT_PATH" \
    -DFREETYPE_LIBRARY:FILEPATH="$ANDROID_FREETYPE_LIBRARY" \
    -DFREETYPE_INCLUDE_DIRS:PATH="$ANDROID_FREETYPE_INCLUDE_DIR" \
    -DBEAR_EDITORS_ENABLED=0 \
    -DBEAR_ENGINE_NO_MANPAGES=1 \
    -DROLLER_PAINTING_ENABLED=1 \
    -DTUNNEL_ENABLED=0 \
    -DRP_NO_MANPAGES=1 \
    -DBEAR_ENGINE_CORE_LINK_TYPE=STATIC \
    -DBUILD_PLATFORM="android" \
    -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX" \
    -DRP_ANDROID_LAUNCHER_LINK_DIRECTORIES="$ANDROID_TOOLCHAIN_ROOT/arm-linux-androideabi/lib/" \
    -DRP_INSTALL_DATA_DIR="$DATA_DIR" \
    -DRP_INSTALL_ANDROID_LAUNCHER_DIR="libs/armeabi-v7a" \
    $CMAKE_ARGS

[ $? -eq 0 ] || exit 1

TMP_FILE=$(mktemp)

cd asgp

# Converts the gettext files into the ISO-8859-15 encoding
# since libintl-lite does not convert the string encoding.
find . -name "*.po" \
    | while read f
do
    echo "Converting encoding of $f"

    git checkout HEAD "$f"

    iconv $f --to-code ISO-8859-15 \
        | sed 's:charset=UTF-8:charset=ISO-8859-15:' \
        > $TMP_FILE
    cp $TMP_FILE $f
done

rm $TMP_FILE
cd ..

make install || exit 1

cd asgp

# Now we can restore the encoding to avoid commiting by mistake.
find . -name "*.po" \
    | while read f
do
    echo "Restoring encoding of $f"
    git checkout HEAD "$f"
done

cd ..

cd asgp/android/data
cp --parents $( find . -name "*.png" \
    -o -name "*.canim" \
    -o -name "*.spritepos" ) \
    $INSTALL_PREFIX/$DATA_DIR
cd -

rm --force --recursive $INSTALL_PREFIX/$DATA_DIR/share
mkdir $INSTALL_PREFIX/$DATA_DIR/share
mv $INSTALL_PREFIX/share/locale $INSTALL_PREFIX/$DATA_DIR/share/

rm --force $(find $INSTALL_PREFIX/$DATA_DIR/ -name "AUTHORS")

cd asgp/android/lib/src
echo "Building library with custom script."
sh ./make.sh || exit 1

cd -

cp $ANDROID_TOOLCHAIN_ROOT/arm-linux-androideabi/lib/libgnustl_shared.so \
    $PWD/asgp/android/java/libs/armeabi-v7a

$ANDROID_TOOLCHAIN_ROOT/bin/arm-linux-androideabi-strip --strip-all \
    $PWD/asgp/android/java/libs/armeabi-v7a/lib*.so \
    || exit 1

# Remove the static libraries, installed by default
rm $(grep '\.a$' < install_manifest.txt)

cd asgp/android/java

rm bin -fr

TARGET_APK=

if [ $APK_BUILD_TYPE = "release" ]
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

APK_SIZE=$(wc -c $TARGET_APK | cut -f1 -d' ')
APK_MAX_SIZE=$((50 * 1024 * 1024))

if [ $APK_SIZE -gt $APK_MAX_SIZE ]
then
    echo "/!\\ APK too large by $(($APK_SIZE - $APK_MAX_SIZE)) bytes /!\\"
fi

DEVICE_ID=$(adb devices | grep 'device$' | cut -f1)
echo "Installing on device $DEVICE_ID."
adb -s $DEVICE_ID install -r $TARGET_APK
