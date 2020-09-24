#!/bin/sh

mkdir transmit2cca/current
mv results transmit2cca/current

cat trace.txt | grep -E "(NID=1.*STATE=ccaa$|NID=2.*STATE=ccab$)" > transmit2cca/current/cca.txt

python3 compute-transmit2cca-length.py 0 transmit2cca/current/cca.txt > transmit2cca/current/t2c-evol.data
python3 compute-transmit2cca-length.py 10 transmit2cca/current/cca.txt > transmit2cca/current/t2c-bin10.data
python3 compute-transmit2cca-length.py 100 transmit2cca/current/cca.txt > transmit2cca/current/t2c-bin100.data

