---
title: ceph 问题列表
date: 2019-10-14 15:47:02
tags: ceph
---





# 问题列表

## Ceph升级（L to N）引发的问题

| 问题ID                   | 1                                                            |
| ------------------------ | ------------------------------------------------------------ |
| 问题出现版本             | pre-14.2.3                                                   |
| 问题现象                 | ...<br/>POOLS:<br/>    POOL                           ID      STORED      OBJECTS     USED        %USED     MAX AVAIL <br/>    data                             0      63 TiB      44.59M      63 TiB     30.21        48 TiB <br/>...<br/>but when one OSD was updated it changed to<br/>POOLS:<br/>    POOL                           ID      STORED      OBJECTS     USED        %USED     MAX AVAIL <br/>    data                             0     558 GiB      43.50M    <br/>1.7 TiB      1.22        45 TiB |
| 问题触发条件             | 1、从nautilus之前的集群进行了升级<br/>2、然后，您提供一个或多个新的BlueStore OSD，或在升级的OSD上运行“ceph-bluestore-tool repair”。 |
| 问题原因                 | 根本原因是，从Nautilus开始，BlueStore维护了每个池的使用情况统计信息，但是它需要对磁盘上的格式进行少量更改。<br/>除非您运行ceph-bluestore-tool修复，否则升级后的OSD不会拥有新的统计信息。<br/>问题在于，一旦* any * OSD报告了er-pool统计信息，mon就开始使用新的统计信息（而不是等到* all * OSD都在这样做）。 |
| 问题解决办法             | 为避免此问题，可以<br/>1、升级后不要置备新的BlueStore OSD<br/>2、更新所有OSD，以保留新的每个池统计信息。现有的BlueStore OSD可以通过以下方式转换：<br/>     systemctl stop ceph-osd@$N<br/>     ceph-bluestore-tool repair --path /var/lib/ceph/osd/ceph-$N<br/>     systemctl start ceph-osd@$N<br/>请注意，FileStore根本不支持新版每个池统计信息，因此，如果集群中有文FileStore OSD，则没有解决方法。无需将文件存储OSD替换为bluestore。<br/>修复程序[1]正在通过QA检查，将在14.2.3中出现； 它不会在14.2.2完整发布。 |
| ceph-users地址           | http://lists.ceph.com/pipermail/ceph-users-ceph.com/2019-July/035889.html<br/>https://github.com/ceph/ceph/pull/28978<br/>https://tracker.ceph.com/versions/574 |
| 备注                     |                                                              |
| 实践修复                 |                                                              |
| 问题修复版本（社区计划） | 14.2.3                                                       |



| 问题ID                   | 2                                                            |
| ------------------------ | ------------------------------------------------------------ |
| 问题出现版本             | 14.2.2                                                       |
| 问题现象                 | Legacy BlueStore stats reporting detected on 6 OSD(s)        |
| 问题触发条件             | 1、从nautilus之前的集群进行了升级                            |
| 问题原因                 |                                                              |
| 问题解决办法             | systemctl stop ceph-osd@$OSDID<br/>ceph-bluestore-tool repair --path /var/lib/ceph/osd/ceph-$OSDID<br/>systemctl start ceph-osd@$OSDID |
| ceph-users地址           | http://lists.ceph.com/pipermail/ceph-users-ceph.com/2019-July/036010.html |
| 备注                     | 可以静默告警 bluestore warn on legacy statfs = false         |
| 实践修复                 | [root@ceph1 ~]# systemctl stop ceph-osd@1.service<br/>[root@ceph1 ~]# ceph-bluestore-tool repair --path /var/lib/ceph/osd/ceph-1/<br/>2019-10-14 15:39:53.940 7f87c8114f80 -1 bluestore(/var/lib/ceph/osd/ceph-1) fsck error: legacy statfs record found, removing<br/>2019-10-14 15:39:53.940 7f87c8114f80 -1 bluestore(/var/lib/ceph/osd/ceph-1) fsck error: missing Pool StatFS record for pool 2<br/>2019-10-14 15:39:53.940 7f87c8114f80 -1 bluestore(/var/lib/ceph/osd/ceph-1) fsck error: missing Pool StatFS record for pool ffffffffffffffff<br/>repair success<br/>[root@ceph1 ~]# systemctl start ceph-osd@1.service |
| 问题修复版本（社区计划） |                                                              |



| 问题ID                   | 3                                                            |
| ------------------------ | ------------------------------------------------------------ |
| 问题出现版本             | 14.2.2                                                       |
| 问题现象                 | Legacy BlueStore stats reporting detected on 6 OSD(s)        |
| 问题触发条件             | 1、从nautilus之前的集群进行了升级                            |
| 问题原因                 |                                                              |
| 问题解决办法             | systemctl stop ceph-osd@$OSDID<br/>ceph-bluestore-tool repair --path /var/lib/ceph/osd/ceph-$OSDID<br/>systemctl start ceph-osd@$OSDID |
| ceph-users地址           | http://lists.ceph.com/pipermail/ceph-users-ceph.com/2019-July/036002.html |
| 备注                     | 可以静默告警 bluestore warn on legacy statfs = false         |
| 实践修复                 |                                                              |
| 问题修复版本（社区计划） |                                                              |



| 问题ID                   | 4                                                            |
| ------------------------ | ------------------------------------------------------------ |
| 问题出现版本             | 14.2.4                                                       |
| 问题现象                 | 3 monitors have not enabled msgr2告警                        |
| 问题触发条件             | 1、从nautilus之前的集群进行了升级                            |
| 问题原因                 | messenger v2 protocol（msgr2）是Ceph’s on-wire protocol第二次主要修订。 |
| 问题解决办法             | ceph mon enable-msgr2<br>systemctl restart ceph-mon@ceph1.service |
| ceph-users地址           |                                                              |
| 备注                     |                                                              |
| 实践修复                 | ceph mon enable-msgr2<br/>systemctl restart ceph-mon@ceph1.service |
| 问题修复版本（社区计划） |                                                              |



| 问题ID                   | 5    |
| ------------------------ | ---- |
| 问题出现版本             |      |
| 问题现象                 |      |
| 问题触发条件             |      |
| 问题原因                 |      |
| 问题解决办法             |      |
| ceph-users地址           |      |
| 备注                     |      |
| 实践修复                 |      |
| 问题修复版本（社区计划） |      |



| 问题ID                   |      |
| ------------------------ | ---- |
| 问题出现版本             |      |
| 问题现象                 |      |
| 问题触发条件             |      |
| 问题原因                 |      |
| 问题解决办法             |      |
| ceph-users地址           |      |
| 备注                     |      |
| 实践修复                 |      |
| 问题修复版本（社区计划） |      |



| 问题ID                   |      |
| ------------------------ | ---- |
| 问题出现版本             |      |
| 问题现象                 |      |
| 问题触发条件             |      |
| 问题原因                 |      |
| 问题解决办法             |      |
| ceph-users地址           |      |
| 备注                     |      |
| 实践修复                 |      |
| 问题修复版本（社区计划） |      |

































