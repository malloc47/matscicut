#!/bin/bash

j=0
seq=$3
fullpath=/home/malloc47/src/projects/matsci/matsciskel

source $seq.sh

c=0
echo "#!/bin/bash" > $seq/$j.sh
for (( s=$1; s<=$2; s++ )) ; do
	echo "#!/bin/bash" > $seq/$s-b.sh
	echo "mkdir $seq/$s" >> $seq/$j.sh
	echo "$(pwd)/$seq/$s-b.sh" >> $seq/$j.sh
	for (( i=$(($s-1)); i>=$1; i-- )) ; do
		if [ "$i" == "$(($s-1))" ] ; then
			exeg $(($i+1)) $i $seq $s b
		else
			exe $(($i+1)) $i $seq $s b
		fi
	done
	echo "#!/bin/bash" > $seq/$s-f.sh
	echo "$(pwd)/$seq/$s-f.sh" >> $seq/$j.sh
	for (( i=$(($s+1)); i<=$2; i++ )) ; do
		if [ "$i" == "$(($s+1))" ] ; then
			exeg $(($i-1)) $i $seq $s f
		else
			exe $(($i-1)) $i $seq $s f
		fi
	done
	c=$((c+1))

	if [ $c -ge $4 ] ; then
		c=0	
		j=$((j+1))
		echo "#!/bin/bash" > $seq/$j.sh
	fi
done
