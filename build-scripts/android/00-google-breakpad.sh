#!/bin/sh

set -e

. $(dirname $0)/config.sh

cd $CACHE

if [ ! -d breakpad ]
then
    git clone https://chromium.googlesource.com/breakpad/breakpad
fi

cd breakpad

if [ ! -d src/third_party/lss ]
then
    git clone https://chromium.googlesource.com/linux-syscall-support \
        src/third_party/lss
fi

CXX=$ANDROID_CXX CC=$ANDROID_CC AR=$ANDROID_AR ./configure \
   --host=arm-linux-androidabi \
   --disable-processor \
   --disable-tools \
   --prefix=$INSTALL_PREFIX \
   CFLAGS="$CFLAGS -O3 -DNDEBUG" LDFLAGS="$LDFLAGS -I$PWD/src"

make -j$(nproc)

set_shell_variable \
    ANDROID_BREAKPAD_INCLUDE_DIR $PWD/src
set_shell_variable \
    ANDROID_BREAKPAD_LIBRARY $PWD/src/client/linux/libbreakpad_client.a
