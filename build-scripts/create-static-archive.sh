#!/bin/sh

SOURCE_ROOT=$(echo $PWD/$0 | sed 's:\(.\+/\)[^/]\+:\1:')../
BINARY_ROOT=static-build-tree

. $SOURCE_ROOT/build-scripts/version.sh

sh $SOURCE_ROOT/build-scripts/build-static.sh
make -f Makefile.static

cp $SOURCE_ROOT/release/ReadMe.txt \
    $SOURCE_ROOT/release/ChangeLog \
    $BINARY_ROOT/

cmake . -DCMAKE_BUILD_TYPE=release \
    -DCMAKE_INSTALL_PREFIX=$BINARY_ROOT \
    -DBEAR_EDITORS_ENABLED=0
cd asgp/data
make install > /dev/null
cd - > /dev/null

cd bear-engine/desktop/locale/bear-engine/
make install > /dev/null
cd - > /dev/null

cd asgp/desktop/locale/super-great-park/
make install > /dev/null
cd - > /dev/null

RUNTIME_LIB_DIRECTORY=$BINARY_ROOT/bin/lib
rm -fr $RUNTIME_LIB_DIRECTORY
mkdir -p $RUNTIME_LIB_DIRECTORY

ldd $BINARY_ROOT/bin/andy-super-great-park \
    | sed 's:^[[:blank:]]::' \
    | grep -v '^/' \
    | grep '/' \
    | sed 's:=>::g;s:(.\+)$::g' \
    | while read LIBRARY_NAME LIBRARY_PATH
do
    cp $LIBRARY_PATH $RUNTIME_LIB_DIRECTORY/$LIBRARY_NAME
done

make_tar() {

    EXE_NAME="$1"

    tar cfz ${EXE_NAME}_${ASGP_VERSION}_$(uname -p).tar.gz \
        bin/$EXE_NAME \
        bin/lib \
        ReadMe.txt \
        ChangeLog \
        share/ \
        --transform "s:^bin/::;s:^:$EXE_NAME/:g"
}

cd $BINARY_ROOT
make_tar andy-super-great-park

cd - > /dev/null
