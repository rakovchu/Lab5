#!/bin/sh

make || exit 1

echo
echo "inserting module..."
sudo insmod timer.ko || exit 1
sleep 3
echo "removing module..."
sudo rmmod timer.ko || exit 1

echo "dmesg:"
dmesg | tail
