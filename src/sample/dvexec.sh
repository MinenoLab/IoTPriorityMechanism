#!/bin/sh
if [ $# -ne 2 ]; then
  echo "error usage sinarionum devicenum" 1>&2
  exit 1
fi
md=`date +%m%d`

mypath=$(cd $(dirname $0) && pwd)

if [ $2 -le 2 ]; then
  ${mypath}/2015IotPriorityLimiter_tdevice 133.70.169.200 10${2} ${mypath}/DummyCameraDatas.txt jpg ${mypath}/DummyLCameraDatas.txt spg > ${mypath}/${md}result/result1${1}${2}.txt
else
  ${mypath}/2015IotPriorityLimiter_device 133.70.169.200 10${2} ${mypath}/DummySensorDatas.txt txt > ${mypath}/${md}result/result1${1}${2}.txt
fi

