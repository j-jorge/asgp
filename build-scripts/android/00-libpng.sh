#!/bin/sh

set -e

. $(dirname $0)/config.sh

cd $CACHE

if [ ! -d libpng-android ]
then
    git clone https://github.com/julienr/libpng-android.git
fi

cd libpng-android

ndk_build

cp jni/png*.h $INSTALL_PREFIX/include/
cp obj/local/$ANDROID_ABI/libpng.a $INSTALL_PREFIX/lib/

set_shell_variable \
    ANDROID_PNG_INCLUDE_DIR $INSTALL_PREFIX/include/
set_shell_variable \
    ANDROID_PNG_LIBRARY $INSTALL_PREFIX/lib/libpng.a
