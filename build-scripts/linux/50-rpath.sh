#!/bin/bash

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")"; pwd)"
. "$SCRIPT_DIR"/config.sh

find "$INSTALL_PREFIX/lib" -maxdepth 1 -name "*.so" \
    | while read F
do
    patchelf --set-rpath '$ORIGIN' "$F"
done

patchelf --set-rpath '$ORIGIN/../lib' "$INSTALL_PREFIX/bin/straining-coasters"
