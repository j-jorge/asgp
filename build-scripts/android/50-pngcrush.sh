#!/bin/bash

set -e

. $(dirname $0)/config.sh

BUILD_TYPE=$(build_mode $@)

cd "$ASGP_APK_ROOT/java/"

PNGCRUSH_ARGS="-rem allb -reduce"

TEMP_PNG=$(mktemp)
trap "rm -f $TEMP_PNG" EXIT

find assets res -name "*.png" \
    | while read F
      do
          pngcrush -rem allb -reduce $F $TEMP_PNG
          cp $TEMP_PNG $F
      done
