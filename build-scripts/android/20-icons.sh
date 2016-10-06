#!/bin/sh

set -e

. $(dirname $0)/config.sh

SOURCE="$SOURCE_ROOT/desktop/icon.png"

find "$ASGP_APK_ROOT/java/res" -name "ic_launcher.png" \
    | while read TARGET_ICON
do
    SIZE=$(identify -format "%wx%h" "$TARGET_ICON")
    convert "$SOURCE" -resize $SIZE "$TARGET_ICON"
done
