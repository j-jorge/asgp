#!/bin/sh

SCRIPT_DIR="$(cd "$(dirname "$0")"; pwd)"

. "$SCRIPT_DIR"/linux/config.sh

cd "$SOURCE_ROOT"

. asgp/build-scripts/version.sh

run_all_scripts()
{
    ROOT="$1"
    shift
    
    for f in "$ROOT"/*.sh
    do
        if [ -x "$f" ]
        then
            echo "\033[01;34mExecuting $f\033[00m"
            "$f" "$@"
        fi
    done
}

run_all_scripts "$SCRIPT_DIR"/linux "$@" 2>&1 | tee build.log
