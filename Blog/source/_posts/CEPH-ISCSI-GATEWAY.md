---
title: CEPH ISCSI GATEWAY
date: 2019-10-08 15:14:50
tags: ceph,iscsi
---

iSCSI gateway将Ceph Storage与iSCSI标准集成在一起，将RADOS Block Device（RBD）images导出为SCSI disks高可用（HA）iSCSI target。iSCSI协议允许客户端（initiators）通过TCP / IP网络将SCSI命令发送到SCSI storage devices（targets）。这允许异构客户端（例如Microsoft Windows）访问Ceph存储群集。

每个iSCSI gateway网关都运行Linux IO target kernel subsystem（LIO）以提供iSCSI协议支持。LIO利用userspace直通（TCMU）Ceph的librbd库进行交互，并将RBD images公开给iSCSI客户端。借助Ceph的iSCSI gateway，您可以有效地运行完整的block-storage infrastructure，并具有传统Storage Area Network （SAN） 的所有功能和优势。

![](<https://docs.ceph.com/docs/mimic/_images/ditaa-f7600ac6387b34e7a79850e43f3b46794cbbc62f.png>)

# Requirements

要实现Ceph iSCSI gateway，有一些要求。对于高可用的Ceph iSCSI gateway解决方案，建议使用2到4个iSCSI gateway节点。

有关硬件建议，请参阅[Hardware Recommendation page](http://docs.ceph.com/docs/master/start/hardware-recommendations/)获取更多详细信息。

```
注意 在iSCSI gateway节点上，RBD images的内存占用量可能会增大。根据映射(mapped)的RBD images的数量，相应地计划内存需求。
```

Ceph Monitors或OSD没有特定的iSCSI gateway选项，但是降低默认timers以检测OSD至关重要，它可以降低initiator超时的可能性，这一点很重要。建议为存储集群中的每个OSD节点使用以下配置选项：

```
[osd]
osd heartbeat grace = 20
osd heartbeat interval = 5
```

- Ceph Monitor的在线更新

```
ceph tell <daemon_type>.<id> config set <parameter_name> <new_value>

ceph tell osd.0 config set osd_heartbeat_grace 20
ceph tell osd.0 config set osd_heartbeat_interval 5
```

- OSD节点的在线更新

```
ceph daemon <daemon_type>.<id> config set osd_client_watch_timeout 15

ceph daemon osd.0 config set osd_heartbeat_grace 20
ceph daemon osd.0 config set osd_heartbeat_interval 5
```

有关设置Ceph的配置选项的更多详细信息，请参阅[Configuration page](http://docs.ceph.com/docs/master/rados/configuration/)。

# Configuring the iSCSI Target

传统上，对Ceph存储集群的块级访问仅限于QEMU和librbd，这是在OpenStack环境中采用的关键因素。从Ceph Luminous版本开始，块级访问正在扩展，以提供标准的iSCSI支持，从而允许更广泛的平台使用，并有可能打开新的用例。

- RHEL/CentOS 7.5; Linux kernel v4.16 or newer; or the [Ceph iSCSI client test kernel](https://shaman.ceph.com/repos/kernel/ceph-iscsi-test)
- 一个可用的Ceph Storage cluster
- iSCSI gateways可以与OSD位于同一节点，也可以位于专用节点上
- iSCSI前端流量和Ceph后端流量使用单独的网络

安装和配置 Ceph iSCSI gateway的方法： 

- [Using Ansible](https://docs.ceph.com/docs/mimic/rbd/iscsi-target-ansible/)
- [Using the Command Line Interface](https://docs.ceph.com/docs/mimic/rbd/iscsi-target-cli/)

# Configuring the iSCSI Initiators

- [iSCSI Initiator for Linux](https://docs.ceph.com/docs/mimic/rbd/iscsi-initiator-linux)
- [iSCSI Initiator for Microsoft Windows](https://docs.ceph.com/docs/mimic/rbd/iscsi-initiator-win)
- [iSCSI Initiator for VMware ESX](https://docs.ceph.com/docs/mimic/rbd/iscsi-initiator-esx)

```
警告：通过多个iSCSI网关导出RBD image时，不支持使用SCSI persistent group reservations（PGR）和基于SCSI 2 reservations的应用程序。
```

# Monitoring the iSCSI Gateways

Ceph为iSCSI gateway环境提供了一个附加工具，以监视导出的RADOS Block Device（RBD）images的性能。

该gwtop工具是一个类似top的工具，用于显示通过iSCSI导出到客户端的RBD images的聚合性能指标。这些指标来自Performance Metrics Domain Agent（PMDA）。来自Linux-IO target（LIO）PMDA的信息用于列出每个已导出的RBD images以及所连接的客户端及其关联的I / O指标。

**Requirements**

- 正在运行的Ceph iSCSI gateway

**Installing**

1、在每个iSCSI gateway节点安装ceph-iscsi-tools

```
yum install ceph-iscsi-tools
```

2、在每个iSCSI gateway节点安装performance co-pilot

```
yum install pcp
```

3、在每个iSCSI gateway节点安装LIO PMDA

```
yum install pcp-pmda-lio
```

4、在每个iSCSI gateway节点enable并start performance co-pilot service

```
# systemctl enable pmcd
# systemctl start pmcd
```

5、注册pcp-pmda-lio agent

```
cd /var/lib/pcp/pmdas/lio
./Install
```

默认情况下，`gwtop`假定iSCSI gateway configuration object被存储在rbd pool被叫做gateway.conf的RADOS object中。此configuration定义了需要收集性能信息的iSCSI gateways。可以使用`-g`或`-c`标志将其覆盖 。gwtop --help获取更多详细信息。

LIO configuration确定要从performance co-pilot提取性能统计信息的类型。当`gwtop`启动时，它着眼于LIO configuration，如果发现user-space的磁盘，然后`gwtop` 自动选择LIO收集器。

**Example gwtop Outputs**

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

在*Client* column，`(CON)`表示iSCSI initiator (client)当前已登录到iSCSI gateway。如果显示`-multi-`，则表示多个clients映射到单个RBD image。

