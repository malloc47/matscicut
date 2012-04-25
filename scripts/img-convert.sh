#!/bin/bash
echo -n "" > jobs
path=/home/malloc47/src/projects/matsci/matsciskel
for f in $(find $1 -maxdepth 3 -name \*.label | grep -v ^./ground) ; do
	if [[ ! -f ${f%.*}.png  ]] && [[ -f ./img/$(basename ${f%.*}.png) ]] || [[ -f ./img/$(basename ${f%.*}.tif) ]] ; then
	[[ -f ./img/$(basename ${f%.*}.png) ]] && type=png
	[[ -f ./img/$(basename ${f%.*}.tif) ]] && type=tif
	#if [ ! -f ${f%.*}.png -a -f ./img/$(basename ${f%.*}.$type) ]; then
		#echo "../scripts/label-to-img.sh $f ./img/$(basename ${f%.*}.$type) ${f%.*}.png" >> jobs
		echo "$path/render_labels.py $f ./img/$(basename ${f%.*}.$type) ${f%.*}.png" >> jobs
    fi
done
