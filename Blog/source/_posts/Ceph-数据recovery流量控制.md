---
title: Ceph 数据recovery流量控制
date: 2019-11-16 12:34:16
tags: ceph
---

ceph在扩容或缩容期间会有数据rebalance。如何控制在rebalance时，尽量降低对client IO的影响？调研如下：

# 首先，在什么情况下ceph会出现数据rebalance？

本质上，用户数据写入ceph时，会被切分成大小相等的object，这些object由PG承载，分布到不同的OSD上（每个OSD一般会对应一块硬盘）。数据的迁移会以PG为单位进行，所以当PG发生变化时，就会有数据rebalance。

### 那么在什么时候PG会变化呢？

从用户使用角度讲一般有如下几种场景：

- 添加/删除OSD
- 重新调整pool的PG数

### Client IO优先

降低recovery的I/O优先级

```
调整后
[root@ceph ~]# ceph daemon osd.2 config show | grep osd_recovery_op_priority 
    "osd_recovery_op_priority": "3",

使用默认值
[root@ceph ~]# ceph daemon osd.2 config show | grep osd_client_op_priority 
    "osd_client_op_priority": "63",
```

降低recovery的I/O带宽及backfill带宽

```
调整后
[root@ceph ~]# ceph daemon osd.2 config show | grep osd_recovery_max_active
    "osd_recovery_max_active": "1",

调整后
[root@ceph ~]# ceph daemon osd.2 config show | grep osd_recovery_sleep
    "osd_recovery_sleep": "0.200000",
```

调整相关命令

```
[root@ceph ~]# ceph daemon osd.2 config set osd_recovery_op_priority 3
{
    "success": "osd_recovery_op_priority = '3' (not observed, change may require restart) "
}
[root@ceph ~]# ceph daemon osd.2 config show | grep osd_recovery_op_priority 
    "osd_recovery_op_priority": "3",

获取osd ID
[root@ceph ~]# ll /var/run/ceph/ | grep osd | awk '{print $9}' | sed 's/ceph-\(.*\).asok/\1/'
osd.13
osd.2
osd.5
osd.8
```



### 添加OSD时

1、BACK FILLING

当新的OSD加入群集时，CRUSH将把placement groups从群集中的OSD重新分配给新添加的OSD。强制新OSD立即接受重新分配的placement groups会给新OSD带来过多的负担。用placement groups backfilling OSD在后台运行。backfilling完成后，新的OSD将在准备就绪后开始处理请求。

在backfilling操作期间，您可能会看到以下几种状态之一：

- backfill_wait：表示backfilling操作尚未完成，但尚未进行
- backfilling：表示正在进行backfilling操作
- backfill_toofull：表示已请求backfill操作，但由于存储容量不足而无法完成

如果无法重新backfilled placement group，则可以认为该placement group considered incomplete（不完整）

backfill_toofull状态可能是瞬态的。随着PG的移动，空间可能变得可用。 backfill_toofull与backfill_wait类似，因为一旦条件发生变化，backfill就可以继续进行。

Ceph提供了许多设置来管理将placement groups重新分配给OSD（尤其是新OSD）相关的负载峰值。默认情况下，osd_max_backfill设置OSD之间最大并发backfill数为1。backfill full ratio可以使OSD拒绝接受backfill请求（默认为90％），使用ceph osd set-backfillfull-ratio命令进行更改。如果OSD拒绝backfill请求，则osd backfill retry interval使OSD可以重试该请求（默认为30秒后）。OSD还可以设置osd backfill scan min和osd backfill scan max以管理扫描间隔（默认为64和512）。

```
    Option("osd_max_backfills", Option::TYPE_UINT, Option::LEVEL_ADVANCED)
    .set_default(1)
    .set_description("Maximum number of concurrent local and remote backfills or recoveries per OSD ")
    .set_long_description("There can be osd_max_backfills local reservations AND the same remote reservations per OSD. So a value of 1 lets this OSD participate as 1 PG primary in recovery and 1 shard of another recovering PG."),
```



# 参考资料

【1】https://blog.csdn.net/Linux_kiss/article/details/82857117

【2】http://www.zphj1987.com/2017/08/10/Ceph-recover-speed-control/





