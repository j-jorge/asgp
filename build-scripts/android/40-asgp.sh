#!/bin/bash

set -e

. $(dirname $0)/config.sh

BUILD_TYPE=$(build_mode $@)
BUILD_DIR="$CACHE/asgp/$BUILD_TYPE"
mkdir --parents "$BUILD_DIR"

ASGP_INSTALL_PREFIX="$ASGP_APK_ROOT/java/"
ASGP_ASSETS_DIR_NAME="assets"

configure()
{
    pushd "$BUILD_DIR"

    pwd
    
    CC="$ANDROID_CC $CFLAGS" CXX="$ANDROID_CXX $CXXFLAGS" AR=$ANDROID_AR \
       cmake -B. -H$SOURCE_ROOT \
       -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
       -DCMAKE_PREFIX_PATH=$INSTALL_PREFIX \
       -DBoost_INCLUDE_DIR:PATH="$ANDROID_BOOST_INCLUDE_DIR" \
       -DBoost_USE_STATIC_RUNTIME=ON \
       -DPNG_PNG_INCLUDE_DIR:PATH="$ANDROID_PNG_INCLUDE_DIR" \
       -DPNG_LIBRARY:FILEPATH="$ANDROID_PNG_LIBRARY" \
       -DBUILD_PLATFORM="android" \
       -DCMAKE_INSTALL_PREFIX="$ASGP_INSTALL_PREFIX" \
       -DBEAR_ROOT_DIRECTORY="$BEAR_ROOT_DIR" \
       -DRP_INSTALL_DATA_DIR="$ASGP_ASSETS_DIR_NAME"

    popd
}

convert_localization()
{
    pushd "$SOURCE_ROOT/desktop/locale/straining-coasters"

    TMP_FILE=$(mktemp)

    # Converts the gettext files into the ISO-8859-15 encoding
    # since libintl-lite does not convert the string encoding.
    find . -name "*.po" \
        | while read f
    do
        printf "Converting encoding of '%s'\n" "$f"

        git checkout HEAD "$f"
        
        iconv $f --to-code ISO-8859-15 \
            | sed 's:charset=UTF-8:charset=ISO-8859-15:' \
                  > $TMP_FILE
        cp $TMP_FILE $f
    done

    rm $TMP_FILE
    popd
}

restore_localization()
{
    pushd "$SOURCE_ROOT/desktop/locale/straining-coasters"
    git checkout HEAD "$SOURCE_ROOT/desktop/locale/straining-coasters"
    popd
}

build()
{
    pushd "$BUILD_DIR"
    rm -fr "$ASGP_INSTALL_PREFIX/$ASGP_ASSETS_DIR_NAME"
    make -j$(nproc) install
    mv "$ASGP_INSTALL_PREFIX/lib/librp.a" "$INSTALL_PREFIX/lib/"
    popd
}

copy_android_assets()
{
    pushd "$SOURCE_ROOT/android/data"

    cp --parents $( find . -name "*.png" \
                         -o -name "*.canim" \
                         -o -name "*.spritepos" ) \
       "$ASGP_INSTALL_PREFIX/$ASGP_ASSETS_DIR_NAME/"
    
    popd
}

install_locale()
{
    rm --force --recursive "$ASGP_INSTALL_PREFIX/$ASGP_ASSETS_DIR_NAME/share"
    mkdir "$ASGP_INSTALL_PREFIX/$ASGP_ASSETS_DIR_NAME/share"
    mv "$ASGP_INSTALL_PREFIX/share/locale" \
       "$ASGP_INSTALL_PREFIX/$ASGP_ASSETS_DIR_NAME/share/"

    rm --force $(find "$ASGP_INSTALL_PREFIX/$ASGP_ASSETS_DIR_NAME/" \
                      -name "AUTHORS")
}

convert_localization
configure
build
restore_localization

copy_android_assets
install_locale
