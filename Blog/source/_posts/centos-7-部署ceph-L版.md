---
title: Centos 7 部署Ceph L版
date: 2018-04-10 22:21:44
tags: CEPH
---

ceph L版已经发布很久了，官方说默认使用BlueStore作为OSD的存储后端，在Cephalocon APAC 2018上也是讨论的焦点之一。

提到BlueStore，不得不说一说Ceph的STORAGE DEVICES。

## STORAGE DEVICES
Ceph守护进程将数据存储在磁盘上：
```
Ceph OSDs ( Object Storage Daemons )
    Client端的大多数数据写入Ceph后被存储的地方，一般而言，每个OSD都由单一存储设备支持，如传统硬盘（HDD）或固态硬盘（SSD）。
    OSD还可以由多种设备组合，如存储数据的HDD和存储某些元数据的SSD（或SSD的分区）。
    群集中OSD的数量通常取决于你要存储的数据量，还需要考虑每个存储设备的容量以及冗余级别和存储类型（replication或erasure coding）。
Ceph Monitor
    管理关键群集状态，如cluster membership和authentication信息。对于较小的集群，需要几千兆字节（几个GB），然而对于较大的集群，monitor的数据库可以达到几十甚至几百千兆（几十个GB甚至几百个GB）。
```

## OSD BACKENDS
OSD可以通过两种方式管理存储的数据。从Luminous 12.2.z发行版开始，新的默认（推荐）后端是 BlueStore。在Luminous之前，默认（也是唯一的选择）是 FileStore。

### BLUESTORE
BlueStore是专门用于Ceph OSD管理磁盘上的数据的专用存储后端。在过去十年间，受到了FileStore管理OSD经验的启发.
BlueStore的主要功能包括：
```
直接管理存储设备 ( Direct management of storage devices )
    BlueStore使用原始块设备或分区。这避免了任何可能限制性能或增加复杂性的抽象层（如像XFS这样的本地文件系统）。

使用RocksDB进行元数据管理 ( Metadata management with RocksDB )
    为了管理内部元数据，我们嵌入了RocksDB的key/value数据库。例如在磁盘上，从object names到block locations的映射。

完整的数据和元数据校验 ( Full data and metadata checksumming )
    默认情况下，写入BlueStore的所有数据和元数据都受到一个或多个校验和的保护。没有数据或元数据在未经过验证的情况下，就从磁盘读取或返回给用户。

内置压缩 ( Inline compression )
    写入的数据在写入磁盘之前可以选择压缩。

多设备元数据分层 ( Multi-device metadata tiering )
    BlueStore允许将其内部journal（预写日志，write-ahead log）写入单独的高速设备（如SSD，NVMe或NVDIMM）以提高性能。
    如果有大量更快速的存储可用，则内部元数据也可以存储在更快的设备上。

高效的写时复制 ( Efficient copy-on-write )
    RBD和CephFS快照依赖于copy-on-write clone机制，也在BlueStore中得到了有效的实现。这将为常规快照和erasure coded池提供高效的IO（依靠clone实现高效的two-phase commits）

```
http://docs.ceph.com/docs/master/rados/configuration/bluestore-config-ref/
http://docs.ceph.com/docs/master/rados/operations/bluestore-migration/

### FILESTORE
FileStore是在Ceph中存储objects的传统方法。它依赖于标准文件系统（通常是XFS）和某个元数据的key/value数据库（传统上是LevelDB，现在是RocksDB）结合使用。
FileStore经过良好测试并广泛用于生产，但由于其整体设计和对传统文件系统存储object数据的依赖性，因此存在许多性能缺陷。
尽管FileStore通常能够在大多数与POSIX兼容的文件系统（包括btrfs和ext4）上运行，但我们只建议使用XFS。
btrfs和ext4都有已知的bug和缺陷，使用它们可能会导致数据丢失。默认情况下，所有的Ceph提供的工具都将使用XFS。

http://docs.ceph.com/docs/master/rados/configuration/filestore-config-ref/

在ceph L版代码结构改动比较大，增加了CEPH-MGR向外部监测和管理系统提供额外的监测接口，今天就用虚拟机搭建实验环境玩一玩。

## 环境信息
```shell
[root@cephL ~]# yum install -y redhat-lsb
[root@cephL ~]# lsb_release -a
LSB Version:	:core-4.1-amd64:core-4.1-noarch:cxx-4.1-amd64:cxx-4.1-noarch:desktop-4.1-amd64:desktop-4.1-noarch:languages-4.1-amd64:languages-4.1-noarch:printing-4.1-amd64:printing-4.1-noarch
Distributor ID:	CentOS
Description:	CentOS Linux release 7.4.1708 (Core)
Release:	7.4.1708
Codename:	Core

[root@cephL ~]# lsblk
NAME            MAJ:MIN RM  SIZE RO TYPE MOUNTPOINT
sda               8:0    0   40G  0 disk
├─sda1            8:1    0    1G  0 part /boot
└─sda2            8:2    0   39G  0 part
  ├─centos-root 253:0    0   36G  0 lvm  /
  └─centos-swap 253:1    0    3G  0 lvm  [SWAP]
sdb               8:16   0   30G  0 disk
sdc               8:32   0   30G  0 disk
sr0              11:0    1 1024M  0 rom
```

## 安装
### 安装pip和ceph-deploy
```shell
[root@cephL ~]# curl "https://bootstrap.pypa.io/get-pip.py" -o "get-pip.py"
[root@cephL ~]# python get-pip.py
[root@cephL ~]# python -m pip install -U pip
[root@cephL ~]# pip install --upgrade setuptools
[root@cephL ~]# pip install ceph-deploy
[root@cephL ~]# ceph-deploy --version
2.0.0
```

### 安装ceph软件包
```shell
[root@cephL ~]# mkdir ceph-deploy && cd ceph-deploy
[root@cephL ceph-deploy]# ceph-deploy install cephL --release luminous
```

### 开始部署一个新的集群，然后为它写一个CLUSTER.conf和keyring

```shell
[root@cephL ceph-deploy]# ceph-deploy new --public-network 192.168.56.101/24  --cluster-network 192.168.56.101/24 cephL
```

### 部署MON

```shell
[root@cephL ceph-deploy]# ceph-deploy mon create-initial
[root@cephL ceph-deploy]# ceph-deploy mon create cephL
ceph        1110       1  0 12:57 ?        00:00:01 /usr/bin/ceph-mon -f --cluster ceph --id cephL --setuser ceph --setgroup ceph
```

### 部署OSD

bluestore方法

```shell
# 在创建osd时，L版默认是bluestore
[root@cephL ceph-deploy]# ceph-deploy osd create --data /dev/sdb cephL
ceph        1514       1  0 12:57 ?        00:00:01 /usr/bin/ceph-osd -f --cluster ceph --id 0 --setuser ceph --setgroup ceph
[root@cephL ceph-deploy]# ceph-deploy osd create --data /dev/sdc cephL
ceph        1518       1  0 12:57 ?        00:00:01 /usr/bin/ceph-osd -f --cluster ceph --id 1 --setuser ceph --setgroup ceph
```

遇到问题

```shell
[root@cephL ceph-deploy]# ceph -s
2018-04-10 12:00:19.660298 7fd1fe0ae700 -1 auth: unable to find a keyring on /etc/ceph/ceph.client.admin.keyring,/etc/ceph/ceph.keyring,/etc/ceph/keyring,/etc/ceph/keyring.bin,: (2) No such file or directory
2018-04-10 12:00:19.660310 7fd1fe0ae700 -1 monclient: ERROR: missing keyring, cannot use cephx for authentication
2018-04-10 12:00:19.660312 7fd1fe0ae700  0 librados: client.admin initialization error (2) No such file or directory
[errno 2] error connecting to the cluster

[root@cephL ceph-deploy]# chmod +r *
[root@cephL ceph-deploy]# cp ceph.client.admin.keyring /etc/ceph/
[root@cephL ceph-deploy]# ceph -s
  cluster:
    id:     765752b7-1f77-4d0d-bc18-936b8ad409fd
    health: HEALTH_WARN
            no active mgr

  services:
    mon: 1 daemons, quorum cephL
    mgr: no daemons active
    osd: 2 osds: 2 up, 2 in

  data:
    pools:   0 pools, 0 pgs
    objects: 0 objects, 0 bytes
    usage:   0 kB used, 0 kB / 0 kB avail
    pgs:
```

filestore方法

```shell
# 如果是filestore则需要对data device和journal device先做GPT partition
--data DATA           The OSD data logical volume (vg/lv) or absolute path to device
--journal JOURNAL     Logical Volume (vg/lv) or path to GPT partition

[root@cephL ceph-deploy]# fdisk /dev/sdb
WARNING: fdisk GPT support is currently new, and therefore in an experimental phase. Use at your own discretion.
欢迎使用 fdisk (util-linux 2.23.2)。
更改将停留在内存中，直到您决定将更改写入磁盘。
使用写入命令前请三思。
命令(输入 m 获取帮助)：g
Building a new GPT disklabel (GUID: 80097CEF-475B-4161-ACC7-7164F6A39DD2)
命令(输入 m 获取帮助)：n
分区号 (1-128，默认 1)：
第一个扇区 (2048-62914526，默认 2048)：
Last sector, +sectors or +size{K,M,G,T,P} (2048-62914526，默认 62914526)：
已创建分区 1
命令(输入 m 获取帮助)：w
The partition table has been altered!
Calling ioctl() to re-read partition table.
正在同步磁盘。

[root@cephL ceph-deploy]# fdisk /dev/sdc
WARNING: fdisk GPT support is currently new, and therefore in an experimental phase. Use at your own discretion.
欢迎使用 fdisk (util-linux 2.23.2)。
更改将停留在内存中，直到您决定将更改写入磁盘。
使用写入命令前请三思。
命令(输入 m 获取帮助)：g
Building a new GPT disklabel (GUID: 21DFA98C-5BCF-40E7-A120-3DEDEA6600ED)
命令(输入 m 获取帮助)：n
分区号 (1-128，默认 1)：
第一个扇区 (2048-62914526，默认 2048)：
Last sector, +sectors or +size{K,M,G,T,P} (2048-62914526，默认 62914526)：
已创建分区 1
命令(输入 m 获取帮助)：w
The partition table has been altered!
Calling ioctl() to re-read partition table.
正在同步磁盘。

[root@cephL ceph-deploy]# lsblk
NAME            MAJ:MIN RM  SIZE RO TYPE MOUNTPOINT
sda               8:0    0   40G  0 disk 
├─sda1            8:1    0    1G  0 part /boot
└─sda2            8:2    0   39G  0 part 
  ├─centos-root 253:0    0   36G  0 lvm  /
  └─centos-swap 253:1    0    3G  0 lvm  [SWAP]
sdb               8:16   0   30G  0 disk 
└─sdb1            8:17   0   30G  0 part 
sdc               8:32   0   30G  0 disk 
└─sdc1            8:33   0   30G  0 part 
sr0              11:0    1 1024M  0 rom  

[root@cephL ceph-deploy]# ceph-deploy osd create --filestore --fs-type xfs --data /dev/sdb1 --journal /dev/sdc1 cephL
[ceph_deploy.conf][DEBUG ] found configuration file at: /root/.cephdeploy.conf
[ceph_deploy.cli][INFO  ] Invoked (2.0.0): /usr/bin/ceph-deploy osd create --filestore --fs-type xfs --data /dev/sdb1 --journal /dev/sdc1 cephL
[ceph_deploy.cli][INFO  ] ceph-deploy options:
[ceph_deploy.cli][INFO  ]  verbose                       : False
[ceph_deploy.cli][INFO  ]  bluestore                     : None
[ceph_deploy.cli][INFO  ]  cd_conf                       : <ceph_deploy.conf.cephdeploy.Conf instance at 0x22c7320>
[ceph_deploy.cli][INFO  ]  cluster                       : ceph
[ceph_deploy.cli][INFO  ]  fs_type                       : xfs
[ceph_deploy.cli][INFO  ]  block_wal                     : None
[ceph_deploy.cli][INFO  ]  default_release               : False
[ceph_deploy.cli][INFO  ]  username                      : None
[ceph_deploy.cli][INFO  ]  journal                       : /dev/sdc1
[ceph_deploy.cli][INFO  ]  subcommand                    : create
[ceph_deploy.cli][INFO  ]  host                          : cephL
[ceph_deploy.cli][INFO  ]  filestore                     : True
[ceph_deploy.cli][INFO  ]  func                          : <function osd at 0x225ae60>
[ceph_deploy.cli][INFO  ]  ceph_conf                     : None
[ceph_deploy.cli][INFO  ]  zap_disk                      : False
[ceph_deploy.cli][INFO  ]  data                          : /dev/sdb1
[ceph_deploy.cli][INFO  ]  block_db                      : None
[ceph_deploy.cli][INFO  ]  dmcrypt                       : False
[ceph_deploy.cli][INFO  ]  overwrite_conf                : False
[ceph_deploy.cli][INFO  ]  dmcrypt_key_dir               : /etc/ceph/dmcrypt-keys
[ceph_deploy.cli][INFO  ]  quiet                         : False
[ceph_deploy.cli][INFO  ]  debug                         : False
[ceph_deploy.osd][DEBUG ] Creating OSD on cluster ceph with data device /dev/sdb1
[cephL][DEBUG ] connected to host: cephL 
[cephL][DEBUG ] detect platform information from remote host
[cephL][DEBUG ] detect machine type
[cephL][DEBUG ] find the location of an executable
[ceph_deploy.osd][INFO  ] Distro info: CentOS Linux 7.4.1708 Core
[ceph_deploy.osd][DEBUG ] Deploying osd to cephL
[cephL][DEBUG ] write cluster configuration to /etc/ceph/{cluster}.conf
[cephL][DEBUG ] find the location of an executable
[cephL][INFO  ] Running command: /usr/sbin/ceph-volume --cluster ceph lvm create --filestore --data /dev/sdb1 --journal /dev/sdc1
[cephL][DEBUG ] Running command: /bin/ceph-authtool --gen-print-key
[cephL][DEBUG ] Running command: /bin/ceph --cluster ceph --name client.bootstrap-osd --keyring /var/lib/ceph/bootstrap-osd/ceph.keyring -i - osd new 8b7be4a6-b563-434e-b030-132880a10d31
[cephL][DEBUG ] Running command: vgcreate --force --yes ceph-8e2515c1-6170-4299-b82c-a5a47681f946 /dev/sdb1
[cephL][DEBUG ]  stdout: Physical volume "/dev/sdb1" successfully created.
[cephL][DEBUG ]  stdout: Volume group "ceph-8e2515c1-6170-4299-b82c-a5a47681f946" successfully created
[cephL][DEBUG ] Running command: lvcreate --yes -l 100%FREE -n osd-data-8b7be4a6-b563-434e-b030-132880a10d31 ceph-8e2515c1-6170-4299-b82c-a5a47681f946
[cephL][DEBUG ]  stdout: Logical volume "osd-data-8b7be4a6-b563-434e-b030-132880a10d31" created.
[cephL][DEBUG ] Running command: /bin/ceph-authtool --gen-print-key
[cephL][DEBUG ] Running command: mkfs -t xfs -f -i size=2048 /dev/ceph-8e2515c1-6170-4299-b82c-a5a47681f946/osd-data-8b7be4a6-b563-434e-b030-132880a10d31
[cephL][DEBUG ]  stdout: meta-data=/dev/ceph-8e2515c1-6170-4299-b82c-a5a47681f946/osd-data-8b7be4a6-b563-434e-b030-132880a10d31 isize=2048   agcount=4, agsize=1965824 blks
[cephL][DEBUG ]          =                       sectsz=512   attr=2, projid32bit=1
[cephL][DEBUG ]          =                       crc=1        finobt=0, sparse=0
[cephL][DEBUG ] data     =                       bsize=4096   blocks=7863296, imaxpct=25
[cephL][DEBUG ]          =                       sunit=0      swidth=0 blks
[cephL][DEBUG ] naming   =version 2              bsize=4096   ascii-ci=0 ftype=1
[cephL][DEBUG ] log      =internal log           bsize=4096   blocks=3839, version=2
[cephL][DEBUG ]          =                       sectsz=512   sunit=0 blks, lazy-count=1
[cephL][DEBUG ] realtime =none                   extsz=4096   blocks=0, rtextents=0
[cephL][DEBUG ] Running command: mount -t xfs -o rw,noatime,inode64 /dev/ceph-8e2515c1-6170-4299-b82c-a5a47681f946/osd-data-8b7be4a6-b563-434e-b030-132880a10d31 /var/lib/ceph/osd/ceph-0
[cephL][DEBUG ] Running command: chown -R ceph:ceph /dev/sdc1
[cephL][DEBUG ] Running command: ln -s /dev/sdc1 /var/lib/ceph/osd/ceph-0/journal
[cephL][DEBUG ] Running command: ceph --cluster ceph --name client.bootstrap-osd --keyring /var/lib/ceph/bootstrap-osd/ceph.keyring mon getmap -o /var/lib/ceph/osd/ceph-0/activate.monmap
[cephL][DEBUG ]  stderr: got monmap epoch 1
[cephL][DEBUG ] Running command: chown -R ceph:ceph /dev/sdc1
[cephL][DEBUG ] Running command: chown -R ceph:ceph /var/lib/ceph/osd/ceph-0/
[cephL][DEBUG ] Running command: ceph-osd --cluster ceph --osd-objectstore filestore --mkfs -i 0 --monmap /var/lib/ceph/osd/ceph-0/activate.monmap --osd-data /var/lib/ceph/osd/ceph-0/ --osd-journal /var/lib/ceph/osd/ceph-0/journal --osd-uuid 8b7be4a6-b563-434e-b030-132880a10d31 --setuser ceph --setgroup ceph
[cephL][DEBUG ]  stderr: 2018-05-07 23:01:34.834993 7f315e466d00 -1 journal check: ondisk fsid 00000000-0000-0000-0000-000000000000 doesn't match expected 8b7be4a6-b563-434e-b030-132880a10d31, invalid (someone else's?) journal
[cephL][DEBUG ]  stderr: 2018-05-07 23:01:34.865621 7f315e466d00 -1 journal do_read_entry(4096): bad header magic
[cephL][DEBUG ] 2018-05-07 23:01:34.865667 7f315e466d00 -1 journal do_read_entry(4096): bad header magic
[cephL][DEBUG ] 2018-05-07 23:01:34.865988 7f315e466d00 -1 read_settings error reading settings: (2) No such file or directory
[cephL][DEBUG ]  stderr: 2018-05-07 23:01:34.916284 7f315e466d00 -1 created object store /var/lib/ceph/osd/ceph-0/ for osd.0 fsid 39f3b85e-ee3c-4d8d-93c2-7f7c8aa47121
[cephL][DEBUG ] Running command: ceph-authtool /var/lib/ceph/osd/ceph-0/keyring --create-keyring --name osd.0 --add-key AQBDavBa0IPpIBAAlQxlaWxNrnTX/uaOMdZEQw==
[cephL][DEBUG ]  stdout: creating /var/lib/ceph/osd/ceph-0/keyring
[cephL][DEBUG ] added entity osd.0 auth auth(auid = 18446744073709551615 key=AQBDavBa0IPpIBAAlQxlaWxNrnTX/uaOMdZEQw== with 0 caps)
[cephL][DEBUG ] Running command: chown -R ceph:ceph /var/lib/ceph/osd/ceph-0/keyring
[cephL][DEBUG ] --> ceph-volume lvm prepare successful for: /dev/sdb1
[cephL][DEBUG ] Running command: ln -snf /dev/sdc1 /var/lib/ceph/osd/ceph-0/journal
[cephL][DEBUG ] Running command: chown -R ceph:ceph /dev/sdc1
[cephL][DEBUG ] Running command: systemctl enable ceph-volume@lvm-0-8b7be4a6-b563-434e-b030-132880a10d31
[cephL][DEBUG ]  stderr: Created symlink from /etc/systemd/system/multi-user.target.wants/ceph-volume@lvm-0-8b7be4a6-b563-434e-b030-132880a10d31.service to /usr/lib/systemd/system/ceph-volume@.service.
[cephL][DEBUG ] Running command: systemctl start ceph-osd@0
[cephL][DEBUG ] --> ceph-volume lvm activate successful for osd ID: 0
[cephL][DEBUG ] --> ceph-volume lvm create successful for: /dev/sdb1
[cephL][INFO  ] checking OSD status...
[cephL][DEBUG ] find the location of an executable
[cephL][INFO  ] Running command: /bin/ceph --cluster=ceph osd stat --format=json
[ceph_deploy.osd][DEBUG ] Host cephL is now ready for osd use.
```

### 移除OSD

```shell
# 使OSD进入out状态
[root@cephL ceph-deploy]# ceph osd out 0
marked out osd.0.
# 观察数据迁移
[root@cephL ceph-deploy]# ceph -w
# 停止对应的OSD进程
[root@cephL ceph-deploy]# sudo systemctl stop ceph-osd@0
# 清除数据
[root@cephL ceph-deploy]# ceph osd purge 0 --yes-i-really-mean-it
purged osd.0
# 在ceph.conf中移除osd配置
[root@cephL ceph-deploy]# vi /etc/ceph/ceph.conf 
```

### 部署CEPH-MGR

```shell
install netstat tool
[root@cephL ~]# yum -y install net-tools

[root@cephL ceph-deploy]# ceph-deploy mgr create cephL:cephLMGR
ceph        1111       1  0 12:57 ?        00:00:08 /usr/bin/ceph-mgr -f --cluster ceph --id cephLMGR --setuser ceph --setgroup ceph
[root@cephL ceph-deploy]# ceph mgr module enable dashboard

open 7000 port
[root@cephL ceph-deploy]# sudo firewall-cmd --zone=public --add-port=7000/tcp --permanent
[root@cephL ceph-deploy]# sudo firewall-cmd --reload
```

相关命令

```shell
[root@cephL ceph-deploy]# ceph mgr module ls
[root@cephL ceph-deploy]# ceph mgr services
[root@cephL ceph-deploy]# ceph tell mgr help
```

### 部署MDS并创建CEPH FS

![](http://docs.ceph.com/docs/master/_images/ditaa-b5a320fc160057a1a7da010b4215489fa66de242.png)

```shell
[root@cephL ceph-deploy]# ceph-deploy mds create cephL
ceph        2150       1  0 13:00 ?        00:00:00 /usr/bin/ceph-mds -f --cluster ceph --id cephL --setuser ceph --setgroup ceph
```

Ceph文件系统至少需要两个RADOS pool，一个用于存储数据，一个用于存储元数据。

配置这些pool时，可以考虑：

​     对元数据pool使用更多的replication数量，因为该pool中的任何数据丢失都可能导致整个文件系统无法访问。

​     为元数据pool使用SSD等较低延迟的存储设备，因为这将直接影响客户端上文件系统操作的延迟。

```shell
ceph osd pool create cephfs_data <pg_num>
ceph osd pool create cephfs_metadata <pg_num>
例如：
[root@cephL ceph-deploy]# ceph osd pool create cephfs_data 32
[root@cephL ceph-deploy]# ceph osd pool create cephfs_metadata 32
```

更改pool的副本数

```shell
ceph osd pool set {poolname} size {num-replicas}
例如：
[root@cephL ceph-deploy]# ceph osd pool set cephfs_data size 1
[root@cephL ceph-deploy]# ceph osd pool set cephfs_data size 1
```

一旦创建了pool，就可以使用fs new命令启用文件系统：

```shell
ceph fs new <fs_name> <metadata> <data>
例如：
ceph fs new cephFS cephfs_metadata cephfs_data
```

一旦创建了文件系统，您的MDS将能够进入active状态。例如，在single MDS system中：

```shell
[root@cephL ceph-deploy]# ceph mds stat
cephFS-1/1/1 up  {0=cephL=up:active}
```

一旦创建了文件系统并且MDS处于active状态，你就可以挂载文件系统了。如果您创建了多个文件系统，在挂载文件系统时，选择使用哪一个。

如果创建了多个文件系统，并且client在挂载时没有指定挂载哪个文件系统，你可以使用ceph fs set-default命令来设置client默认看到的文件系统。

挂载CEPH FS ( File System ) 有两种方式：

**KERNEL DRIVER**

要挂载Ceph文件系统，您可以在知道monitor主机IP地址的情况下使用mount命令，或使用mount.ceph utility将monitor主机名解析为IP地址。例如：

```shell
sudo mkdir /mnt/mycephfs
sudo mount -t ceph 192.168.0.1:6789:/ /mnt/mycephfs
例如：
[root@cephL ceph-deploy]# sudo mount -t ceph 192.168.56.101:6789:/ /mnt/mycephfs
mount error 22 = Invalid argument
Ceph 10.x (Jewel)版本开始，如果使用kernel方式（无论是krbd还是cephFS）官方推荐至少使用4.x的kernel。
如果无法升级linux kernel，那么映射rbd请使用librbd方式，cephFS请使用fuse方式。
```

如果挂载Ceph文件系统时开启了cephx authentication，您必须指定user和secret。

```
sudo mount -t ceph 192.168.0.1:6789:/ /mnt/mycephfs -o name=admin,secret=AQATSKdNGBnwLhAAnNDKnH65FmVKpXZJVasUeQ==
```

上述用法在Bash history中留下了secret。更安全的方法是从文件中读取secret。 例如：

```
sudo mount -t ceph 192.168.0.1:6789:/ /mnt/mycephfs -o name=admin,secretfile=/etc/ceph/admin.secret
```

如果您有多个文件系统，请使用mds_namespace选项指定要挂载的文件系统，例如-o mds_namespace=myfs

要卸载Ceph文件系统，可以使用umount命令。 例如：

```
sudo umount /mnt/mycephfs
提示：在执行此命令之前，请确保您不在挂载的目录中。
```

**FUSE**

在用户空间（FUSE）中挂载Ceph文件系统之前，请确保客户端主机具有Ceph配置文件的副本以及Ceph元数据服务器的CAPS keyring。

在您的客户端主机上，将Ceph配置文件从monitor主机复制到/etc/ceph目录。

```
sudo mkdir -p /etc/ceph
sudo scp {user}@{server-machine}:/etc/ceph/ceph.conf /etc/ceph/ceph.conf
```

在您的客户端主机上，将monitor主机的Ceph keyring复制到/etc/ceph目录。

```
sudo scp {user}@{server-machine}:/etc/ceph/ceph.keyring /etc/ceph/ceph.keyring
```

确保Ceph配置文件和keyring在您的客户端机器上设置了适当的权限（例如，chmod 644）。

要将Ceph文件系统挂在为FUSE，可以使用ceph-fuse命令。 例如：

```
sudo mkdir /home/usernname/cephfs
sudo ceph-fuse -m 192.168.0.1:6789 /home/username/cephfs
```

如果您拥有多个文件系统，请使用 --client_mds_namespace 命令行参数指定要挂载哪一个文件系统，或者向ceph.conf中添加client_mds_namespace设置。

要自动挂载ceph-fuse，您可以在system fstab中添加一个条目。此外还可以使用ceph-fuse@.service和ceph-fuse.target systemd units。通常这些unit文件为ceph-fuse描述默认的dependencies和推荐的execution context。例如使用ceph-fuse挂载到/mnt：

```
sudo systemctl start ceph-fuse@/mnt.service
```

持久化挂载点可通过以下方式进行设置：

```
sudo systemctl enable ceph-fuse@/mnt.service
```

### 部署RGW

![](http://docs.ceph.com/docs/master/_images/ditaa-50d12451eb76c5c72c4574b08f0320b39a42e5f1.png)

Ceph Object Gateway原来叫RADOS Gateway，它是构建在librados之上的对象存储接口，为应用程序提供了一个RESTful gateway，用户可以通过HTTP协议访问Ceph存储集群。

Ceph Object Storage支持两个接口：

- S3-compatible：与Amazon S3 RESTful API中一些子集兼容的接口，提供对象存储功能。

- Swift-compatible：与OpenStack Swift API中一些子集兼容的接口，提供对象存储功能。

Ceph Object Storage使用Ceph Object Gateway daemon (radosgw)，它是一个HTTP server，用于与Ceph存储集群进行交互。由于它提供了与OpenStack Swift和Amazon S3兼容的接口，因此Ceph Object Gateway具有自己的用户管理。Ceph Object Gateway可以将数据存储在与Ceph Filesystem和Ceph Block Device相同的Ceph存储集群中。但是我相信在生产环境中不会这么做，如果数据量大的话会影响Ceph Filesystem和Ceph Block Device的性能，个人一般会独立出一个Ceph Object Gateway集群。S3和Swift API共享一个通用的namespace，因此您可以使用一个API编写数据并使用另一个API检索它。

```
Note：Ceph Object Storage 不使用 Ceph Metadata Server
```





```shell
# 必须部署MGR，才能部署RGW

[root@cephL ceph-deploy]# ceph-deploy rgw create cephL:RGW
root        2799       1  0 13:13 ?        00:00:00 /usr/bin/radosgw -f --cluster ceph --name client.rgw.RGW --setuser ceph --setgroup ceph

# 重启RGW
[root@cephL ~]# systemctl restart ceph-radosgw@rgw.cephL.service
[root@cephL ~]# systemctl restart ceph-radosgw@rgw

问题一，这难道是ceph-deploy 2.0.0的坑？
[root@cephL ~]# tailf /var/log/ceph/ceph-client.rgw.log
2018-05-11 22:30:31.999421 7f537c31fe00  0 ceph version 12.2.4 (52085d5249a80c5f5121a76d6288429f35e4e77b) luminous (stable), process (unknown), pid 3450
2018-05-11 22:30:32.021546 7f537c31fe00 -1 auth: unable to find a keyring on /var/lib/ceph/radosgw/ceph-rgw/keyring: (2) No such file or directory
2018-05-11 22:30:32.021561 7f537c31fe00 -1 monclient: ERROR: missing keyring, cannot use cephx for authentication
2018-05-11 22:30:32.021563 7f537c31fe00  0 librados: client.rgw initialization error (2) No such file or directory
2018-05-11 22:30:32.022900 7f537c31fe00 -1 Couldn't init storage provider (RADOS)

[root@cephL radosgw]# pwd
/var/lib/ceph/radosgw
[root@cephL radosgw]# ls
ceph-rgw.RGW
[root@cephL radosgw]# mv ceph-rgw.RGW  ceph-rgw 


```



### 配置变动

在L版中，删除pool的操作做了强制限制。需要在/etc/ceph/ceph.conf中加入相关参数才允许删除pool。

```
# 允许删除pool，需要添加
mon allow pool delete = true
```

