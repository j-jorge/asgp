#!/bin/sh

FULL_BUILD=1

rm -f CMakeCache.txt

. asgp/build-scripts/version.sh

do_build() {

    CMAKE_ARGS="$@"
    . asgp/build-scripts/windows-build.sh

    cp "$COMMON_LIBRARIES_DIR/"* "$INSTALL_PREFIX/bin/"
} # do_build

VERSION=${ASGP_VERSION}

do_build

cp asgp/release/installer-files/* "$INSTALL_PREFIX"

makensis "$INSTALL_PREFIX/asgp.nsi"
cp "$INSTALL_PREFIX"/asgp.exe asgp_$VERSION.exe

wine mingw32-make uninstall
rm -fr "$INSTALL_PREFIX"
