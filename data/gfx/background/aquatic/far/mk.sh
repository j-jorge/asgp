#!/bin/bash

if [ -z "$1" ]
then
    echo $0 file.xcf 1>&2
    exit 1
fi

SOURCE_XCF="$1"
shift 1

# Finds the path to common.scm relatively to the current folder
function find_common_functions() {

    REF_DIR="$(pwd)"
    RESULT=

    while [ ! -f "common.scm" ] && [ ! "$(pwd)" == "/" ]
    do
        RESULT="../$RESULT"
        cd ..
    done

    cd "$REF_DIR"
    echo $RESULT"common.scm"
} # find_common_functions()

# Finds the indices of the layers to display.
function get_layers() {

    GREP_EXCLUDE=""

    for ARG in "$@"
    do
        case $ARG in
            -*) if [ -z "$GREP_EXCLUDE" ]
                then
                    GREP_EXCLUDE="\(${ARG:1}\)"
                else
                    GREP_EXCLUDE="$GREP_EXCLUDE\|\(${ARG:1}\)"
                fi;
                ;;
        esac
    done

    if [ -z "$GREP_EXCLUDE" ]
    then
        GREP_EXCLUDE='^$'
    fi

    xcfinfo "$SOURCE_XCF" | tail -n +2 | cat -b | grep -v "$GREP_EXCLUDE" \
        | while read INDEX REMAINING
    do
        echo -n "$(( $INDEX - 1 )) "
    done
} # get_layers

OUTPUT_PNG="$(mktemp).png"

WIDTH=$(xcfinfo "$SOURCE_XCF" | head -n 1 | cut -d' ' -f3 | cut -dx -f1)
HEIGHT=$(xcfinfo "$SOURCE_XCF" | head -n 1 | cut -d' ' -f3 | cut -dx -f2)

WIDTH=$(( $WIDTH / 2 ))
HEIGHT=$(( $HEIGHT / 2 ))
LAYERS=`get_layers "$@"`

gimp-console -b - <<EOF
(load "$(find_common_functions)")

(let ( (src_name "$SOURCE_XCF") )
  (let ( (src_image (car (gimp-file-load 1 src_name src_name)))
         (dest_image (new-image $WIDTH $HEIGHT))
         (dest_name "$OUTPUT_PNG")
       )

    (create-layer src_image '($LAYERS) 0 0 $WIDTH $HEIGHT dest_image)

    (save-frames-and-exit dest_name dest_image)
    ) ; let
  ) ; let

EOF

image-cutter -X=512 -Y=512 -f=bk_ $OUTPUT_PNG
rm $OUTPUT_PNG
