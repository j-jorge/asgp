#!/bin/bash

if [ $# -ne 5 ]
then
    (
        echo "$0 xcf_file sprite_sheet_name sprite_sheet_size sprite_size sprites_per_sheet"
        echo "example:"
        echo $0 font.xcf font '"512 x 512"' '"autosize * 0.5"' 6
    ) 1>&2

    exit 1
fi

XCF_FILE="$1"
SPRITE_SHEET_NAME="$2"
SPRITE_SHEET_SIZE="$3"
SPRITE_SIZE="$4"

SPRITES_PER_SHEET=$5

SPRITE_SHEET_INDEX=1
SPRITE_INDEX=0

xcfinfo "$XCF_FILE" \
    | tail -n +2 \
    | cut -d' ' -f5- \
    | grep -v -- '-.' \
    | while read LAYER
    do

        L=
        case "$LAYER" in
            '"' ) L='\"'
                ;;
            * ) L="$LAYER"
        esac

        if ! (echo "$L" | iconv -t ISO-8859-15 > /dev/null 2> /dev/null)
        then
            continue
        fi

        SPRITE_INDEX=$(( $SPRITE_INDEX + 1 ))

        if [ "$SPRITE_INDEX" -eq $(( $SPRITES_PER_SHEET + 1)) ]
        then
            SPRITE_INDEX=1
            SPRITE_SHEET_INDEX=$(( $SPRITE_SHEET_INDEX + 1 ))
        fi
        
        if [ "$SPRITE_INDEX" -eq 1 ]
        then
            echo -n "sprite_sheet \"$SPRITE_SHEET_NAME-$SPRITE_SHEET_INDEX\""
            echo " $SPRITE_SHEET_SIZE"
            echo
            echo "img \"$XCF_FILE\""
        fi

        echo "\"$(echo $L | iconv -t ISO-8859-15)\" $SPRITE_SIZE with img"
        echo "  \"$L\""
        echo "  ;"
        echo
      done
