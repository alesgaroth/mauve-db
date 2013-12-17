#!/bin/bash

fails=0
tries=0
if [ -n "$1" ] ;then
	CMD=$1
else
	CMD=../src/mauve
fi


for file in [ft][ae][si][tl]*; do 
	tries=$((tries + 1))
	echo $file:
	case $file in
		
		t*.sh) ./$file;
			if [ $? -ne 0 ]; then
				echo $file should have returned zero. FAIL
				fails=$((fails + 1))
			fi
			;;
		f*.sh) ./$file;
			if [ $? -eq 0 ]; then
				echo $file should have returned non-zero. FAIL
				fails=$((fails + 1))
			fi
			;;
		t*.sql)
			$CMD -uroot -ppassword < $file
			if [ $? -ne 0 ]; then
				echo $file should have returned zero. FAIL
				fails=$((fails + 1))
			fi
			;;
		f*.sql)
			$CMD -uroot -ppassword < $file
			if [ $? -eq 0 ]; then
				echo $file should have returned non-zero. FAIL
				fails=$((fails + 1))
			fi
			;;
		t*.c)
			exec=$(basename $file .c).exe
			rm -f $exec
			make $exec
			echo ./$exec
			./$exec
			if [ $? -ne 0 ]; then
				echo $file should have returned zero. FAIL
				fails=$((fails + 1))
			fi
			;;
		f*.c)
			exec=$(basename $file .c).exe
			rm -f $exec
			make $exec
			echo ./$exec
			./$exec
			if [ $? -eq 0 ]; then
				echo $file should have returned non-zero. FAIL
				fails=$((fails + 1))
			fi
			;;
		*.exe|*.o)  # ignore
				tries=$((tries - 1))
			;;
		*)
			echo "don't know $file"
			fails=$((fails + 1))
			;;
	esac
done
if [ $fails -ne 0 ]; then
echo failed $fails of $tries
else
	echo all $tries tests passed
fi
exit $fails
