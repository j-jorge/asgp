#!/bin/sh

# This is the directory containing this script
SCRIPT_DIR=$(cd $(dirname $0); pwd)

. $SCRIPT_DIR/android/config.sh

cd "$SOURCE_ROOT"

. asgp/build-scripts/version.sh

ANDROID_ASGP_VERSION=$(xpath -q -e '/manifest/@android:versionName' asgp/android/java/AndroidManifest.xml \
    | cut -d'"' -f2)

if [ "$ASGP_VERSION" != "$ANDROID_ASGP_VERSION" ]
then
    echo "Version name in Android manifest ($ANDROID_ASGP_VERSION) is not the same than the source version ($ASGP_VERSION)." 1>&2
    exit 1
fi

run_all_scripts()
{
    ROOT=$1
    shift
    
    for f in $ROOT/*.sh
    do
        if [ -x $f ]
        then
            echo "\033[01;34mExecuting $f\033[00m"
            $f $@
        fi
    done
}

run_all_scripts $SCRIPT_DIR/android $@ 2>&1 | tee build.log
