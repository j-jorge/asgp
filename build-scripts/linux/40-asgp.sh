#!/bin/bash

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")"; pwd)"
. "$SCRIPT_DIR"/config.sh

ASGP_BUILD_DIR="$(project_build_dir asgp)"

mkdir -p "$ASGP_BUILD_DIR"
cd "$ASGP_BUILD_DIR"

cmake "$SOURCE_ROOT" \
      -DCMAKE_BUILD_TYPE="$BUILD_MODE" \
      -DCMAKE_PREFIX_PATH="$INSTALL_PREFIX" \
      -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX" \
      -DRP_NO_MANPAGES=1 \
      -DBEAR_ROOT_DIRECTORY="$BEAR_ROOT_DIR"

make -j$(nproc) install
