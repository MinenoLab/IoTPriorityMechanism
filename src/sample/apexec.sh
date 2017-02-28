#!/bin/sh
if [ $# -ne 2 ]; then
  echo "error usage sinarionum scinariofilename" 1>&2
  exit 1
fi
md=`date +%m%d`

mypath=$(cd $(dirname $0) && pwd)

#./2015IotPriorityLimiter_brserver ${2} >${md}result/result1${1}br.txt &
${mypath}/2015IotPriorityLimiter_appserver >${mypath}/${md}result/result1${1}ap.txt

