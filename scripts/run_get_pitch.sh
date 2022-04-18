#!/bin/bash

# Put here the program (maybe with path)

umaxnorm=$1
llindarPos=$2
llindarNeg=$3
llindarUnvoiced=$4
llindarPot=$5


if [ -z "$umaxnorm" ]
then
    umaxnorm=0.36
fi

if [ -z "$llindarPos" ]
then
    llindarPos=0.014
fi

if [ -z "$llindarNeg" ]
then
    llindarNeg=-0.014
fi

if [ -z "$llindarUnvoiced" ]
then 
    llindarUnvoiced=0.001
fi

if [ -z "$llindarPot" ]
then 
    llindarPot=-43.5
fi

GETF0="get_pitch --umaxnorm=$umaxnorm --llindarPos=$llindarPos --llindarNeg=$llindarNeg --llindarUnvoiced=$llindarUnvoiced --llindarPot=$llindarPot"
for fwav in pitch_4k/train/*.wav; do
    ff0=${fwav/.wav/.f0}
    echo "$GETF0 $fwav $ff0 ----"
    $GETF0 $fwav $ff0 > /dev/null || (echo "Error in $GETF0 $fwav $ff0"; exit 1)
done




exit 0
