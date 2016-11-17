#!/bin/sh

set -e

. $(dirname $0)/config.sh

cd $CACHE

if [ -d libclaw ]
then
    cd libclaw
    git pull
else
    git clone git@github.com:j-jorge/libclaw.git
    cd libclaw
fi

CC="$ANDROID_CC $CFLAGS" CXX="$ANDROID_CXX $CXXFLAGS" AR=$ANDROID_AR cmake . \
   -DCMAKE_BUILD_TYPE=release \
   -DBoost_INCLUDE_DIR:PATH="$ANDROID_BOOST_INCLUDE_DIR" \
   -DBoost_USE_STATIC_RUNTIME=ON \
   -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX \
   -DJPEG_INCLUDE_DIR:PATH="$ANDROID_JPEG_INCLUDE_DIR" \
   -DJPEG_LIBRARY:FILEPATH="$ANDROID_JPEG_LIBRARY" \
   -DPNG_PNG_INCLUDE_DIR:PATH="$ANDROID_PNG_INCLUDE_DIR" \
   -DPNG_LIBRARY:FILEPATH="$ANDROID_PNG_LIBRARY"

make install

set_shell_variable \
    ANDROID_LIBCLAW_CMAKE_DIR $INSTALL_PREFIX/share/cmake/libclaw/
