#!/bin/sh

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")"; pwd)"
BUILD_MODE=release

for ARG in "$@"
do
    if [ "$ARG" = "--debug" ]
    then
        BUILD_MODE=debug
    fi
done

export BUILD_MODE
export SOURCE_ROOT="$SCRIPT_DIR"/../../

BUILD_DIR="$SOURCE_ROOT/build/linux/"

export CACHE="$BUILD_DIR/cache"
export INSTALL_PREFIX="$BUILD_DIR/$BUILD_MODE/install/"

CONFIG_VARIABLES_FILE="$CACHE/variables.sh"

[ -d "$CACHE" ] || mkdir --parents "$CACHE"
[ -d "$INSTALL_PREFIX" ] || mkdir --parents "$INSTALL_PREFIX"
[ -f "$CONFIG_VARIABLES_FILE" ] && . "$CONFIG_VARIABLES_FILE"

remove_variable_entry_if_any()
{
    KEY="$1"
    FILE="$2"

    [ -f "$CONFIG_VARIABLES_FILE" ] || return 0

    TMP=$(mktemp /tmp/tmp.XXXXXXXXXX)
    ( grep -v "$KEY=" "$CONFIG_VARIABLES_FILE" || echo ) > $TMP
    mv $TMP "$CONFIG_VARIABLES_FILE"
}

set_shell_variable()
{
    KEY="$1"
    VALUE="$2"

    printf "Setting shell config variable: '%s=%s'.\n" \
           "$KEY" "$VALUE"

    remove_variable_entry_if_any "$KEY"
    printf 'export %s="%s"\n' "$KEY" "$VALUE" >> "$CONFIG_VARIABLES_FILE"
}

git_clone_project()
{
    PROJECT_NAME="$1"
    PROJECT_REPOSITORY="$2"
    PROJECT_COMMIT="$3"

    cd "$CACHE"

    [ -d "$PROJECT_NAME" ] || mkdir "$PROJECT_NAME"
    cd "$PROJECT_NAME"

    [ -d "build/$BUILD_MODE" ] || mkdir --parents "build/$BUILD_MODE"

    if [ -d source ]
    then
        cd source
        git fetch origin
    else
        git clone "$PROJECT_REPOSITORY" source
        cd source
    fi
    
    git checkout "$PROJECT_COMMIT"
}

project_build_dir()
{
    printf "%s/%s/build/%s\n" "$CACHE" "$1" "$BUILD_MODE"
}

project_source_dir()
{
    printf "%s/%s/source/\n" "$CACHE" "$1"
}
