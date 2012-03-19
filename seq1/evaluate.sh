#!/bin/bash

for f in $(find $1 -maxdepth 3 -name \*.label | grep -v ^./ground) ; do
    # if there is groundtruth with the same name
    if [ -f ./ground/$(basename $f) -a ! -f ${f%.*}.score ] ; then
	echo ../scripts/fmeasure.sh ./ground/$(basename $f) $f ${f%.*}.score
	../scripts/fmeasure.sh ./ground/$(basename $f) $f ${f%.*}.score
    fi
done
