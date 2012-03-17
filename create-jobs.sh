#!/bin/bash

j=0
echo "#!/bin/bash" > scripts/$j.sh
for (( s=90; s<=100; s++ )) ; do
	echo "#!/bin/bash" > scripts/$s-b.sh
	echo "/home/malloc47/src/projects/matsci/matscicut/scripts/$s-b.sh" >> scripts/$j.sh
	for (( i=$(($s-1)); i>=90; i-- )) ; do
		echo /home/malloc47/src/projects/matsci/matscicut/matscicut $(($i+1)) $i >> scripts/$s-b.sh
	done
	echo "#!/bin/bash" > scripts/$s-f.sh
	echo "/home/malloc47/src/projects/matsci/matscicut/scripts/$s-f.sh" >> scripts/$j.sh
	for (( i=$(($s+1)); i<=100; i++ )) ; do
		echo /home/malloc47/src/projects/matsci/matscicut/matscicut $(($i-1)) $i >> scripts/$s-f.sh
	done

	#if [ "$((($s*2)%4))" == "0" ] ; then
		#echo "#!/bin/bash" > scripts/$j.sh
	#fi
done
