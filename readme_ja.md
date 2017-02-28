# IoT Priority Mechanism
## 概要
このソースは、峰野研究室の「IoT向け優先度制御プラットフォーム」のプロトタイプです。
## 動作要件
+ サーバ側：Debian系Linux (制作環境 Ubuntu 14.04LTS)
+ IoTデバイス側：Raspbian Jessie他Debian系Linux (制作環境 Ubuntu 14.04LTS)
+ GCC 4.8以上
+ Boost 1.55以上
+ Kazuho Oku氏制作の[picojson.h](https://github.com/kazuho/picojson/)
## 構造
```
| readme.md
└─src
   ├─core
   │  ├─APState #アプリケーションサーバの動作
   │  ├─BrokerState #ブローカーサーバでの動作
   │  ├─DeviceState #IoTデバイスでの動作
   │  ├─structure #各種データ構造
   │  └─utils  #通信などのユーティリティモジュール
   └─sample #サンプルプログラム
```

## 実行方法
sampleプログラム…APサーバにIoTデバイスが1分に1回ダミーデータを送信するプログラムです。
以下にsampleディレクトリ内にあるサンプルプログラムを実行する方法を示します。
1. [picojson.h](https://github.com/kazuho/picojson/)をutilsディレクトリに配置
1. サーバ側コンパイル
```sh
$ cd ./src/sample
$ make -f makefile_br
$ make -f makefile_ap
```
1. IoTデバイス側コンパイル
```sh
$ cd ./src/sample
$ make -f makefile_dv
```
1. サーバサイドのプログラム実行
```sh
$ ./2015IotPriorityLimiter_brserver PRIORITY_REQUIRE_FILE
$ ./2015IotPriorityLimiter_appserver
```
※PRIORITY_REQUIRE…優先度要求を記したJSONファイル sy_*.jsonがサンプルとして存在します。
1. IoTデバイスのプログラム実行
```sh
$ ./2015IotPriorityLimiter_device SERVER_IP DEVICE_ID SENDDATA DATA_TYPE
```
※　SERVER_IP…brokerサーバ、APサーバのIP(IPv4)  
  DEVICE_ID…1~254の任意の数字  
  SENDDATA…使用するダミーデータ、拡張子は任意  
  DATA_TYPE…データの種類を規定する。任意の文字列

## 諸注意
本プログラムに起因する一切の責任は負いません。またサポート等も行えません。