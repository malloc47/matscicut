#!/bin/bash

j=0
echo "#!/bin/bash" > $j.sh
for (( s=90; s<=100; s++ )) ; do
	echo "#!/bin/bash" > $s-b.sh
	echo "/home/malloc47/src/projects/matsci/matscicut/scripts/$s-b.sh" >> $j.sh
	for (( i=$(($s-1)); i>=90; i-- )) ; do
		echo /home/malloc47/src/projects/matsci/matscicut/matscicut $(($i+1)) $i >> $s-b.sh
	done
	echo "#!/bin/bash" > $s-f.sh
	echo "/home/malloc47/src/projects/matsci/matscicut/scripts/$s-f.sh" >> $j.sh
	for (( i=$(($s+1)); i<=100; i++ )) ; do
		echo /home/malloc47/src/projects/matsci/matscicut/matscicut $(($i-1)) $i >> $s-f.sh
	done

	#if [ "$((($s*2)%4))" == "0" ] ; then
		#echo "#!/bin/bash" > $j.sh
	#fi
done
