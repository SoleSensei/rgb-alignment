#!/bin/bash

for N in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 
do
echo ----------- pic$N ------------
./build/bin/align ../small/$N.bmp ../test/align/$N.bmp --align
./build/bin/align ../small/$N.bmp ../test/mirror/$N.bmp --align --mirror

./build/bin/align ../test/align/$N.bmp ../test/gw/$N.bmp --gray-world
./build/bin/align ../test/align/$N.bmp ../test/contrast/$N.bmp --autocontrast 0.2
./build/bin/align ../test/align/$N.bmp ../test/sharp/$N.bmp --unsharp
./build/bin/align ../test/align/$N.bmp ../test/median/1/$N.bmp --median
./build/bin/align ../test/align/$N.bmp ../test/median/3/$N.bmp --median 3
./build/bin/align ../test/align/$N.bmp ../test/median/10/$N.bmp --median 10

done

tar -cf ../test/solepics.tar ../test