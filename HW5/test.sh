#!/bin/bash

if [ $# == 1 ]
then
    cp $1 test.txt
    ./astar test.txt
else
    echo "Usage: ./test.sh <testcase>"
fi
