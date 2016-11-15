#!/bin/bash

set -e

. $(dirname $0)/config.sh

cd "$SOURCE_ROOT"

LAST_VERSION=$(git tag | cut -d'.' -f 3 | sort -g | tail -n 1)
ASGP_VERSION_PATCH=$(( $LAST_VERSION + 1 ))

set_shell_variable ASGP_VERSION_PATCH $ASGP_VERSION_PATCH
set_shell_variable ASGP_VERSION_FULL 1.0.$ASGP_VERSION_PATCH
