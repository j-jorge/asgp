#!/bin/sh

set -e

. $(dirname $0)/config.sh

DEVICE_ID=$(adb devices | grep 'device$' | cut -f1)

[ ! -z "$DEVICE_ID" ] || exit 0

echo "Installing on device $DEVICE_ID."
adb -s $DEVICE_ID install -r $TARGET_APK
