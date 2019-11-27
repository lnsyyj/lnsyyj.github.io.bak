---
title: Ceph iSCIS gateway 监控
date: 2019-11-27 23:27:23
tags: ceph
---

# 监控ISCSI GATEWAYS

Ceph为iSCSI gateway环境提供了一个附加工具，以监视导出的RADOS Block Device (RBD) images的性能。

gwtop工具是一个类似top的工具，用于显示通过iSCSI导出到客户端的RBD images的聚合性能指标。 这些指标来自Performance Metrics Domain Agent（PMDA）。 来自Linux-IO target (LIO) PMDA的信息用于列出每个已导出的RBD image以及所连接的客户端及其相关的I/O指标。

**Requirements:**

正在运行的Ceph iSCSI gateway

**Installing:**

1、以root用户身份在每个iSCSI gateway节点上安装ceph-iscsi-tools package：

```
# yum install ceph-iscsi-tools
```

2、以root用户身份在每个iSCSI gateway节点上安装performance co-pilot package：

```
# yum install pcp
```

3、以root用户身份在每个iSCSI gateway节点上安装LIO PMDA package：

```
# yum install pcp-pmda-lio
```

4、以root用户身份，在每个iSCSI gateway节点上启用并启动performance co-pilot service：

```
# systemctl enable pmcd
# systemctl start pmcd
```

5、以root用户身份注册pcp-pmda-lio agent：

```
cd /var/lib/pcp/pmdas/lio
./Install
```

默认情况下，gwtop假定iSCSI gateway configuration object存储在rbd pool中的一个名为gateway.conf的RADOS object中。 此configuration定义了收集性能统计信息的iSCSI gateways。 可以使用-g或-c标志将其覆盖。 有关更多详细信息，请参见gwtop --help。

LIO configuration决定要从performance co-pilot提取的性能统计信息的类型。 gwtop启动时，它会查看LIO configuration，如果找到user-space disks，则gwtop会自动选择LIO collector。

示例``gwtop``输出

```
gwtop  2/2 Gateways   CPU% MIN:  4 MAX:  5    Network Total In:    2M  Out:    3M   10:20:00
Capacity:   8G    Disks:   8   IOPS:  503   Clients:  1   Ceph: HEALTH_OK          OSDs:   3
Pool.Image       Src    Size     iops     rMB/s     wMB/s   Client
iscsi.t1703             500M        0      0.00      0.00
iscsi.testme1           500M        0      0.00      0.00
iscsi.testme2           500M        0      0.00      0.00
iscsi.testme3           500M        0      0.00      0.00
iscsi.testme5           500M        0      0.00      0.00
rbd.myhost_1      T       4G      504      1.95      0.00   rh460p(CON)
rbd.test_2                1G        0      0.00      0.00
rbd.testme              500M        0      0.00      0.00
```

在Client列中，(CON)表示iSCSI initiator (client)当前已登录iSCSI gateway。 如果显示-multi-，则多个clients映射到单个RBD image。