#!/bin/sh

# Search the c++ compiler for Android
CXX_COMPILER=

if [ -z "$ANDROID_CXX" ]
then
    echo "ANDROID_CXX is not set. Could not find Android's C++ compiler." 1>&2
    exit 1;
else
    CXX_COMPILER="$ANDROID_CXX"
fi

export CXX="$CXX_COMPILER"

CXX_FLAGS="-O3 -DNDEBUG"

$CXX $CXX_FLAGS $DEFINES \
    -fPIC -shared \
    -Wl,-soname,libandy-super-great-park.so \
    -Wl,--export-dynamic -rdynamic \
    -I. \
    -I../../../../bear/bear-engine/core/src \
    code/*.cpp \
    -o '../../../android/java/libs/armeabi-v7a/libandy-super-great-park.so' \
    -L../../../../bear/bear-engine/bin/ \
    -L../../../bin/ \
    -L$ANDROID_TOOLCHAIN_ROOT/sysroot/usr/lib \
    -Wl,--no-as-needed -Wl,--whole-archive \
    -lrp \
    -lbear_generic_items \
    -Wl,--as-needed -Wl,--no-whole-archive \
    -lbear_engine \
    -lbear_audio \
    -lbear_communication \
    -lbear_debug \
    -lbear_expr \
    -lbear_gui \
    -lbear_input \
    -lbear_net \
    -lbear_text_interface \
    -lbear_time \
    -lbear_universe \
    -lbear_visual \
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
    -Wl,-Bdynamic \
    -lgnustl_shared \
    -lz \
    -lGLESv2 \
    -lGLESv1_CM \
    -llog \
    -ldl \
    -landroid

