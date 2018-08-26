#!/bin/sh

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")"; pwd)"
. "$SCRIPT_DIR"/config.sh

git_clone_project \
    bear \
    git@github.com:j-jorge/bear.git \
    2a785228d85997dc1682ee71899841528fa09c33

cd "$(project_build_dir bear)"

BEAR_ROOT_DIR="$(project_source_dir bear)"

cmake "$BEAR_ROOT_DIR" \
   -DCMAKE_BUILD_TYPE="$BUILD_MODE" \
   -DCMAKE_PREFIX_PATH="$INSTALL_PREFIX" \
   -DBEAR_EDITORS_ENABLED=0 \
   -DTESTING_ENABLED=0 \
   -DBEAR_ENGINE_NO_MANPAGES=1 \
   -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX"

make -j$(nproc) install

set_shell_variable BEAR_ROOT_DIR "$BEAR_ROOT_DIR"
