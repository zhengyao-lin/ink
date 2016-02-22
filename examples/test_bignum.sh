#! /bin/bash

BASEDIR=$(dirname $0)

time ink $BASEDIR/bignum.ink > out1
time python $BASEDIR/bignum.py > out2
time ruby $BASEDIR/bignum.rb > out3

echo ""

md5sum out1 out2 out3
rm out1 out2 out3