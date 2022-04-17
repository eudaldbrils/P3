#!/bin/bash 
for umaxnorm in $(seq 0.3 0.005 0.4);do
    for llindarNeg in $(seq -0.04 0.005 0);do
        for llindarPos in $(seq 0 0.005 0.04);do
            for llindarUnvoiced in $(seq 0 0.0001 0.0015);do
                echo -n "umaxnorm=$umaxnorm llindarNeg=$llindarNeg llindarPos=$llindarPos llindarUnvoiced=$llindarUnvoiced   "
                scripts/run_get_pitch.sh $umaxnorm $llindarNeg $llindarPos $llindarUnvoiced > /dev/null
                pitch_evaluate pitch_4k/train/*f0ref | fgrep TOTAL
            done  
        done

    done 
    
done | sort -t: -k 2n;
exit 0