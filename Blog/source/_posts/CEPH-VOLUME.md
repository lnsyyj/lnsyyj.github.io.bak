---
title: CEPH-VOLUME
date: 2018-05-19 19:19:32
tags: CEPH-Luminous
---

# CEPH-VOLUME

使用pluggable（可插拔）工具将 OSDs 与 lvm 或physical disks等不同的device技术部署在一起（[lvm](http://docs.ceph.com/docs/master/ceph-volume/lvm/)本身被视为plugin）, 并尝试采用predictable、robust的方法来preparing、activating和starting OSDs。

[Overview](http://docs.ceph.com/docs/master/ceph-volume/intro/#ceph-volume-overview)|[Plugin Guide](http://docs.ceph.com/docs/master/dev/ceph-volume/plugins/#ceph-volume-plugins)

### Command Line Subcommands

目前支持`lvm`，以及已经使用`ceph-disk`部署的普通磁盘（带有GPT分区）。


- [lvm](http://docs.ceph.com/docs/master/ceph-volume/lvm/#ceph-volume-lvm)

- [simple](http://docs.ceph.com/docs/master/ceph-volume/simple/#ceph-volume-simple)

### Node inventory

[inventory](http://docs.ceph.com/docs/master/ceph-volume/inventory/#ceph-volume-inventory)子命令提供有关节点物理磁盘inventory（清单）的信息和元数据。

# MIGRATING

从Ceph版本13.0.0开始，不推荐使用ceph-disk。弃用警告中的link会链接到此页面。强烈建议用户开始使用ceph-volume。有两种迁移途径：

1、保留使用ceph-disk部署的OSD：[simple](http://docs.ceph.com/docs/master/ceph-volume/simple/#ceph-volume-simple) 命令在禁用ceph-disk触发的同时，提供了接管管理的方法。

2、用ceph-volume重新部署现存的OSD：详细信息请参照 [Replacing an OSD](http://docs.ceph.com/docs/master/rados/operations/add-or-rm-osds/#rados-replacing-an-osd)

有关为何删除ceph-disk的详细信息，请参阅[Why was ceph-disk replaced?](http://docs.ceph.com/docs/master/ceph-volume/intro/#ceph-disk-replaced) 章节。

### NEW DEPLOYMENTS

对于新的部署，推荐使用[lvm](http://docs.ceph.com/docs/master/ceph-volume/lvm/#ceph-volume-lvm)，它可以使用任何logical volume作为data OSD 的输入, 也可以从device中设置minimal/naive logical volume。

### EXISTING OSDS

如果群集已经具有ceph-disk提供的OSDs，则ceph-volume可以用[simple](http://docs.ceph.com/docs/master/ceph-volume/simple/#ceph-volume-simple)的方式接管。在data device或OSD directory上完成扫描，并且ceph-disk被完全禁用。完全支持加密。



# LVM

实现此功能需要通过lvm子命令来部署OSDs：`ceph-volume lvm`

### Command Line Subcommands

[prepare](http://docs.ceph.com/docs/master/ceph-volume/lvm/prepare/#ceph-volume-lvm-prepare)
[activate](http://docs.ceph.com/docs/master/ceph-volume/lvm/activate/#ceph-volume-lvm-activate)
[create](http://docs.ceph.com/docs/master/ceph-volume/lvm/create/#ceph-volume-lvm-create)
[list](http://docs.ceph.com/docs/master/ceph-volume/lvm/list/#ceph-volume-lvm-list)

### Internal functionality

lvm子命令的其他部分是internal的，不向用户公开，这些部分解释了如何协同工作，阐明了工具的工作流程。

[Systemd Units](http://docs.ceph.com/docs/master/ceph-volume/lvm/systemd/#ceph-volume-lvm-systemd) | [lvm](http://docs.ceph.com/docs/master/dev/ceph-volume/lvm/#ceph-volume-lvm-api)



# SIMPLE

实现此功能需要通过simple子命令来管理OSD：`ceph-volume simple`

### Command Line Subcommands

- [scan](http://docs.ceph.com/docs/master/ceph-volume/simple/scan/#ceph-volume-simple-scan)
- [activate](http://docs.ceph.com/docs/master/ceph-volume/simple/activate/#ceph-volume-simple-activate)
- [systemd](http://docs.ceph.com/docs/master/ceph-volume/simple/systemd/#ceph-volume-simple-systemd)







