#!/bin/bash



#stty -F /dev/ttyUSB1 speed 115200 cs8 -cstopb -parenb -echo
count=1
while [ 1 ]
do
echo -n -e \\xA8\\x38\\x7E\\x64\\x38\\x38\\x7E\\x64\\x38\\x38\\x7E\\x64\\x38\\x38\\x7E\\x64\\x38\\x86\\xF2\\xD1\\x45 > /dev/ttyUSB1
#echo  -e '\xA8x36' > /dev/ttyUSB1
#echo -n -e \\x38 > /dev/ttyUSB1
#echo -n "kdjkfuhndkjfdlxjfydh" > /dev/ttyUSB1
#echo -n -e \\xA8\\x38 > /dev/ttyUSB1
crc32  <(echo  -n -e \\xA8\\x38\\x7E\\x64\\x38\\x38\\x7E\\x64\\x38\\x38\\x7E\\x64\\x38\\x38\\x7E\\x64\\x38) > /dev/ttyUSB1
echo $count
(( count++ ))
sleep 1
done
#100110000010001110110110111
#4C11DB7