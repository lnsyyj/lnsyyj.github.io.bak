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

## Command Line Subcommands

### prepare

[prepare](http://docs.ceph.com/docs/master/ceph-volume/lvm/prepare/#ceph-volume-lvm-prepare)

该子命令允许[filestore](http://docs.ceph.com/docs/master/glossary/#term-filestore)或[bluestore](http://docs.ceph.com/docs/master/glossary/#term-bluestore)设置。 建议在使用`ceph-volume lvm`之前预先配置逻辑卷。除添加额外元数据外，逻辑卷不会改变。

为了帮助识别volumes，preparing的过程中该工具使用[LVM tags](http://docs.ceph.com/docs/master/glossary/#term-lvm-tags)分配一些元数据信息。

[LVM tags](http://docs.ceph.com/docs/master/glossary/#term-lvm-tags)使volume易于发现，帮助识别它们作为Ceph系统的一部分，扮演着什么角色（journal, filestore, bluestore, etc…）

虽然最初支持[filestore](http://docs.ceph.com/docs/master/glossary/#term-filestore)（默认情况下支持），但可以使用以下命令指定：

- [–filestore](http://docs.ceph.com/docs/master/ceph-volume/lvm/prepare/#ceph-volume-lvm-prepare-filestore)
- [–bluestore](http://docs.ceph.com/docs/master/ceph-volume/lvm/prepare/#ceph-volume-lvm-prepare-bluestore)

**FILESTORE**

这是OSD backend，允许为[filestore](http://docs.ceph.com/docs/master/glossary/#term-filestore) objectstore OSD 准备逻辑卷。

它可以使用逻辑卷作为OSD data和带分区的physical device或逻辑卷作为journal。除了遵循data和journal的最小大小要求外,这些卷不需要特殊准备。

API调用如下所示：

```
ceph-volume lvm prepare --filestore --data volume_group/lv_name --journal journal
```

启用[encryption](http://docs.ceph.com/docs/mimic/ceph-volume/lvm/encryption/#ceph-volume-lvm-encryption)（加密），需要使用--dmcrypt标志：

```
ceph-volume lvm prepare --filestore --dmcrypt --data volume_group/lv_name --journal journal
```





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

通过接管管理，它会disable所有用于在startup时触发device的ceph-disk systemd units，依赖基本的（可定制的）JSON配置和systemd来启动OSD。

此过程包括两个步骤：

1、[Scan](http://docs.ceph.com/docs/master/ceph-volume/simple/scan/#ceph-volume-simple-scan)（扫描）正在运行的OSD或data device

2、[Activate](http://docs.ceph.com/docs/master/ceph-volume/simple/activate/#ceph-volume-simple-activate)（激活）扫描的OSD

扫描将推断出ceph-volume启动OSD所需的所有内容，因此当需要激活时，OSD可以正常启动而不会受到来自ceph-disk的干扰。

作为激活过程的一部分，负责对udev事件作出反应的ceph-disk的systemd units链接到/dev/null，以便它们完全处于非活动状态。