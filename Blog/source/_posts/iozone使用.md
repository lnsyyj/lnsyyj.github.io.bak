---
title: iozone使用
date: 2018-05-09 17:22:27
tags: Golang
---

### yum安装方法

1、安装

```
yum install -y iozone
```

2、参数

```
iozone -l 1 -u 1 -r 16k -s 64g -F
```

-l是最小进程数量lower  

-u是最大进程数量upper

-r是读写的基本单位，16k作为读写的基本单位，根据模拟应用程序进行合理设置（目的是模拟真实应用）

-s指定默认读写的大小，建议不要指定的太小，一般指定的是内存的2倍

-F指定测试文件位置，可以是多个



### 编译安装方法

```
yum install gcc
wget http://www.iozone.org/src/current/iozone3_471.tar
tar xvf iozone3_471.tar
cd iozone3_471/src/current/

ARM平台：
make linux-arm
X86平台：
make linux-AMD64

cp iozone /usr/bin/
./iozone -+u -+d -+p -+t -z -l 1 -u 1 -r 4k -s 1G -F /home/node-3/1-FILE
```