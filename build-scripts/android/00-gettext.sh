#!/bin/sh

set -e

. $(dirname $0)/config.sh

cd $CACHE

if [ ! -d libintl-lite ]
then
    git clone git@github.com:j-jorge/libintl-lite.git
fi

cd libintl-lite/internal

$ANDROID_CXX $CXXFLAGS -DNDEBUG -O3 -c libintl.cpp -o libintl.o
$ANDROID_AR rs ../libintl.a libintl.o

cd ..

cp libintl.a $INSTALL_PREFIX/lib
cp libintl.h $INSTALL_PREFIX/include
