#!/bin/bash


for (( s=90; s<=100; s++ )) ; do
	echo "#!/bin/bash" > scripts/$s-b.sh
	for (( i=$(($s-1)); i>=90; i-- )) ; do
		echo /home/malloc47/src/projects/matsci/matscicut/matscicut $(($i+1)) $i >> scripts/$s-b.sh
	done
	echo "#!/bin/bash" > scripts/$s-f.sh
	for (( i=$(($s+1)); i<=100; i++ )) ; do
		echo /home/malloc47/src/projects/matsci/matscicut/matscicut $(($i-1)) $i >> scripts/$s-f.sh
	done
done
