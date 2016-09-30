#!/bin/sh

set -e

. $(dirname $0)/config.sh

cd $CACHE

if [ ! -d rapidjson ]
then
    git clone https://github.com/j-jorge/rapidjson.git
fi

cd rapidjson

CC="$ANDROID_CC $CFLAGS" CXX="$ANDROID_CXX $CXXFLAGS" AR=$ANDROID_AR cmake . \
   -DCMAKE_BUILD_TYPE=release \
   -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX \
   -DRAPIDJSON_BUILD_DOC=OFF \
   -DRAPIDJSON_BUILD_EXAMPLES=OFF \
   -DRAPIDJSON_BUILD_TESTS=OFF \
   -DRAPIDJSON_BUILD_THIRDPARTY_GTEST=OFF

make install

set_shell_variable \
    ANDROID_RAPIDJSON_INCLUDE_DIR $INSTALL_PREFIX/include/
set_shell_variable \
    ANDROID_RAPIDJSON_LIBRARY $INSTALL_PREFIX/lib/librapidjson.a
