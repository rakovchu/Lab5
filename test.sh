#!/bin/sh

make || exit 1

echo
echo "inserting module"
sudo insmod timer.ko || exit 1
echo -n "read /sys/timer/interval: "
cat /sys/timer/interval
echo "waiting 5 s..."
sleep 5
echo "write 3 to /sys/timer/interval"
echo 3 > /sys/timer/interval
echo "waiting 8 s..."
sleep 8
echo -n "read /sys/timer/interval: "
cat /sys/timer/interval
echo "removing module"
sudo rmmod timer.ko || exit 1

echo "dmesg:"
dmesg | tail
