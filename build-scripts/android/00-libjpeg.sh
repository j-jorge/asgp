#!/bin/sh

set -e

. $(dirname $0)/config.sh

cd $CACHE

if [ -d libjpeg-turbo ]
then
    cd libjpeg-turbo
else
    git clone \
        git://git.linaro.org/people/tomgall/libjpeg-turbo/libjpeg-turbo.git \
        -b linaro-android

    cd libjpeg-turbo
    patch -p0 < "$PATCHES"/jpeg-android.diff
fi

ndk_build libjpeg.a

cp jconfig.h jerror.h jmorecfg.h jpegint.h jpeglib.h turbojpeg.h \
   $INSTALL_PREFIX/include/

cp obj/local/$ANDROID_ABI/libjpeg.a \
   $INSTALL_PREFIX/lib/

set_shell_variable \
    ANDROID_JPEG_INCLUDE_DIR $INSTALL_PREFIX/include/
set_shell_variable \
    ANDROID_JPEG_LIBRARY $INSTALL_PREFIX/lib/libjpeg.a
