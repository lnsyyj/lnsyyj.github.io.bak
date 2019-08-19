---
title: ceph社区为什么要使用ceph-volume代替ceph-disk
date: 2019-08-16 16:35:15
tags: ceph
---

[原文地址](<https://docs.ceph.com/docs/master/ceph-volume/intro/#ceph-disk-replaced>)

# OVERVIEW

ceph-volume旨在成为一个单一用途的命令行工具，将logical volumes部署为OSDs，在preparing, activating, 和creating OSDs时，试图维护与ceph-disk类似的API。

它不依赖于Ceph安装的udev rules，从而背离ceph-disk。这些rules允许自动检测先前设置的devices，这些devices又被送入ceph-disk以activate它们。

# REPLACING `CEPH-DISK`

ceph-disk工具是在项目需要支持许多不同类型的init系统（upstart, sysvinit, etc…）时创建的，同时能够发现devices。这导致工具最初（以及后来）专注于GPT分区上。特别是在GPT GUID上，它以独特的方式标记device，以回答以下问题：

- device是否是Journal？
- 是否是encrypted data partition（加密数据分区）？
- device是否已部分准备就绪？

为了解决这些问题，它使用UDEV规则来匹配GUID，这些GUID将调用ceph-disk，最后在ceph-disk systemd unit和ceph-disk可执行文件之间来回切换。该过程非常不可靠且耗时（必须为每个OSD设置接近3小时的超时），并且会导致OSD 在节点的boot过程中根本不出现。

考虑到UDEV的异步行为，很难调试甚至复现这些问题。

由于ceph-disk的world-view必须是专门的GPT分区，这意味着它无法与其他技术（如LVM或类似的device mapper devices）一起使用。它最终决定创建一些模块化的东西，从LVM支持开始，并根据需要扩展其他技术。

# GPT PARTITIONS ARE SIMPLE?

尽管分区通常很容易理解，但ceph-disk分区并不简单。它需要大量特殊标志才能使它们与device discovery工作流程一起正常工作。以下是创建数据分区的示例调用：

```
/sbin/sgdisk --largest-new=1 --change-name=1:ceph data --partition-guid=1:f0fc39fd-eeb2-49f1-b922-a11939cf8a0f --typecode=1:89c57f98-2fe5-4dc0-89c1-f3ad0ceff2be --mbrtogpt -- /dev/sdb
```

创建这些不仅很难，而且这些分区要求device由 Ceph 独占。例如，在某些情况下，在device加密时将创建一个特殊分区，其中包含未加密的密钥。这是头盘域知识，不会转化为"GPT 分区很简单"的理解。下面是正在创建的特殊分区的示例：

```
/sbin/sgdisk --new=5:0:+10M --change-name=5:ceph lockbox --partition-guid=5:None --typecode=5:fb3aabf9-d25f-47cc-bf5e-721d181642be --mbrtogpt -- /dev/sdad
```





























