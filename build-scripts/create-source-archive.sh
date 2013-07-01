#!/bin/sh

SOURCE_ROOT=$(echo $PWD/$0 | sed 's:\(.\+/\)[^/]\+:\1:')../

INITIAL_DIRECTORY=$PWD
ASGP_GIT_PATH=$SOURCE_ROOT/
BEAR_GIT_PATH=$SOURCE_ROOT/../bear/

. $SOURCE_ROOT/build-scripts/version.sh

WORKING_DIRECTORY=$(mktemp --directory)

cd "$WORKING_DIRECTORY"

ASGP_ARCHIVE_NAME=andy-super-great-park_$ASGP_VERSION

SOURCE_DIRECTORY=$ASGP_ARCHIVE_NAME
mkdir $SOURCE_DIRECTORY

cd $SOURCE_DIRECTORY

git archive master $ASGP_GIT_PATH | tar -x
git archive master $BEAR_GIT_PATH | tar -x

cd ..

tar cfz $INITIAL_DIRECTORY/$ASGP_ARCHIVE_NAME.tar.gz $ASGP_ARCHIVE_NAME

rm -fr "$WORKING_DIRECTORY"