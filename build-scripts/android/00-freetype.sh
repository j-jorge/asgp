#!/bin/sh

set -e

. $(dirname $0)/config.sh

ARCHIVE_NAME="freetype-2.6.tar.gz"
ARCHIVE_MD5="1d733ea6c1b7b3df38169fbdbec47d2b"
DOWNLOAD_LINK="http://download.savannah.gnu.org/releases/freetype/$ARCHIVE_NAME"
FREETYPE_SOURCE_DIR="freetype-2.6"

TARGET="$CACHE/$ARCHIVE_NAME"

ARCHIVE=$(download_file "$DOWNLOAD_LINK" "$ARCHIVE_NAME" "$ARCHIVE_MD5")

cd $CACHE

if [ ! -d "$FREETYPE_SOURCE_DIR" ]
then
    tar xf "$ARCHIVE"
fi

cd "$FREETYPE_SOURCE_DIR"

CXX=$ANDROID_CXX CC=$ANDROID_CC AR=$ANDROID_AR ./configure --host=arm-linux-androidabi \
   --prefix=$INSTALL_PREFIX -without-zlib -without-harfbuzz -without-png \
   CFLAGS="$CFLAGS -O3 -DNDEBUG" LDFLAGS="$LDFLAGS"
make
make install

set_shell_variable \
    ANDROID_FREETYPE_INCLUDE_DIR $INSTALL_PREFIX/include/freetype2
set_shell_variable \
    ANDROID_FREETYPE_LIBRARY $INSTALL_PREFIX/lib/libfreetype.a
