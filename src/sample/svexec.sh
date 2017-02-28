#!/bin/sh
if [ $# -ne 2 ]; then
  echo "error usage sinarionum scinariofilename" 1>&2
  exit 1
fi
md=`date +%m%d`

./2015IotPriorityLimiter_brserver ${2} >${md}result/result1${1}br.txt
./2015IotPriorityLimiter_appserver >${md}result/result1${1}ap.txt

