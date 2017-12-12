#!/system/bin/sh

output=$1
echo "stack info" > $output
filelist=`ls /proc`
for file in $filelist
do
    if [[ $file != *[!0-9]* ]]; then
        cat "/proc/$file/comm" >> $output
        threadlist=`ls /proc/$file/task`
        for thread in $threadlist
        do
            cat "/proc/$file/task/$thread/comm" >> $output
            cat "/proc/$file/task/$thread/stack" >> $output
        done
    fi
done
