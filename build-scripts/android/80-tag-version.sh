#!/bin/sh

set -e

. $(dirname $0)/config.sh

if [ $(build_mode) = "release" ]
then
    cd "$SOURCE_ROOT"
    git tag $ASGP_VERSION_FULL
fi
