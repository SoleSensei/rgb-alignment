#!/bin/bash

#compile
make all
 
#create output dirs
mkdir ./test_pics/out
mkdir ./test_pics/out/align 
mkdir ./test_pics/out/mirror 
mkdir ./test_pics/out/gw
mkdir ./test_pics/out/contrast 
mkdir ./test_pics/out/sharp 
mkdir ./test_pics/out/median
mkdir ./test_pics/out/median/1 
mkdir ./test_pics/out/median/3
mkdir ./test_pics/out/median/10 
#====
mkdir ./test_pics/out/align/big 
mkdir ./test_pics/out/gw/big
mkdir ./test_pics/out/contrast/big 
mkdir ./test_pics/out/sharp/big
mkdir ./test_pics/out/median/1/big 
mkdir ./test_pics/out/median/3/big
mkdir ./test_pics/out/median/10/big

for N in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 
do
echo ----------- pic$N ------------
./build/bin/align ./test_pics/in/small/$N.bmp ./test_pics/out/align/$N.bmp --align
./build/bin/align ./test_pics/in/small/$N.bmp ./test_pics/out/mirror/$N.bmp --align --mirror

./build/bin/align ./test_pics/out/align/$N.bmp ./test_pics/out/gw/$N.bmp --gray-world
./build/bin/align ./test_pics/out/align/$N.bmp ./test_pics/out/contrast/$N.bmp --autocontrast 0.2
./build/bin/align ./test_pics/out/align/$N.bmp ./test_pics/out/sharp/$N.bmp --unsharp
./build/bin/align ./test_pics/out/align/$N.bmp ./test_pics/out/median/1/$N.bmp --median
./build/bin/align ./test_pics/out/align/$N.bmp ./test_pics/out/median/3/$N.bmp --median 3
./build/bin/align ./test_pics/out/align/$N.bmp ./test_pics/out/median/10/$N.bmp --median 10

done
#big pictures processing
for N in 1 2 3 4 5 6
do
echo ----------- big-pic$N ------------
./build/bin/align ./test_pics/in/big/$N.bmp ./test_pics/out/align/big/$N.bmp --align

./build/bin/align ./test_pics/out/align/big/$N.bmp ./test_pics/out/gw/big/$N.bmp --gray-world
./build/bin/align ./test_pics/out/align/big/$N.bmp ./test_pics/out/contrast/big/$N.bmp --autocontrast 0.2
./build/bin/align ./test_pics/out/align/big/$N.bmp ./test_pics/out/sharp/big/$N.bmp --unsharp
./build/bin/align ./test_pics/out/align/big/$N.bmp ./test_pics/out/median/1/big/$N.bmp --median
./build/bin/align ./test_pics/out/align/big/$N.bmp ./test_pics/out/median/3/big/$N.bmp --median 3
./build/bin/align ./test_pics/out/align/big/$N.bmp ./test_pics/out/median/10/big/$N.bmp --median 10

done

#tar -cf ./test_pics/solepics.tar ./test_pics
#make clean