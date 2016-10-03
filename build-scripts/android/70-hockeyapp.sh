#!/bin/sh

set -e

. $(dirname $0)/config.sh

cd $ASGP_APK_ROOT/java

curl --progress-bar --connect-timeout 120 \
     -F "status=2" -F "notify=0" -F "notes_type=0" \
     -H "X-HockeyAppToken: 4d6173e6883842538a0648f5a8bec9d5" \
     -F "ipa=@$TARGET_APK" \
     -F "libs=@$ASGP_SYMBOLS" \
     "https://rink.hockeyapp.net/api/2/apps/upload" \
     | tee /dev/null

echo
