# IoT Priority Mechanism
## About this repository
This repository is an implementation of our paper, [Implementing and Evaluating Priority Control Mechanism for Heterogeneous Remote Monitoring IoT System](http://dl.acm.org/citation.cfm?id=3004040)
## Operational requirements
+ Server Side：Debian Linux (Work environment is Ubuntu 14.04LTS)
+ Device Side：Raspbian Jessie and other Debian Linux (Work environment is Ubuntu 14.04LTS)
+ GCC 4.8 or higher
+ Boost 1.55 or higher
+ [picojson.h](https://github.com/kazuho/picojson/) (Created by Kazuho Oku)

## constructions
```
| readme.md
└─src
   ├─core
   │  ├─APState # Behavior in Aplication Server
   │  ├─BrokerState # Behavior in Broker Server
   │  ├─DeviceState # Behavior in IoT devices
   │  ├─structure # Data structures
   │  └─utils  # Utility modules such as telecommunication modules
   └─sample # Sample programs
```

## Usage
This chapter using sample programs
Sample programs that are IoT devices try to send dummy data to Aplication Server once per a minute.
how to run as follows.
1. Locate [picojson.h](https://github.com/kazuho/picojson/) to "utils" directory.   

1. Compile in servers
```sh
$ cd ./src/sample
$ make -f makefile_br
$ make -f makefile_ap
```
1. Compile in IoT devices
```sh
$ cd ./src/sample
$ make -f makefile_dv
```
1. Run servers
```sh
$ ./2015IotPriorityLimiter_brserver PRIORITY_REQUIRE_FILE
$ ./2015IotPriorityLimiter_appserver
```
 `ATTEPRIORITY_REQUIRE` : JSON files that described priority requirements are exists such as sy_*.json .  

1. Run IoT devices
```sh
$ ./2015IotPriorityLimiter_device SERVER_IP DEVICE_ID SENDDATA DATA_TYPE
```
`SERVER_IP` : IP address (Ipv4) of the broker server and the applictaion server.   
`DEVICE_ID` : Any numbers in 1 to 254.  
`SENDDATA` : Any dummy data.  
`DATA_TYPE` : Type of `SENDDATA`. Any charcters.

## NOTE
We take all possible measures to ensure about this repository. However, if this source code has any bugs, we do not take any responsibility and any supports.
