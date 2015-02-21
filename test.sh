#!/bin/sh
IMAGE="/home/denvorTHINK/Desktop/Droid4/makeLinux/workspace/boot.img"
EXE="./GetImageInfo"
echo "Test Wrong parameter"
$EXE
$EXE 1 2 3
echo "Test Unreached file"
$EXE foo
echo "Test make file"
./GetImageInfo main.c
echo "Test Real file"
$EXE $IMAGE
