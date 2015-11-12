#!/bin/zsh

#  singleRunAnalyzer.sh
#  
#
#  Created by Brian L Dorney on 12/11/15.
# $1 -> input root file
# $2 -> name of tree containing data in $1
# $3 -> comma separated list of channel numbers e.g. 1,2,3,4
# $4 -> analysis configuration file
# $5 -> verbose mode (true/false)
#
#

g++ -g3 -c -O0 `root-config --cflags --glibs --libs` -std=c++11 singleRunAnalyzer.cpp -I../include
g++ -o singleRunAnalyzer singleRunAnalyzer.o  `root-config --cflags --glibs --libs`
#./singleRunAnalyzer $1 $2 $3 $4 $5
