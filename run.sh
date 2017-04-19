#!/bin/bash
g++ player-prg.cpp -std=c++11 -DAP_PLAYER
mv a.out ap_player
g++ player-prg.cpp -std=c++11 -DRND_PLAYER
mv a.out rnd_player
g++ player-prg.cpp -std=c++11 -DARB
mv a.out arb
if [ ! -d data ]
then
    mkdir data
fi
pike pfsnntp.pike -f ./ap_player -s ./rnd_player -a ./arb -b 1 -n 10 -v 1

