#!/bin/sh

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")"; pwd)"
. "$SCRIPT_DIR"/config.sh

git_clone_project \
    libclaw \
    git@github.com:j-jorge/libclaw.git \
    aaa735f48cb2e1212c3f5d1bf4e13b0879887384

cd "$(project_build_dir libclaw)"

cmake "$(project_source_dir libclaw)" \
   -DCMAKE_BUILD_TYPE="$BUILD_MODE" \
   -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX"

make -j$(nproc) install
