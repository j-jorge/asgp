#!/bin/bash

set -e

. $(dirname $0)/config.sh

if [ $(build_mode) = "release" ]
then
    CXX_FLAGS="-O3 -DNDEBUG"
else
    CXX_FLAGS="-O0 -g -D_DEBUG"
fi

TARGET_DIR="$ASGP_APK_ROOT/java/libs/$ANDROID_ABI/"

[ -d "$TARGET_DIR" ] || mkdir --parents "$TARGET_DIR"

ASGP_LIB_NAME=libandy-super-great-park.so
TARGET_SO="$TARGET_DIR/$ASGP_LIB_NAME"
SOURCES="$ASGP_APK_ROOT/lib/src/code/*.c*"

compile_object()
{
    if [ $2 -nt $1 ]
    then
        return
    fi
    
    if [[ $1 = *.c ]]
    then
        COMPILER="$ANDROID_CC $CFLAGS"
    else
        COMPILER="$ANDROID_CXX $CXXFLAGS"
    fi

    printf "Compiling %s\n" "$2"

    $COMPILER \
             -fPIC \
             -I"$SDL_SOURCE_DIR/src/main/android/" \
             -I"$SOURCE_ROOT/android/lib/src/" \
             -I"$BEAR_ROOT_DIR/bear-engine/core/src" \
             -I$INSTALL_PREFIX/include \
             -I$INSTALL_PREFIX/include/SDL2 \
             -c "$1" \
             -o "$2"
}

compile()
{
    for f in $SOURCES
    do
        OBJECT_FILE=$(echo $f | sed 's|c\(pp\)\?$|o|g')
        OBJECTS="$OBJECTS $OBJECT_FILE"
        compile_object $f $OBJECT_FILE
    done
        
    printf "Compiling %s\n" "$TARGET_SO"
    
    $ANDROID_CXX $CXXFLAGS $LDFLAGS \
             -fPIC -shared \
             -Wl,-soname,$ASGP_LIB_NAME \
             -Wl,--export-dynamic -rdynamic \
             $OBJECTS \
             -o "$TARGET_SO" \
             -L$INSTALL_PREFIX/lib \
             -Wl,--no-as-needed -Wl,--whole-archive \
             -lrp \
             -lbear_generic_items \
             -Wl,--as-needed -Wl,--no-whole-archive \
             -lbear_engine \
             -lbear_audio \
             -lbear_communication \
             -lbear_expr \
             -lbear_gui \
             -lbear_input \
             -lbear_net \
             -lbear_text_interface \
             -lbear_time \
             -lbear_universe \
             -lbear_visual \
             -lbear_debug \
             -Wl,-Bstatic \
             -lclaw_tween \
             -lclaw_graphic \
             -lclaw_net \
             -lclaw_application \
             -lclaw_configuration_file \
             -lclaw_dynamic_library \
             -lclaw_logger \
             -lfreetype \
             -lpng \
             -ljpeg \
             -lboost_thread \
             -lboost_regex \
             -lboost_signals \
             -lboost_filesystem \
             -lboost_system \
             -lintl \
             -lSDL2_mixer \
             -lSDL2 \
             -lsupc++ \
             -lstdioext \
             -Wl,-Bdynamic \
             -lgnustl_shared \
             -lz \
             -lGLESv2 \
             -lGLESv1_CM \
             -llog \
             -ldl \
             -landroid
}

compile

cp $ANDROID_TOOLCHAIN_ROOT/arm-linux-androideabi/lib/libgnustl_shared.so \
    "$TARGET_DIR"

$ANDROID_TOOLCHAIN_ROOT/bin/arm-linux-androideabi-strip --strip-all \
    "$TARGET_DIR"/*.so \
