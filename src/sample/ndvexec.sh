#!/bin/sh
if [ $# -ne 3 ]; then
  echo "error usage sinarionum devicenum jsonfilename" 1>&2
  exit 1
fi
md=`date +%m%d`

mypath=$(cd $(dirname $0) && pwd)

if [ $2 -le 2 ]; then
  ${mypath}/2015IotPriorityLimiter_tnormaldevice 133.70.169.200 10${2} ${mypath}/DummyCameraDatas.txt jpg ${mypath}/DummyLCameraDatas.txt spg ${mypath}/${3}> ${mypath}/${md}result/result1${1}${2}_n.txt
else
  ${mypath}/2015IotPriorityLimiter_normaldevice 133.70.169.200 10${2} ${mypath}/DummySensorDatas.txt txt  ${mypath}/${3}> ${mypath}/${md}result/result1${1}${2}_n.txt
fi
