---
title: vdbench 文件系统测试
date: 2018-04-12 18:49:34
tags:
---


## Centos 7 安装vdbench

```
1、首先安装Java JDK和一些工具包，这里使用的是java-1.7.0
[root@cephL ~]# sudo yum install -y java-1.7.0-openjdk java-1.7.0-openjdk-devel unzip
[root@cephL ~]# mkdir vdbench && cd vdbench
2、把下载后的vdbench拷贝到该目录并解压，这里使用的是vdbench50406
[root@cephL vdbench]# unzip vdbench50406.zip
3、测试是否可以运行
[root@cephL vdbench]# ./vdbench -t
```

## 运行测试
```
1、准备测试目录
[root@cephL node-1]# pwd
/root/node-1
2、到vdbench目录准备配置文件
[root@cephL ~]# cd /root/vdbench/
[root@cephL vdbench]# vi filesystem.conf
fsd=fsd1,anchor=/root/node-1,depth=2,width=2,files=2,size=128k
fwd=fwd1,fsd=fsd1,operation=read,xfersize=4k,fileio=sequential,fileselect=random,threads=2
rd=rd1,fwd=fwd1,fwdrate=max,format=yes,elapsed=10,interval=1

该配置文件表示：
第一行：
fsd、fwd、rd：是唯一标识
depth：从/root/node-1目录开始，在其中创建2层深度的目录（深度）
width：从/root/node-1目录开始，每层目录创建2个平级目录（广度）
files：在使用depth和width创建的目录中，最深层每个目录创建2个文件
size：每个文件大小为128k
openflags：
第二行：
operation：值为read，表示每个线程，根据fileselect的值（这里是随机）选择一个文件后，打开该文件进行读取
xfersize：连续读取4k blocks(xfersize=4k)直到文件结束(size=128k)，关闭文件并随机选择另一个文件
fileio：表示文件IO的方式，random或者sequential
fileselect：值为random，表示每个线程随机选择一个文件
threads：值为2，表示启动2个线程（线程默认值为1）
第三行：
fwdrate：每秒有多少file system operations，max为无限制
format：值为yes，表示创建完整的目录结构，包括所有文件初始化到所需的128k大小
elapsed：持续运行时间，默认设置为30（以秒为单位）。注意：至少是interval的2倍，
interval：该参数指定每个报告间隔时间（以秒为单位）

3、开始一个简单的测试
[root@cephL vdbench]# ./vdbench -f filesystem.conf
4、查看被测目录中生成的测试文件
[root@cephL ~]# tree /root/node-1/ -h
/root/node-1/                         --- depth 0
├── [  68]  no_dismount.txt
├── [  44]  vdb.1_1.dir               --- depth 1  width 1
│   ├── [  50]  vdb.2_1.dir           --- depth 2  width 1
│   │   ├── [128K]  vdb_f0001.file    --- depth 2  width 1  files 1
│   │   └── [128K]  vdb_f0002.file    --- depth 2  width 1  files 2
│   └── [  50]  vdb.2_2.dir           --- depth 2  width 2
│       ├── [128K]  vdb_f0001.file    --- depth 2  width 2  files 1
│       └── [128K]  vdb_f0002.file    --- depth 2  width 2  files 2
├── [  44]  vdb.1_2.dir               --- depth 1  width 2
│   ├── [  50]  vdb.2_1.dir           --- depth 2  width 1
│   │   ├── [128K]  vdb_f0001.file    --- depth 2  width 1  files 1
│   │   └── [128K]  vdb_f0002.file    --- depth 2  width 1  files 2
│   └── [  50]  vdb.2_2.dir           --- depth 2  width 2
│       ├── [128K]  vdb_f0001.file    --- depth 2  width 2  files 1
│       └── [128K]  vdb_f0002.file    --- depth 2  width 2  files 2
└── [ 159]  vdb_control.file

6 directories, 10 files
```

## 其他常用参数

openflags
```
这个参数可以指定在SD, WD, FSD, FWD, RD中
可用
```

sizes（注意这里有s）
```
这个参数可以指定在FSD中
该参数指定文件的大小（size），可以指定一个或一组文件的大小（size）。
例如：sizes=(32k,50,64k,50)，其中第一个数字表示文件大小（size），第二个数字表示必须具有此大小（size）的文件的百分比。如果指定一组文件，百分比加起来必须为100。
当您指定sizes=(nnn,0)时，vdbench将创建平均大小（size）为'nnn'字节的文件

以上标出（size）可能与下面的规则有关，所以保留原文。以下规则个人理解为，如果大于某个size规则（如果有小数点），那么必须是某个规则的倍数。
有一些规则与最终使用的文件大小（size）有关：
如果size > 10m，size将为1m的倍数。（个人理解，size为10.1m是不行的，必须是1m的倍数）
如果size > 1m，size将为100k的倍数。
如果size > 100k，size将为10k的倍数。
如果size < 100k，size将为1k的倍数。

关于百分比的实验：
实验1
在8个文件中，4个1k的文件，4个2k的文件
[root@cephL vdbench]# vi filesystem.conf
fsd=fsd1,anchor=/root/node-1,depth=2,width=2,files=2,sizes=(1k,50,2k,50)
fwd=fwd1,fsd=fsd1,operation=read,xfersize=4k,fileio=sequential,fileselect=random,threads=2
rd=rd1,fwd=fwd1,fwdrate=max,format=yes,elapsed=10,interval=1

[root@cephL vdbench]# ./vdbench -f filesystem.conf
[root@cephL ~]# tree node-1/ -h
node-1/
├── [  68]  no_dismount.txt
├── [  44]  vdb.1_1.dir
│   ├── [  50]  vdb.2_1.dir
│   │   ├── [2.0K]  vdb_f0001.file
│   │   └── [1.0K]  vdb_f0002.file
│   └── [  50]  vdb.2_2.dir
│       ├── [1.0K]  vdb_f0001.file
│       └── [2.0K]  vdb_f0002.file
├── [  44]  vdb.1_2.dir
│   ├── [  50]  vdb.2_1.dir
│   │   ├── [1.0K]  vdb_f0001.file
│   │   └── [1.0K]  vdb_f0002.file
│   └── [  50]  vdb.2_2.dir
│       ├── [2.0K]  vdb_f0001.file
│       └── [2.0K]  vdb_f0002.file
└── [ 194]  vdb_control.file

6 directories, 10 files

实验2
在8个文件中，1个1k的文件，3个2k的文件，3个3k的文件，1个4k的文件。不知道为什么。。。。。。。。
[root@cephL vdbench]# vi filesystem.conf
fsd=fsd1,anchor=/root/node-1,depth=2,width=2,files=2,sizes=(1k,25,2k,25,3k,25,4k,25)
fwd=fwd1,fsd=fsd1,operation=read,xfersize=4k,fileio=sequential,fileselect=random,threads=2
rd=rd1,fwd=fwd1,fwdrate=max,format=yes,elapsed=10,interval=1

[root@cephL vdbench]# ./vdbench -f filesystem.conf
[root@cephL ~]# tree node-1/ -h
node-1/
├── [  68]  no_dismount.txt
├── [  44]  vdb.1_1.dir
│   ├── [  50]  vdb.2_1.dir
│   │   ├── [3.0K]  vdb_f0001.file
│   │   └── [2.0K]  vdb_f0002.file
│   └── [  50]  vdb.2_2.dir
│       ├── [1.0K]  vdb_f0001.file
│       └── [3.0K]  vdb_f0002.file
├── [  44]  vdb.1_2.dir
│   ├── [  50]  vdb.2_1.dir
│   │   ├── [2.0K]  vdb_f0001.file
│   │   └── [2.0K]  vdb_f0002.file
│   └── [  50]  vdb.2_2.dir
│       ├── [4.0K]  vdb_f0001.file
│       └── [3.0K]  vdb_f0002.file
└── [ 234]  vdb_control.file

6 directories, 10 files
```