#!/bin/sh

set -e

. $(dirname $0)/config.sh

cd $CACHE

if [ ! -d android-stdioext ]
then
    git clone git@github.com:j-jorge/android-stdioext.git
fi

cd android-stdioext

ndk_build

cp include/stdioext.h $INSTALL_PREFIX/include/

cp obj/local/$ANDROID_ABI/libstdioext.a $INSTALL_PREFIX/lib/
