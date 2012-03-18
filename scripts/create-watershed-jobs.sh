#!/bin/bash

j=0
seqpath=/home/malloc47/src/projects/matsci/matscicut/seq1/watershed
labelpath=/home/malloc47/src/projects/matsci/matscicut/seq1/watershed/labels
groundpath=/home/malloc47/src/projects/matsci/matscicut/seq1/ground
scriptpath=/home/malloc47/src/projects/matsci/matscicut/scripts

echo "#!/bin/bash" > ${j}w.sh
for (( s=90; s<=100; s++ )) ; do

	echo "mkdir $labelpath
cp ${groundpath}/image$(printf '%04d' $s).label $labelpath/" >> ${j}w.sh

	echo "#!/bin/bash" > ${s}w-b.sh
	echo "/home/malloc47/src/projects/matsci/matscicut/scripts/${s}w-b.sh" >> ${j}w.sh
	for (( i=$(($s-1)); i>=90; i-- )) ; do
		echo "/usr/local/matlab/bin/matlab -nodisplay -nojvm -r \"addpath('matlab'); watershed_eval3($(($i+1)),$i); exit;\" ">> ${s}w-b.sh
	done
	echo "#!/bin/bash" > ${s}w-f.sh
	echo "/home/malloc47/src/projects/matsci/matscicut/scripts/${s}w-f.sh" >> ${j}w.sh
	for (( i=$(($s+1)); i<=100; i++ )) ; do
		echo "/usr/local/matlab/bin/matlab -nodisplay -nojvm -r \"addpath('matlab'); watershed_eval3($(($i-1)),$i); exit;\" " >> ${s}w-f.sh
	done

	echo "mv $labelpath $seqpath/$s " >> ${j}w.sh

	#if [ "$((($s*2)%4))" == "0" ] ; then
		#echo "#!/bin/bash" > $j.sh
	#fi
done
