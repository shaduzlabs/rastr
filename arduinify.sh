#!/bin/bash
set -x
rm -f $2/rastr-arduino.zip
rm -rf $2/arduino/*
mkdir -p $2/arduino/rastr/rastr
mkdir -p $2/arduino/rastr/Examples
cp $1/inc/rastr/* $2/arduino/rastr/rastr/.
cp $1/inc/rastr/fonts/* $2/arduino/rastr/.
cp -r $1/src/* $2/arduino/rastr/
cp -r $1/support/arduino/Examples/*  $2/arduino/rastr/Examples/
cp $1/support/arduino/README.md  $2/arduino/rastr/
cp $2/library.properties  $2/arduino/rastr/.
cp $2/rastr.h  $2/arduino/rastr/.
sed -n '/#include <rastr\/DynamicCanvas.hpp>/!p' $2/rastr.h  > $2/arduino/rastr/rastr.h
rm $2/arduino/rastr/rastr/DynamicCanvas.hpp
find $2/arduino/rastr -type f -name *.cpp -print0 | xargs -0 -I%%% mv %%% $2/arduino/rastr/
zip -9 -r $2/rastr-arduino.zip $2/arduino
