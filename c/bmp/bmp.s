#/bin/bash
for arg in `ls *.bmp`
do
	bmpview $arg
	sleep 2
done
