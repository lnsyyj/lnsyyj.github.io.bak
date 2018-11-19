---
title: CEPH-VOLUME
date: 2018-05-19 19:19:32
tags: CEPH-Luminous
---

使用可插拔工具将 OSDs 与 lvm 或物理磁盘等不同的device技术部署在一起（[lvm](http://docs.ceph.com/docs/master/ceph-volume/lvm/)本身被视为plugin）, 并尝试采用可以预测的、健壮的方法来preparing、activating和starting OSDs。

[Overview](http://docs.ceph.com/docs/master/ceph-volume/intro/#ceph-volume-overview)|[Plugin Guide](http://docs.ceph.com/docs/master/dev/ceph-volume/plugins/#ceph-volume-plugins)

# **Command Line Subcommands** 目前支持lvm，以及可能已使用ceph-disk一起部署的普通磁盘（带有GPT分区）。

- [lvm](http://docs.ceph.com/docs/master/ceph-volume/lvm/#ceph-volume-lvm)

- [simple](http://docs.ceph.com/docs/master/ceph-volume/simple/#ceph-volume-simple)

## MIGRATING

从 Ceph 12.2.2 版本开始, ceph-disk已弃用。弃用警告将显示链接到此页面。强烈建议用户开始使用ceph-volume。有两种迁移途径:

1、保留使用ceph-disk部署的OSD：The [simple](http://docs.ceph.com/docs/master/ceph-volume/simple/#ceph-volume-simple) command提供了一种接管管理的方法，同时禁用了“ceph-disk”触发器。

2、用ceph-volume重新部署现有的OSD：深度 [Replacing an OSD](http://docs.ceph.com/docs/master/rados/operations/add-or-rm-osds/#rados-replacing-an-osd)

### NEW DEPLOYMENTS

对于新的部署，推荐使用[lvm](http://docs.ceph.com/docs/master/ceph-volume/lvm/#ceph-volume-lvm)，它可以使用任何logical volume作为数据 OSDs 的输入, 也可以从设备中设置minimal/naive logical volume。

### EXISTING OSDS

如果群集具有 ceph-disk提供的 OSDs, 则 ceph-volume可以用简单（[simple](http://docs.ceph.com/docs/master/ceph-volume/simple/#ceph-volume-simple)）的方式接管。在data device或 OSD directory上进行扫描, ceph-disk被完全禁用。完全支持加密。



# LVM

实现从 lvm 子命令部署 OSDs 所需的功能: ceph-volume lvm















