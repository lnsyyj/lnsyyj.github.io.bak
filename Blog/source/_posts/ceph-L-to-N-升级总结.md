---
title: ceph L to N 升级总结
date: 2019-10-25 10:07:08
tags: ceph
---

[官方原文](https://docs.ceph.com/docs/master/releases/nautilus/)

[参考1](https://pve.proxmox.com/wiki/Ceph_Luminous_to_Nautilus)

### 从Luminous之前的版本进行升级（比如jewel版本）

```
必须先升级到Luminous（12.2.z），然后再尝试升级到Nautilus。 另外，您的集群必须在运行Luminous的同时至少完成了所有PG的一次scrub，并在OSD map中设置了recovery_deletes和purged_snapdirs flags。

[root@ceph1 ~]# ceph osd dump | grep ^flags
flags sortbitwise,recovery_deletes,purged_snapdirs

recovery_deletes flags：Ceph现在在recovery过程中处理delete操作，而不是在peering过程中。以前，将down或out超过15分钟的OSD带回到群集中会导致placement group peering时间延长。peering过程需要很长时间才能完成，因为delete操作是在合并placement group日志作为peering过程的一部分时内联处理的。结果，对处于peering状态的placement group的操作被blocked。通过此更新，Ceph可以在正常recovery过程中而不是peering过程中处理delete操作。结果可以使peering过程更快完成，并且操作不再blocked。（This was fixed with the help of Red Hat Ceph Storage feature request #1452780 and this was released in 2.4 errata version 10.2.7-32.el7cp.）

purged_snapdirs flags：一旦snapsets全部转换，则设置purged_snapdirs OSDMap flags，这样可以更轻松地测试upgrade + conversion是否已完成。特别是，micim+将能够更简单地测试该flags，而无需等待完整的PG统计信息来知道升级到luminous以外的版本是否安全。
```

### 注意

```
1、在从Luminous升级到Nautilus的过程中，将monitors升级到Nautilus后，将无法使用Luminous ceph-osd daemon创建新的OSD。避免在升级过程中添加或替换任何OSD。
2、避免在升级过程中创建任何RADOS pools。
3、您可以使用ceph version(s)命令在每个阶段监视升级进度，该命令将告诉您每种ceph daemon正在运行的ceph版本。
```

# 自研管理平台用到的接口，对比输出是否有修改，以免影响前台功能



# Ceph节点升级过程

1、确认OSD map包含recovery_deletes和purged_snapdirs flags（否则，将导致您的monitor daemons在启动时拒绝加入quorum，从而使其无法运行）

```
执行命令：ceph osd dump | grep ^flags
命令输出：flags sortbitwise,recovery_deletes,purged_snapdirs

如果没有recovery_deletes和purged_snapdirs flags需要手动触发pg scrub，并等待大约24-48小时（根据数据量评估）
执行命令：ceph pg dump pgs_brief | cut -d " " -f 1 | xargs -n1 ceph pg scrub
```

2、确保集群稳定，集群状态HEALTH_OK，没有down掉的OSD或无法恢复的OSD。

```
执行命令：ceph health detail
命令输出：HEALTH_OK

执行命令：ceph osd tree | grep down
命令输出：空

执行命令：ceph osd tree | grep out
命令输出：空
```

3、设置noout flags

```
执行命令：ceph osd set noout
命令输出：noout is set

执行命令：ceph health detail
命令输出：
HEALTH_WARN noout flag(s) set
OSDMAP_FLAGS noout flag(s) set
```

4、配置centos ceph 14 Luminous mirror

```

```

5、升级monitors（通过安装新ceph packages并重新启动monitor daemons来升级monitors）

在**每个monitors主机上**执行：

```
5.1、安装monitor packages
执行命令：yum install ceph-mon
命令输出：提示安装ceph-mon以及依赖

5.2、重启ceph monitor服务
执行命令：systemctl restart ceph-mon.target
命令输出：空
```

monitors启动之后，查找nautilus字符串来验证monitor升级是否完成。确保有min_mon_release 14 (nautilus)字样，如果没有说明尚未升级和重启monitor。

```
执行命令：ceph mon dump | grep min_mon_release
命令输出：
dumped monmap epoch 2
min_mon_release 14 (nautilus)
```

6、升级ceph-mgr（通过安装新ceph packages并重新启动ceph-mgr daemons来升级ceph-mgr）

如果使用Ceph Dashboard需要安装ceph-mgr-dashboard package。

在**每个ceph-mgr主机上**执行：

```
6.1、安装ceph-mgr packages
执行命令：yum install ceph-mgr
命令输出：提示安装ceph-mgr以及依赖

6.2、安装ceph-mgr-dashboard packages
执行命令：yum install ceph-mgr-dashboard
命令输出：提示安装ceph-mgr-dashboard以及依赖

6.3、重启ceph mgr服务
执行命令：systemctl restart ceph-mgr.target
命令输出：空
```

通过ceph -s验证ceph-mgr daemons是否正在运行（确保mgr状态active，standbys mgr加入）。

```
执行命令：ceph -s
命令输出：
...
  services:
    mon: 3 daemons, quorum community-ceph-2,community-ceph-3,community-ceph-1 (age 25m)
    mgr: community-ceph-2(active, since 3d), standbys: community-ceph-3, community-ceph-1
...
```

7、升级ceph-osd（通过安装新ceph packages并重新启动ceph-osd daemons来升级ceph-osd）

在**每个ceph-osd主机上**执行：

```
7.1、安装ceph-osd packages
执行命令：yum install ceph-osd
命令输出：提示安装ceph-osd以及依赖

7.2、重启ceph osd服务
执行命令：systemctl restart ceph-osd.target
命令输出：空
```

查看OSD升级的进度

```
执行命令：ceph osd versions
命令输出：
{
   "ceph version 12.2.x (...) luminous (stable)": 2,
   "ceph version 14.2.4 (...) nautilus (stable)": 4,
}
```

使用ceph-volume接管ceph-disk创建的OSD，在**每个OSD主机上**执行，**执行前确保每个OSD都正在运行，无down或out的OSD**：

```
1、确保OSD运行正常
执行命令：ceph osd tree | grep down
命令输出：空
执行命令：ceph osd tree | grep out
命令输出：空

2、所有使用ceph-disk创建的并正在运行的OSDs，从OSD data partition或directory中捕获元数据
执行命令：ceph-volume simple scan
命令输出：

执行命令后，会生成类似/etc/ceph/osd/0-ab0a204a-42e3-4a47-ab4c-0888edf429cb.json文件，文件内容为：
{
    "active": "ok", 
    "block": {
        "path": "/dev/disk/by-partuuid/0818811f-d70e-4ff0-91c9-58cd701c9a19", 
        "uuid": "0818811f-d70e-4ff0-91c9-58cd701c9a19"
    }, 
    "block_uuid": "0818811f-d70e-4ff0-91c9-58cd701c9a19", 
    "bluefs": 1, 
    "ceph_fsid": "c4051efa-1997-43ef-8497-fb02bdf08233", 
    "cluster_name": "ceph", 
    "data": {
        "path": "/dev/vdc1", 
        "uuid": "ab0a204a-42e3-4a47-ab4c-0888edf429cb"
    }, 
    "fsid": "ab0a204a-42e3-4a47-ab4c-0888edf429cb", 
    "keyring": "AQB1FLFdXVHVARAARTKkxT1xgrDNU/QECUqdxA==", 
    "kv_backend": "rocksdb", 
    "magic": "ceph osd volume v026", 
    "mkfs_done": "yes", 
    "ready": "ready", 
    "systemd": "", 
    "type": "bluestore", 
    "whoami": 0
}

3、使systemd units可以mount已配置的devices，并启动Ceph OSD
执行命令：ceph-volume simple activate --all
命令输出：
--> activating OSD specified in /etc/ceph/osd/1-fe327306-54a4-4362-870d-92d28cf65e42.json
Running command: ln -snf /dev/vdc2 /var/lib/ceph/osd/ceph-1/block
Running command: chown -R ceph:ceph /dev/vdc2
Running command: systemctl enable ceph-volume@simple-1-fe327306-54a4-4362-870d-92d28cf65e42
Running command: ln -sf /dev/null /etc/systemd/system/ceph-disk@.service
--> All ceph-disk systemd units have been disabled to prevent OSDs getting triggered by UDEV events
Running command: systemctl enable --runtime ceph-osd@1
Running command: systemctl start ceph-osd@1
--> Successfully activated OSD 1 with FSID fe327306-54a4-4362-870d-92d28cf65e42

4、重启每个OSD主机，确认OSD是否开机自启
执行命令：reboot
命令输出：无
```

8、升级ceph-mds（通过安装新ceph packages并重新启动ceph-mds daemons来升级ceph-mds）

在**每个ceph-mds主机上**执行：

```
记录rank数量，并将ranks数减少到1，通过ceph mds stat命令输出可以查看rank数量。
{0=ceph3=up:active}代表rank数为1，{0=ceph3=up:active,1=ceph2=up:active}代表rank数为2，以此类推。

8.1、查看当前rank数量
执行命令：ceph mds stat
命令输出：cephfs-1/1/1 up  {0=ceph3=up:active}, 2 up:standby
如果rank为2，命令输出为：cephfs-2/2/2 up  {0=ceph3=up:active,1=ceph2=up:active}, 1 up:standby

8.2、查看cephfs名称
执行命令：ceph fs ls
命令输出：name: cephfs, metadata pool: cephfs_metadata, data pools: [cephfs_data ]

8.3、将ranks数减少到1
执行命令：ceph fs set <fs_name> max_mds 1
命令输出：空

8.4、通过定期检查状态，等待集群停用所有non-zero ranks，当{0=ceph3=up:active}时为已停用所有non-zero ranks
执行命令：ceph status
命令输出：cephfs-1/1/1 up  {0=ceph3=up:active}, 2 up:standby

8.5、使用以下命令使所有standby MDS daemons在适当的主机offline
执行命令：systemctl stop ceph-mds@<daemon_name>
命令输出：空

8.6、确认只有一个MDS处于online，并且cephfs只有一个rank 0
执行命令：ceph status
命令输出：
...
    mds: cephfs-1/1/1 up  {0=ceph3=up:active}
...

8.7、通过安装新packages并重新启动daemon来升级MDS daemon（在每个mds节点上执行）
执行命令：yum install ceph-mds
命令输出：提示安装ceph-mds以及依赖

执行命令：systemctl restart ceph-mds.target
命令输出：空

8.8、重新启动所有已的offline standby MDS daemons
执行命令：systemctl restart ceph-mds.target
命令输出：空

8.9、恢复max_mds原始值
执行命令：ceph fs set <fs_name> max_mds <original_max_mds>
命令输出：空
```

9、升级ceph-radosgw（通过安装新ceph packages并重新启动ceph-radosgw daemons来升级ceph-radosgw）

在**每个ceph-radosgw主机上**执行：

```
5.1、安装radosgw packages
执行命令：yum install ceph-radosgw
命令输出：提示安装ceph-radosgw以及依赖

5.2、重启ceph radosgw服务
执行命令：systemctl restart ceph-radosgw.target
命令输出：空
```

10、启用所有Nautilus的新功能来完成升级

```
执行命令：ceph osd require-osd-release nautilus
命令输出：空
```

11、清除noout flags

```
执行命令：ceph osd unset noout
命令输出：noout is unset
```

12、启用新的 v2 network protocol

```
执行命令：ceph mon enable-msgr2
命令输出：空
```

# Client节点升级过程

1、Client节点升级package

```
执行命令：yum install ceph-common librados2 librbd1 python-rbd python-rados -y
命令输出：提示安装ceph-common librados2 librbd1 python-rbd python-rados以及依赖
```

2、Client节点确认升级后的版本

```
执行命令：ceph --version
命令输出：ceph version 14.2.4 (75f4de193b3ea58512f204623e6c5a16e6c1e1ba) nautilus (stable)
```

# Ceph端升级后修复

### 1、Legacy BlueStore stats reporting detected on 6 OSD(s)

问题描述：

​	使用ceph -s命令查看集群状态时，出现Legacy BlueStore stats reporting detected on 6 OSD(s)

解决办法：

```
systemctl stop ceph-osd@$OSDID
ceph-bluestore-tool repair –path /var/lib/ceph/osd/ceph-$OSDID
systemctl start ceph-osd@$OSDID
```

参考资料：

【1】http://lists.ceph.com/pipermail/ceph-users-ceph.com/2019-July/036002.html

关于执行ceph-bluestore-tool repair报错问题确认：https://tracker.ceph.com/issues/42297



### 2、3 monitors have not enabled msgr2

问题描述：

​	使用ceph -s命令查看集群状态时，出现 3 monitors have not enabled msgr2

解决办法：

```
ceph mon enable-msgr2
systemctl restart ceph-mon@ceph1.service
```