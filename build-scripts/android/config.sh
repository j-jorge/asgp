#!/bin/sh

set -e

SCRIPT_ROOT=$(cd $(dirname $0); pwd)

export SOURCE_ROOT="$SCRIPT_ROOT"/../../

export ANDROID_ABI=armeabi
export ANDROID_CC="$ANDROID_TOOLCHAIN_ROOT/bin/arm-linux-androideabi-gcc"
export ANDROID_CXX="$ANDROID_TOOLCHAIN_ROOT/bin/arm-linux-androideabi-g++"
export ANDROID_AR="$ANDROID_TOOLCHAIN_ROOT/bin/arm-linux-androideabi-ar"

CACHE_ROOT="$SCRIPT_ROOT"/.cache
DOWNLOAD_DIR="$CACHE_ROOT/downloads"

export PATCHES="$SCRIPT_ROOT"/patches/

export CFLAGS="-Wno-unused-local-typedefs -Wno-deprecated-declarations"
export CXXFLAGS="$CFLAGS -Wno-delete-non-virtual-dtor"
export LDFLAGS=""

CFLAGS_ARMV7="-march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16"
LDFLAGS_ARMV7="-march=armv7-a -Wl,--fix-cortex-a8"

CFLAGS_ARMV7_HARD="$CFLAGS_ARMV7 -mhard-float -D_NDK_MATH_NO_SOFTFP=1"
LDFLAGS_ARMV7_HARD="$LDFLAGS_ARMV7 -Wl,--no-warn-mismatch -lm_hard"

for ARG in $@
do
    case $ARG in
        --abi=armv7)
            export ANDROID_ABI=armeabi-v7a
            export CFLAGS="$CFLAGS_ARMV7 $CFLAGS"
            export CXXFLAGS="$CFLAGS_ARMV7 $CXXFLAGS"
            export LDFLAGS="$LDFLAGS_ARMV7 $LDFLAGS"
            ;;
        --abi=armv7-hard)
            export ANDROID_ABI=armeabi-v7a-hard
            export CFLAGS="$CFLAGS_ARMV7_HARD $CFLAGS"
            export CXXFLAGS="$CFLAGS_ARMV7_HARD $CXXFLAGS"
            export LDFLAGS="$LDFLAGS_ARMV7_HARD $LDFLAGS"
            ;;
    esac
done

export CACHE="$CACHE_ROOT"/$ANDROID_ABI
export INSTALL_PREFIX="$CACHE/usr/"

CONFIG_VARIABLES_FILE=$CACHE/variables.sh

[ -d "$CACHE" ] || mkdir --parents "$CACHE"
[ -d "$INSTALL_PREFIX" ] || mkdir "$INSTALL_PREFIX"
[ -d $INSTALL_PREFIX/include ] || mkdir $INSTALL_PREFIX/include
[ -d $INSTALL_PREFIX/lib ] || mkdir $INSTALL_PREFIX/lib
[ -f $CONFIG_VARIABLES_FILE ] && . "$CONFIG_VARIABLES_FILE"

remove_variable_entry_if_any()
{
    KEY="$1"
    FILE="$2"

    [ -f "$CONFIG_VARIABLES_FILE" ] || return 0

    TMP=$(mktemp /tmp/tmp.XXXXXXXXXX)
    ( grep -v "$KEY=" "$CONFIG_VARIABLES_FILE" || echo ) > $TMP
    mv $TMP "$CONFIG_VARIABLES_FILE"
}

set_shell_variable()
{
    KEY="$1"
    VALUE="$2"

    printf "Setting shell config variable: '%s=%s'.\n" \
           "$KEY" "$VALUE"

    remove_variable_entry_if_any "$KEY"
    printf 'export %s="%s"\n' "$KEY" "$VALUE" >> "$CONFIG_VARIABLES_FILE"
}

build_mode()
{
    for ARG in $@
    do
        if [ "$ARG" = "--debug" ]
        then
            printf "debug\n"
            return
        fi
    done

    printf "release\n"
}

download_file()
{
    [ -d "$DOWNLOAD_DIR" ] || mkdir --parents "$DOWNLOAD_DIR"

    URL="$1"
    TARGET="$DOWNLOAD_DIR/$2"
    MD5="$3"
    
    if [ ! -f "$TARGET" ] \
       || [ $(md5sum $TARGET | cut -f1 -d' ') != "$MD5" ]
    then
        rm --force "$TARGET"
        curl -L -o "$TARGET" "$URL" 
    fi

    echo $TARGET
}

ndk_build()
{
    if [ -f ./Android.mk ]
    then
        ARGS="$ARGS APP_BUILD_SCRIPT=./Android.mk"
    fi

    if [ ! -z "$1" ]
    then
        ARGS="$ARGS obj/local/$ANDROID_ABI/$1"
    fi
    
    $NDK_ROOT/ndk-build NDK_PROJECT_PATH=. \
                        APP_PLATFORM=android-18 NDK_DEBUG=0 \
                        APP_ABI=$ANDROID_ABI \
                        APP_CFLAGS="$CFLAGS" APP_CXXFLAGS="$CXXFLAGS" \
                        APP_LDFLAGS="$LD_FLAGS" \
                        $ARGS
}
