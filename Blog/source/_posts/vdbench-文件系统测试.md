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

# 多机测试

应用场景为多个NFS Client挂在相同NFS Server的读写。

```
hd=default,vdbench=/home/vdbench,user=root,shell=ssh
hd=hd1,system=node1
hd=hd2,system=node2
hd=hd3,system=node3

fsd=fsd1,anchor=/mnt/test863,depth=1,width=10,files=10000,size=20m,shared=yes
fwd=format,threads=6,xfersize=1m
fwd=default,xfersize=1m,fileio=random,fileselect=random,rdpct=100,threads=6
fwd=fwd1,fsd=fsd1,host=hd1
fwd=fwd2,fsd=fsd1,host=hd2
fwd=fwd3,fsd=fsd1,host=hd3

rd=rd1,fwd=fwd*,fwdrate=max,format=(restart,only),elapsed=600,interval=1
```

参数解析：

有3台测试节点node1、node2、node3。每台测试节点的/home/vdbench/目录都存在可执行vdbench二进制文件（位置必须相同），使用root用户通过ssh方式连接（节点间需要做ssh免密），每台测试节点的测试目录为/mnt/test863，目录深度为1，最深层目录中的目录宽度为10，最深层每个目录中有10000个文件，每个文件大小20mb

`关于shared=yes`

随着Vdbench运行多个slaves和可选的多个hosts，slaves和hosts之间关于文件状态的通信变得困难。使所有这些slaves设备相互通信所涉及的开销变得过于昂贵。您不希望一个slave删除另一个slave当前正在读取或写入的文件。因此，Vdbench不允许您跨slaves和hosts共享FSD。

当然，在你开始使用庞大的文件系统之前，这一切听起来都很棒。 您刚刚填满了500 TB的磁盘文件，然后您决定要与一个或多个远程主机共享该数据。 从头开始重新创建整个文件结构需要很长时间。 该怎么办？

指定'shared = yes'时，Vdbench将允许您共享文件系统定义（FSD）。 它通过允许每个slave设备仅使用FSD文件结构中定义的每个“第n”文件来实现这一点，其中“n”等于slave数量。（It does this by allowing each slave to use only every ‘nth’ file as is defined in the FSD file structure, where ‘n’ equals the amount of slaves.）

这意味着不同的host不会互相踩到脚趾，但有一个例外：当您指定'format = yes'时，Vdbench首先删除已存在的文件结构。由于这可能是一个旧的文件结构，Vdbench无法传播文件删除周围，让每个slave删除他的'第n'文件。因此，每个slave设备都会尝试删除所有文件，但如果删除失败则不会生成错误消息（因为不同的slave设备只是删除了它）。这些失败的删除将被计算并报告在“Miscellaneous statistics”中的“FILE_DELETE_SHARED”计数器下。但是，“FILE_DELETES”计数器可以包含高于存在的文件数量的计数。我已经看到多个slaves设备能够同时删除同一个文件而没有操作系统将任何错误传递给Java的情况。

`关于rdpct（Read Percentage）`

此参数允许您混合读取和写入。 使用operation=read只允许你做read，operation=write只允许你做write。 但是，指定rdpct=，您将能够在同一个选定文件中混合读取和写入。请注意，对于sequential（顺序），这没有多大意义。您可以以读取块1，写入块2，读取块3等。对于随机I/O，这非常有意义。

`关于format=`

- no

  不需要任何格式,但现有文件结构必须与 FSD 定义的结构相匹配。

- yes

  Vdbench 将首先删除当前文件结构,然后再次创建文件结构。然后,它将执行您在 RD 中的请求。

- restart

  Vdbench将仅创建尚未创建的文件，并且还将扩展未达到其正确大小的文件。 （这是totalsize和workingsetsize可以发挥作用的地方）。

- only

  与'yes'相同，但Vdbench不会执行当前的RD。

- dir(ectories)

  与‘yes’相同，但它只会创建目录。

- clean

  Vdbench只会删除当前的文件结构，而不会执行当前的RD。

- once

  这将覆盖每个forxxx参数循环完成format的默认行为。

- limited

  format将在elapsed=seconds之后终止，而不是所有文件或为totalsize=选择的文件已格式化之后终止。

- complete

  只能与'format=no'一起使用，并且会告诉Vdbench format已经完成，但是Vdbench不应该尝试通过目录搜索来验证每个目录和文件的状态。 当然，如果一个或多个目录或文件丢失或文件未达到其预期大小，结果不可预测。在测试期间删除或创建目录或文件时非常危险。