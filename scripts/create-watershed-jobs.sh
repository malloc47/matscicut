#!/bin/bash

j=0
seq=$3
seqpath=/home/malloc47/src/projects/matsci/matscicut/$seq/watershed
labelpath=/home/malloc47/src/projects/matsci/matscicut/$seq/watershed/labels
groundpath=/home/malloc47/src/projects/matsci/matscicut/$seq/ground
scriptpath=/home/malloc47/src/projects/matsci/matscicut/scripts/$seq

echo "#!/bin/bash" > $seq/${j}w.sh
for (( s=$1; s<=$2; s++ )) ; do

	echo "mkdir $labelpath
cp ${groundpath}/image$(printf '%04d' $s).label $labelpath/" >> $seq/${j}w.sh

	echo "#!/bin/bash" > $seq/${s}w-b.sh
	echo "$scriptpath/${s}w-b.sh" >> $seq/${j}w.sh
	for (( i=$(($s-1)); i>=$1; i-- )) ; do
		echo "/usr/local/matlab/bin/matlab -nodisplay -nojvm -r \"addpath('matlab'); watershed_eval3($(($i+1)),$i); exit;\" ">> $seq/${s}w-b.sh
	done
	echo "#!/bin/bash" > $seq/${s}w-f.sh
	echo "$scriptpath/${s}w-f.sh" >> $seq/${j}w.sh
	for (( i=$(($s+1)); i<=$2; i++ )) ; do
		echo "/usr/local/matlab/bin/matlab -nodisplay -nojvm -r \"addpath('matlab'); watershed_eval3($(($i-1)),$i); exit;\" " >> $seq/${s}w-f.sh
	done

	echo "mv $labelpath $seqpath/$s " >> $seq/${j}w.sh

	#if [ "$((($s*2)%4))" == "0" ] ; then
		#echo "#!/bin/bash" > $j.sh
	#fi
done
