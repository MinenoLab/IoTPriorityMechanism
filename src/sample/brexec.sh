#!/bin/sh
if [ $# -ne 2 ]; then
  echo "error usage sinarionum scinariofilename" 1>&2
  exit 1
fi
md=`date +%m%d`

mypath=$(cd $(dirname $0) && pwd)

${mypath}/2015IotPriorityLimiter_brserver ${mypath}/${2} >${mypath}/${md}result/result1${1}br.txt
#./2015IotPriorityLimiter_appserver >${md}result/result1${1}ap.txt &
