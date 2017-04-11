#!/bin/bash

LENGTH=200
T1=/test/test1
T2=/test/test2

for i in `seq 1 $LENGTH`
do
        echo "ONE x$i" >> $T1
        echo "THREE x$i" >> $T2
        echo "TWO x$i" >> $T1
        echo "FOUR x$i" >> $T2
done

B1=$(stat -c%s "$T1")
B2=$(stat -c%s "$T2")

echo "$T1 is now $(($LENGTH*2)) lines longer and $B1 bytes in size."
echo "$T2 is now $(($LENGTH*2)) lines longer and $B2 bytes in size."
