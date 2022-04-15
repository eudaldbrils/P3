#!/bin/bash

for fwav in *.wav; do 
    sox $fwav -r 4k ../../pitch_4k/train/$fwav
done