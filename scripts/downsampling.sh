#!/bin/bash

for fwav in *.wav; do 
    sox $fwav -r 4k ../../pitch_4k/train/$fwav
done

#executat dins del pitch_bd/train. Previament s'ha creat la carpeta pitch_4k/train