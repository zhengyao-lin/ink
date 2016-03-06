#! /bin/bash

BASEDIR=$(dirname $0)

echo "ink:"
time ink $BASEDIR/bignum.ink > out1
echo -e "\npython:"
time python $BASEDIR/bignum.py > out2
echo -e "\nruby:"
time ruby $BASEDIR/bignum.rb > out3

echo ""

md5sum out1 out2 out3
rm out1 out2 out3