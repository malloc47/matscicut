exe () {
	fullpath=/home/malloc47/src/projects/matsci/matsciskel
	echo $fullpath/matsciskel.py e global $3/img/image`printf '%04d' $1`.png $3/$4/image`printf '%04d' $1`.label $3/img/image`printf '%04d' $2`.png $3/$4/image`printf '%04d' $2`.label $3/$4/image`printf '%04d' $2`.png 20 >> $3/$4-$5.sh
}

exeg () {
	fullpath=/home/malloc47/src/projects/matsci/matsciskel
	echo $fullpath/matsciskel.py e global $3/img/image`printf '%04d' $1`.png $3/ground/image`printf '%04d' $1`.label $3/img/image`printf '%04d' $2`.png $3/$4/image`printf '%04d' $2`.label $3/$4/image`printf '%04d' $2`.png 20 >> $3/$4-$5.sh
}
