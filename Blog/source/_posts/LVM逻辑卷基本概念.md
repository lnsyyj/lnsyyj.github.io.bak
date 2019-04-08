---
title: LVM逻辑卷基本概念
date: 2019-04-08 10:06:35
tags: LVM
---

LinuxCast视频教程笔记

# 传统磁盘管理的问题

当分区大小不够用时无法扩展其大小，只能通过添加硬盘、创建新的分区来扩展空间，但是新添加进来的硬盘是作为独立文件系统存在的，原有的文件系统并未得到扩充，上层应用很多时候只能访问一个文件系统。只能让现有磁盘下线，换上新的磁盘之后，再将原始数据导入。

# LVM

LVM（Logical volume Manager）逻辑卷管理通过将底层物理硬盘抽象封装起来，以逻辑卷的形式表现给上层系统，逻辑卷的大小可以动态调整，而且不会丢失现有数据。新加入的硬盘也不会改变现有上层的逻辑卷。

作为一种动态磁盘管理机制，逻辑卷技术大大提高了磁盘管理的灵活性。

![](<https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/Blog/source/_posts/LVM%E9%80%BB%E8%BE%91%E5%8D%B7%E5%9F%BA%E6%9C%AC%E6%A6%82%E5%BF%B5/LVM-1.png>)

上图黄色为：VG    橙色为：LV

（1）首先把物理硬盘格式化（存储里面叫条带化）为物理卷（PV），格式化为物理卷的过程实际上是把硬盘空间化成一个一个的PE（PE是逻辑卷空间管理的最基本单位，默认4M）

（2）第二步我们要创建一个VG，VG的作用是用来装PE的，就像一个空间池。我们可以把一个或者多个PV加到VG当中。加入多少个PV，我们的VG容量就是这些PV大小之和。（当创建VG以后，在/dev目录下会多出一个目录）

（3）最后创建LV（每个LV的空间可能来自不同的物理硬盘）

![](<https://github.com/lnsyyj/lnsyyj.github.io/blob/hexo/Blog/source/_posts/LVM%E9%80%BB%E8%BE%91%E5%8D%B7%E5%9F%BA%E6%9C%AC%E6%A6%82%E5%BF%B5/LVM-2.png?raw=true>)

# 创建LVM

