#!/bin/sh
cmd=/bin/ls
declare -a args=('-l' '-a' '-R' '-c' '-f' '-g' '-itr' '-H' '-i' '-m' '-d' '-n' '-N' '-q' '-r' '-s' '-k' '-U')
cd ~/Desktop/syssec

for(( i = 0; i <= 19; i++ ))
do
	for (( j = $i+1 ; j <= 19; j++ ))
	do
		./systrace.out $cmd ${args[$i]} ${args[$j]}
	done
done

for(( i = 0; i <= 18; i++ ))
do
	for (( j = $i+1 ; j <= 18; j++ ))
	do
		for (( k = $j+1; k <= 18; k++))
		do
			./systrace.out $cmd ${args[$i]} ${args[$j]} ${args[$k]}
		done
	done
done

for(( i = 0; i <= 19; i++ ))
do
	./systrace.out $cmd ${args[$i]}
done

