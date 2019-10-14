---
title: UPGRADING CEPH
date: 2019-10-12 10:55:29
tags: ceph
---

# SUMMARY

您可以在群集处于online并in service状态时升级Ceph群集中的daemons！某些类型的daemons依赖于其他daemon。例如，Ceph
Metadata Servers和Ceph Object Gateways依赖于Ceph
Monitors和Ceph OSD Daemons。我们建议按以下顺序升级：

1、Ceph Deploy

2、Ceph Monitors

3、Ceph OSD Daemons

4、Ceph Metadata Servers

5、Ceph Object Gateways

通常，我们建议升级所有daemons以确保它们都在同一版本上（例如，所有ceph-mon daemons，所有ceph-osd daemons等）。我们还建议您先升级集群中的所有daemons，然后再尝试使用新功能。

[Upgrade Procedures](https://docs.ceph.com/docs/master/install/upgrading-ceph/#upgrade-procedures)比较简单，在升级之前需要查看[release notes document of your release](https://docs.ceph.com/docs/master/releases)。基础过程包括三个步骤：

1、 在admin节点上使用ceph-deploy为多个host升级packages（使用ceph-deploy install命令），或者登陆到每个host使用package manager升级Ceph package。例如，在升级Monitors时，ceph-deploy语法可能如下所示： 

```
ceph-deploy install --release {release-name} ceph-node1[ ceph-node2]

ceph-deploy install --release firefly mon1 mon2 mon3
```

注意：ceph-deploy install命令会将指定节点中的packages从旧版本升级到你指定的版本。（该工具不存在ceph-deploy upgrade命令）

2、 登陆到每个Ceph节点，然后重新启动每个Ceph daemon，有关详细信息，请参见[Operating a Cluster](https://docs.ceph.com/docs/master/rados/operations/operating)。

3、 确保您的群集healthy。有关详细信息，请参见[Monitoring a Cluster](https://docs.ceph.com/docs/master/rados/operations/monitoring)。

`重要说明：升级daemon后，将无法降级。`

# CEPH DEPLOY

在升级Ceph daemons之前，先升级ceph-deploy工具。

```
sudo pip install -U ceph-deploy
```

或

```
sudo apt-get install ceph-deploy
```

或

```
sudo yum install ceph-deploy python-pushy
```

# UPGRADE PROCEDURES

以下各节描述了升级过程。

重要说明：每个Ceph版本可能都有一些其他步骤。在开始升级daemons之前，请详细阅读[release notes document of your release](https://docs.ceph.com/docs/master/releases)。

## UPGRADING MONITORS

要升级monitors，执行以下步骤：

1、为每个daemon instance升级Ceph package。

可以使用ceph-deploy一次指定所有的monitor节点。例如：

```
ceph-deploy install --release {release-name} ceph-node1[ ceph-node2]
ceph-deploy install --release hammer mon1 mon2 mon3
```

你也可以在每个节点上使用[distro’s package manager](https://docs.ceph.com/docs/master/install/install-storage-cluster/)。对于Debian/Ubuntu，在每个主机上执行以下步骤：

```
ssh {osd-host}
sudo apt-get update && sudo apt-get install ceph
```

对于CentOS/Red Hat，执行以下步骤：

```
ssh {osd-host}
sudo yum update && sudo yum install ceph
```

2、重新启动每个monitor，对于Ubuntu

```
sudo restart ceph-mon id={hostname}
```

对于CentOS/Red Hat/Debian，使用：

```
sudo /etc/init.d/ceph restart {mon-id}
```

对于CentOS/Red Hat通过ceph-deploy部署的集群，monitor ID通常为mon.{hostname}。

3、确保每个monitor都重新quorum

```
ceph mon stat
```

确保你已完成所有Ceph Monitor的升级步骤。

## UPGRADING AN OSD

要升级Ceph OSD Daemon，请执行以下步骤：

1、 升级Ceph OSD Daemon package。

可以使用ceph-deploy一次指定所有的monitor节点。例如：

```
ceph-deploy install --release {release-name} ceph-node1[ ceph-node2]
ceph-deploy install --release hammer osd1 osd2 osd3
```

你也可以在每个节点上使用[distro’s package manager](https://docs.ceph.com/docs/master/install/install-storage-cluster/)。对于Debian/Ubuntu，在每个主机上执行以下步骤：

```
ssh {osd-host}
sudo apt-get update && sudo apt-get install ceph
```

对于CentOS / Red Hat，执行以下步骤：

```
ssh {osd-host}
sudo yum update && sudo yum install ceph
```

2、重新启动OSD，其中N是OSD number。对于Ubuntu，请使用：

```
sudo restart ceph-osd id=N
```

对于主机上的多个OSD，可以使用Upstart重新启动所有OSD。

```
sudo restart ceph-osd-all
```

对于CentOS/Red Hat/Debian，使用：

```
sudo /etc/init.d/ceph restart N
```

3、确保每个升级的Ceph OSD Daemon都已重新加入集群：

```
ceph osd stat
```

确保你已完成所有Ceph OSD Daemons的升级步骤。

## UPGRADING A METADATA SERVER

要升级Ceph Metadata Server，请执行以下步骤：

1、升级Ceph Metadata Server package。可以使用ceph-deploy一次指定所有的Ceph Metadata Server节点，或在每个节点上使用package manager。例如：

```
ceph-deploy install --release {release-name} ceph-node1
ceph-deploy install --release hammer mds1
```

要手动升级packages，请在每个Debian/Ubuntu节点上执行以下步骤：

```
ssh {mon-host}
sudo apt-get update && sudo apt-get install ceph-mds
```

或在CentOS/Red Hat节点上执行：

```
ssh {mon-host}
sudo yum update && sudo yum install ceph-mds
```

2、重新启动metadata server。对于Ubuntu，请使用：

```
sudo restart ceph-mds id={hostname}
```

对于CentOS/Red Hat/Debian，使用：

```
sudo /etc/init.d/ceph restart mds.{hostname}
```

对于使用ceph-deploy部署的集群，name通常是您在创建时指定的name或hostname。

3、确保metadata server已启动并正在运行：

```
ceph mds stat
```

## UPGRADING A CLIENT

升级packages并在Ceph集群上重新启动daemons后，我们建议您也升级client节点上的ceph-common和client libraries（librbd1和librados2）。

1、 升级package。

```
ssh {client-host}
apt-get update && sudo apt-get install ceph-common librados2 librbd1 python-rados python-rbd
```

2、确保已升级为新版本

```
ceph --version
```

如果没有升级为最新版本，则需要卸载，auto remove dependencies并重新安装。

# 实践

## 升级Ceph服务端package

### 升级可能带来的影响

```
（1）服务端升级可能会影响客户业务，导致业务中断一定时间。如果cephfs或rgw前端有流量（install ceph package时会自动停掉mds与rgw service），请先将升级节点流量先转向其他节点。
（2）如果容器中有ceph client packages，同样需要逐一转走流量，再升级
```

1、查看当前环境与版本

```
[root@ceph2 ~]# ceph -v
ceph version 12.2.12 (1436006594665279fe734b4c15d7e08c13ebd777) luminous (stable)

[root@ceph2 ~]# cat /etc/redhat-release 
CentOS Linux release 7.6.1810 (Core) 

[root@ceph2 ~]# uname -a
Linux ceph2 3.10.0-957.el7.x86_64 #1 SMP Thu Nov 8 23:39:32 UTC 2018 x86_64 x86_64 x86_64 GNU/Linux

[root@ceph2 ~]# ceph -s
  cluster:
    id:     c4051efa-1997-43ef-8497-fb02bdf08233
    health: HEALTH_OK
 
  services:
    mon: 3 daemons, quorum ceph1,ceph3,ceph2
    mgr: ceph2(active), standbys: ceph3, ceph1
    mds: cephfs-1/1/1 up  {0=ceph1=up:active}, 2 up:standby
    osd: 6 osds: 6 up, 6 in
    rgw: 3 daemons active
 
  data:
    pools:   7 pools, 176 pgs
    objects: 244 objects, 5.27KiB
    usage:   6.04GiB used, 293GiB / 299GiB avail
    pgs:     176 active+clean
 
  io:
    client:   2.00KiB/s rd, 0B/s wr, 1op/s rd, 1op/s wr

[root@ceph2 ~]# ceph osd tree
ID CLASS WEIGHT  TYPE NAME      STATUS REWEIGHT PRI-AFF 
-1       0.29214 root default                           
-5       0.09738     host ceph1                         
 1   hdd 0.04869         osd.1      up  1.00000 1.00000 
 4   hdd 0.04869         osd.4      up  1.00000 1.00000 
-7       0.09738     host ceph2                         
 2   hdd 0.04869         osd.2      up  1.00000 1.00000 
 5   hdd 0.04869         osd.5      up  1.00000 1.00000 
-3       0.09738     host ceph3                         
 0   hdd 0.04869         osd.0      up  1.00000 1.00000 
 3   hdd 0.04869         osd.3      up  1.00000 1.00000 
 
[root@ceph2 ~]# rpm -qa | grep ceph
libcephfs2-12.2.12-0.el7.x86_64
ceph-common-12.2.12-0.el7.x86_64
ceph-radosgw-12.2.12-0.el7.x86_64
ceph-base-12.2.12-0.el7.x86_64
ceph-osd-12.2.12-0.el7.x86_64
ceph-mds-12.2.12-0.el7.x86_64
python-cephfs-12.2.12-0.el7.x86_64
ceph-selinux-12.2.12-0.el7.x86_64
ceph-mon-12.2.12-0.el7.x86_64
ceph-mgr-12.2.12-0.el7.x86_64

[root@ceph2 ~]# rpm -qa | grep rbd
python-rbd-12.2.12-0.el7.x86_64
librbd1-12.2.12-0.el7.x86_64

[root@ceph2 ~]# rpm -qa | grep rados
ceph-radosgw-12.2.12-0.el7.x86_64
librados2-12.2.12-0.el7.x86_64
python-rados-12.2.12-0.el7.x86_64
libradosstriper1-12.2.12-0.el7.x86_64
```

2、修改每台ceph节点的ceph mirror，L版地址修改为N版地址

```
[root@ceph2 ~]# vim /etc/yum.repos.d/ceph_stable.repo 
[ceph_stable]
baseurl = http://mirrors.163.com/ceph/rpm-nautilus/el7/$basearch
gpgcheck = 1
gpgkey = http://mirrors.163.com/ceph/keys/release.asc
name = Ceph Stable repo
```

3、设置noout 标志停机维护

```
[root@ceph2 ~]# ceph osd set noout

[root@ceph2 ~]# ceph -s
  cluster:
    id:     c4051efa-1997-43ef-8497-fb02bdf08233
    health: HEALTH_WARN
            insufficient standby MDS daemons available
            noout flag(s) set
 
  services:
    mon: 3 daemons, quorum ceph1,ceph3,ceph2
    mgr: ceph1(active), standbys: ceph3, ceph2
    mds: cephfs-1/1/1 up  {0=ceph2=up:active}
    osd: 6 osds: 6 up, 6 in
         flags noout
    rgw: 2 daemons active
 
  data:
    pools:   7 pools, 176 pgs
    objects: 245 objects, 5.30KiB
    usage:   6.05GiB used, 293GiB / 299GiB avail
    pgs:     176 active+clean
```

4、升级每台ceph节点的 ceph packages（一个节点一个节点的升）

升级顺序

```
1、Ceph Monitors
2、Ceph Mgr
3、Ceph OSD Daemons
4、Ceph Metadata Servers
5、Ceph Object Gateways
```

升级命令（`注意：安装过程中rgw与mds服务会自动被stop`）

```
[root@ceph2 ~]# yum install ceph
Loaded plugins: fastestmirror
Determining fastest mirrors
epel/x86_64/metalink                                                                                                                                                                                                  | 6.5 kB  00:00:00     
 * base: mirrors.huaweicloud.com
 * epel: mirrors.aliyun.com
 * extras: mirrors.huaweicloud.com
 * updates: mirrors.huaweicloud.com
base                                                                                                                                                                                                                  | 3.6 kB  00:00:00     
ceph_stable                                                                                                                                                                                                           | 2.9 kB  00:00:00     
epel                                                                                                                                                                                                                  | 5.3 kB  00:00:00     
extras                                                                                                                                                                                                                | 2.9 kB  00:00:00     
updates                                                                                                                                                                                                               | 2.9 kB  00:00:00     
(1/8): epel/x86_64/group_gz                                                                                                                                                                                           |  88 kB  00:00:00     
(2/8): ceph_stable/x86_64/primary_db                                                                                                                                                                                  | 192 kB  00:00:01     
(3/8): base/7/x86_64/group_gz                                                                                                                                                                                         | 165 kB  00:00:01     
(4/8): epel/x86_64/updateinfo                                                                                                                                                                                         | 1.0 MB  00:00:02     
(5/8): extras/7/x86_64/primary_db                                                                                                                                                                                     | 152 kB  00:00:01     
(6/8): base/7/x86_64/primary_db                                                                                                                                                                                       | 6.0 MB  00:00:04     
(7/8): updates/7/x86_64/primary_db                                                                                                                                                                                    | 1.9 MB  00:00:05     
(8/8): epel/x86_64/primary_db                                                                                                                                                                                         | 6.9 MB  00:00:16     
Resolving Dependencies
--> Running transaction check
---> Package ceph.x86_64 2:14.2.4-0.el7 will be installed
--> Processing Dependency: ceph-osd = 2:14.2.4-0.el7 for package: 2:ceph-14.2.4-0.el7.x86_64
--> Processing Dependency: ceph-mds = 2:14.2.4-0.el7 for package: 2:ceph-14.2.4-0.el7.x86_64
--> Processing Dependency: ceph-mgr = 2:14.2.4-0.el7 for package: 2:ceph-14.2.4-0.el7.x86_64
--> Processing Dependency: ceph-mon = 2:14.2.4-0.el7 for package: 2:ceph-14.2.4-0.el7.x86_64
--> Running transaction check
---> Package ceph-mds.x86_64 2:12.2.12-0.el7 will be updated
---> Package ceph-mds.x86_64 2:14.2.4-0.el7 will be an update
--> Processing Dependency: ceph-base = 2:14.2.4-0.el7 for package: 2:ceph-mds-14.2.4-0.el7.x86_64
--> Processing Dependency: librdmacm.so.1()(64bit) for package: 2:ceph-mds-14.2.4-0.el7.x86_64
---> Package ceph-mgr.x86_64 2:12.2.12-0.el7 will be updated
---> Package ceph-mgr.x86_64 2:14.2.4-0.el7 will be an update
--> Processing Dependency: python-bcrypt for package: 2:ceph-mgr-14.2.4-0.el7.x86_64
---> Package ceph-mon.x86_64 2:12.2.12-0.el7 will be updated
---> Package ceph-mon.x86_64 2:14.2.4-0.el7 will be an update
---> Package ceph-osd.x86_64 2:12.2.12-0.el7 will be updated
---> Package ceph-osd.x86_64 2:14.2.4-0.el7 will be an update
--> Processing Dependency: libstoragemgmt for package: 2:ceph-osd-14.2.4-0.el7.x86_64
--> Running transaction check
---> Package ceph-base.x86_64 2:12.2.12-0.el7 will be updated
--> Processing Dependency: ceph-base = 2:12.2.12-0.el7 for package: 2:ceph-selinux-12.2.12-0.el7.x86_64
--> Processing Dependency: ceph-base = 2:12.2.12-0.el7 for package: 2:ceph-selinux-12.2.12-0.el7.x86_64
--> Processing Dependency: ceph-base = 2:12.2.12-0.el7 for package: 2:ceph-radosgw-12.2.12-0.el7.x86_64
---> Package ceph-base.x86_64 2:14.2.4-0.el7 will be an update
--> Processing Dependency: librados2 = 2:14.2.4-0.el7 for package: 2:ceph-base-14.2.4-0.el7.x86_64
--> Processing Dependency: libcephfs2 = 2:14.2.4-0.el7 for package: 2:ceph-base-14.2.4-0.el7.x86_64
--> Processing Dependency: librgw2 = 2:14.2.4-0.el7 for package: 2:ceph-base-14.2.4-0.el7.x86_64
--> Processing Dependency: librbd1 = 2:14.2.4-0.el7 for package: 2:ceph-base-14.2.4-0.el7.x86_64
--> Processing Dependency: ceph-common = 2:14.2.4-0.el7 for package: 2:ceph-base-14.2.4-0.el7.x86_64
--> Processing Dependency: liboath.so.0(LIBOATH_1.10.0)(64bit) for package: 2:ceph-base-14.2.4-0.el7.x86_64
--> Processing Dependency: liboath.so.0(LIBOATH_1.12.0)(64bit) for package: 2:ceph-base-14.2.4-0.el7.x86_64
--> Processing Dependency: liboath.so.0(LIBOATH_1.2.0)(64bit) for package: 2:ceph-base-14.2.4-0.el7.x86_64
--> Processing Dependency: liboath.so.0()(64bit) for package: 2:ceph-base-14.2.4-0.el7.x86_64
---> Package librdmacm.x86_64 0:22.1-3.el7 will be installed
---> Package libstoragemgmt.x86_64 0:1.7.3-3.el7 will be installed
--> Processing Dependency: libstoragemgmt-python for package: libstoragemgmt-1.7.3-3.el7.x86_64
--> Processing Dependency: libyajl.so.2()(64bit) for package: libstoragemgmt-1.7.3-3.el7.x86_64
--> Processing Dependency: libconfig.so.9()(64bit) for package: libstoragemgmt-1.7.3-3.el7.x86_64
---> Package python2-bcrypt.x86_64 0:3.1.6-2.el7 will be installed
--> Processing Dependency: python-cffi for package: python2-bcrypt-3.1.6-2.el7.x86_64
--> Processing Dependency: python2-six for package: python2-bcrypt-3.1.6-2.el7.x86_64
--> Running transaction check
---> Package ceph-common.x86_64 2:12.2.12-0.el7 will be updated
---> Package ceph-common.x86_64 2:14.2.4-0.el7 will be an update
--> Processing Dependency: libradosstriper1 = 2:14.2.4-0.el7 for package: 2:ceph-common-14.2.4-0.el7.x86_64
--> Processing Dependency: python-cephfs = 2:14.2.4-0.el7 for package: 2:ceph-common-14.2.4-0.el7.x86_64
--> Processing Dependency: python-rbd = 2:14.2.4-0.el7 for package: 2:ceph-common-14.2.4-0.el7.x86_64
--> Processing Dependency: python-ceph-argparse = 2:14.2.4-0.el7 for package: 2:ceph-common-14.2.4-0.el7.x86_64
--> Processing Dependency: python-rados = 2:14.2.4-0.el7 for package: 2:ceph-common-14.2.4-0.el7.x86_64
--> Processing Dependency: python-rgw = 2:14.2.4-0.el7 for package: 2:ceph-common-14.2.4-0.el7.x86_64
--> Processing Dependency: librabbitmq.so.4()(64bit) for package: 2:ceph-common-14.2.4-0.el7.x86_64
---> Package ceph-radosgw.x86_64 2:12.2.12-0.el7 will be updated
---> Package ceph-radosgw.x86_64 2:14.2.4-0.el7 will be an update
---> Package ceph-selinux.x86_64 2:12.2.12-0.el7 will be updated
---> Package ceph-selinux.x86_64 2:14.2.4-0.el7 will be an update
--> Processing Dependency: selinux-policy-base >= 3.13.1-229.el7_6.15 for package: 2:ceph-selinux-14.2.4-0.el7.x86_64
---> Package libcephfs2.x86_64 2:12.2.12-0.el7 will be updated
---> Package libcephfs2.x86_64 2:14.2.4-0.el7 will be an update
---> Package libconfig.x86_64 0:1.4.9-5.el7 will be installed
---> Package liboath.x86_64 0:2.6.2-1.el7 will be installed
---> Package librados2.x86_64 2:12.2.12-0.el7 will be updated
---> Package librados2.x86_64 2:14.2.4-0.el7 will be an update
---> Package librbd1.x86_64 2:12.2.12-0.el7 will be updated
---> Package librbd1.x86_64 2:14.2.4-0.el7 will be an update
---> Package librgw2.x86_64 2:12.2.12-0.el7 will be updated
---> Package librgw2.x86_64 2:14.2.4-0.el7 will be an update
---> Package libstoragemgmt-python.noarch 0:1.7.3-3.el7 will be installed
--> Processing Dependency: libstoragemgmt-python-clibs for package: libstoragemgmt-python-1.7.3-3.el7.noarch
---> Package python-cffi.x86_64 0:1.6.0-5.el7 will be installed
--> Processing Dependency: python-pycparser for package: python-cffi-1.6.0-5.el7.x86_64
---> Package python2-six.noarch 0:1.9.0-0.el7 will be installed
---> Package yajl.x86_64 0:2.0.4-4.el7 will be installed
--> Running transaction check
---> Package librabbitmq.x86_64 0:0.8.0-2.el7 will be installed
---> Package libradosstriper1.x86_64 2:12.2.12-0.el7 will be updated
---> Package libradosstriper1.x86_64 2:14.2.4-0.el7 will be an update
---> Package libstoragemgmt-python-clibs.x86_64 0:1.7.3-3.el7 will be installed
---> Package python-ceph-argparse.x86_64 2:14.2.4-0.el7 will be installed
---> Package python-cephfs.x86_64 2:12.2.12-0.el7 will be updated
---> Package python-cephfs.x86_64 2:14.2.4-0.el7 will be an update
---> Package python-pycparser.noarch 0:2.14-1.el7 will be installed
--> Processing Dependency: python-ply for package: python-pycparser-2.14-1.el7.noarch
---> Package python-rados.x86_64 2:12.2.12-0.el7 will be updated
---> Package python-rados.x86_64 2:14.2.4-0.el7 will be an update
---> Package python-rbd.x86_64 2:12.2.12-0.el7 will be updated
---> Package python-rbd.x86_64 2:14.2.4-0.el7 will be an update
---> Package python-rgw.x86_64 2:12.2.12-0.el7 will be updated
---> Package python-rgw.x86_64 2:14.2.4-0.el7 will be an update
---> Package selinux-policy-targeted.noarch 0:3.13.1-229.el7 will be updated
---> Package selinux-policy-targeted.noarch 0:3.13.1-252.el7.1 will be an update
--> Processing Dependency: selinux-policy = 3.13.1-252.el7.1 for package: selinux-policy-targeted-3.13.1-252.el7.1.noarch
--> Processing Dependency: selinux-policy = 3.13.1-252.el7.1 for package: selinux-policy-targeted-3.13.1-252.el7.1.noarch
--> Running transaction check
---> Package python-ply.noarch 0:3.4-11.el7 will be installed
---> Package selinux-policy.noarch 0:3.13.1-229.el7 will be updated
---> Package selinux-policy.noarch 0:3.13.1-252.el7.1 will be an update
--> Finished Dependency Resolution

Dependencies Resolved

=============================================================================================================================================================================================================================================
 Package                                                              Arch                                            Version                                                     Repository                                            Size
=============================================================================================================================================================================================================================================
Installing:
 ceph                                                                 x86_64                                          2:14.2.4-0.el7                                              ceph_stable                                          3.0 k
Installing for dependencies:
 libconfig                                                            x86_64                                          1.4.9-5.el7                                                 base                                                  59 k
 liboath                                                              x86_64                                          2.6.2-1.el7                                                 epel                                                  51 k
 librabbitmq                                                          x86_64                                          0.8.0-2.el7                                                 base                                                  37 k
 librdmacm                                                            x86_64                                          22.1-3.el7                                                  base                                                  63 k
 libstoragemgmt                                                       x86_64                                          1.7.3-3.el7                                                 base                                                 243 k
 libstoragemgmt-python                                                noarch                                          1.7.3-3.el7                                                 base                                                 167 k
 libstoragemgmt-python-clibs                                          x86_64                                          1.7.3-3.el7                                                 base                                                  19 k
 python-ceph-argparse                                                 x86_64                                          2:14.2.4-0.el7                                              ceph_stable                                           36 k
 python-cffi                                                          x86_64                                          1.6.0-5.el7                                                 base                                                 218 k
 python-ply                                                           noarch                                          3.4-11.el7                                                  base                                                 123 k
 python-pycparser                                                     noarch                                          2.14-1.el7                                                  base                                                 104 k
 python2-bcrypt                                                       x86_64                                          3.1.6-2.el7                                                 epel                                                  39 k
 python2-six                                                          noarch                                          1.9.0-0.el7                                                 epel                                                 2.9 k
 yajl                                                                 x86_64                                          2.0.4-4.el7                                                 base                                                  39 k
Updating for dependencies:
 ceph-base                                                            x86_64                                          2:14.2.4-0.el7                                              ceph_stable                                          5.4 M
 ceph-common                                                          x86_64                                          2:14.2.4-0.el7                                              ceph_stable                                           18 M
 ceph-mds                                                             x86_64                                          2:14.2.4-0.el7                                              ceph_stable                                          1.8 M
 ceph-mgr                                                             x86_64                                          2:14.2.4-0.el7                                              ceph_stable                                          1.5 M
 ceph-mon                                                             x86_64                                          2:14.2.4-0.el7                                              ceph_stable                                          4.5 M
 ceph-osd                                                             x86_64                                          2:14.2.4-0.el7                                              ceph_stable                                           16 M
 ceph-radosgw                                                         x86_64                                          2:14.2.4-0.el7                                              ceph_stable                                          5.3 M
 ceph-selinux                                                         x86_64                                          2:14.2.4-0.el7                                              ceph_stable                                           21 k
 libcephfs2                                                           x86_64                                          2:14.2.4-0.el7                                              ceph_stable                                          480 k
 librados2                                                            x86_64                                          2:14.2.4-0.el7                                              ceph_stable                                          3.3 M
 libradosstriper1                                                     x86_64                                          2:14.2.4-0.el7                                              ceph_stable                                          342 k
 librbd1                                                              x86_64                                          2:14.2.4-0.el7                                              ceph_stable                                          1.6 M
 librgw2                                                              x86_64                                          2:14.2.4-0.el7                                              ceph_stable                                          4.6 M
 python-cephfs                                                        x86_64                                          2:14.2.4-0.el7                                              ceph_stable                                           91 k
 python-rados                                                         x86_64                                          2:14.2.4-0.el7                                              ceph_stable                                          190 k
 python-rbd                                                           x86_64                                          2:14.2.4-0.el7                                              ceph_stable                                          171 k
 python-rgw                                                           x86_64                                          2:14.2.4-0.el7                                              ceph_stable                                           76 k
 selinux-policy                                                       noarch                                          3.13.1-252.el7.1                                            updates                                              492 k
 selinux-policy-targeted                                              noarch                                          3.13.1-252.el7.1                                            updates                                              7.0 M

Transaction Summary
=============================================================================================================================================================================================================================================
Install  1 Package  (+14 Dependent packages)
Upgrade             ( 19 Dependent packages)

Total download size: 72 M
Is this ok [y/d/N]: y
Downloading packages:
Delta RPMs disabled because /usr/bin/applydeltarpm not installed.
(1/34): ceph-14.2.4-0.el7.x86_64.rpm                                                                                                                                                                                  | 3.0 kB  00:00:00     
(2/34): ceph-common-14.2.4-0.el7.x86_64.rpm                                                                                                                                                                           |  18 MB  00:00:19     
(3/34): ceph-mds-14.2.4-0.el7.x86_64.rpm                                                                                                                                                                              | 1.8 MB  00:00:01     
(4/34): ceph-mgr-14.2.4-0.el7.x86_64.rpm                                                                                                                                                                              | 1.5 MB  00:00:03     
(5/34): ceph-mon-14.2.4-0.el7.x86_64.rpm                                                                                                                                                                              | 4.5 MB  00:00:05     
(6/34): ceph-osd-14.2.4-0.el7.x86_64.rpm                                                                                                                                                                              |  16 MB  00:00:14     
(7/34): ceph-radosgw-14.2.4-0.el7.x86_64.rpm                                                                                                                                                                          | 5.3 MB  00:00:07     
(8/34): ceph-selinux-14.2.4-0.el7.x86_64.rpm                                                                                                                                                                          |  21 kB  00:00:00     
(9/34): libconfig-1.4.9-5.el7.x86_64.rpm                                                                                                                                                                              |  59 kB  00:00:00     
(10/34): librabbitmq-0.8.0-2.el7.x86_64.rpm                                                                                                                                                                           |  37 kB  00:00:00     
(11/34): libcephfs2-14.2.4-0.el7.x86_64.rpm                                                                                                                                                                           | 480 kB  00:00:00     
(12/34): librados2-14.2.4-0.el7.x86_64.rpm                                                                                                                                                                            | 3.3 MB  00:00:05     
(13/34): libradosstriper1-14.2.4-0.el7.x86_64.rpm                                                                                                                                                                     | 342 kB  00:00:00     
(14/34): librdmacm-22.1-3.el7.x86_64.rpm                                                                                                                                                                              |  63 kB  00:00:00     
(15/34): liboath-2.6.2-1.el7.x86_64.rpm                                                                                                                                                                               |  51 kB  00:00:07     
(16/34): librbd1-14.2.4-0.el7.x86_64.rpm                                                                                                                                                                              | 1.6 MB  00:00:01     
(17/34): libstoragemgmt-python-1.7.3-3.el7.noarch.rpm                                                                                                                                                                 | 167 kB  00:00:00     
(18/34): libstoragemgmt-1.7.3-3.el7.x86_64.rpm                                                                                                                                                                        | 243 kB  00:00:00     
(19/34): libstoragemgmt-python-clibs-1.7.3-3.el7.x86_64.rpm                                                                                                                                                           |  19 kB  00:00:00     
(20/34): librgw2-14.2.4-0.el7.x86_64.rpm                                                                                                                                                                              | 4.6 MB  00:00:02     
(21/34): python-ceph-argparse-14.2.4-0.el7.x86_64.rpm                                                                                                                                                                 |  36 kB  00:00:00     
(22/34): python-ply-3.4-11.el7.noarch.rpm                                                                                                                                                                             | 123 kB  00:00:00     
(23/34): python-cffi-1.6.0-5.el7.x86_64.rpm                                                                                                                                                                           | 218 kB  00:00:00     
(24/34): python-cephfs-14.2.4-0.el7.x86_64.rpm                                                                                                                                                                        |  91 kB  00:00:00     
(25/34): python-pycparser-2.14-1.el7.noarch.rpm                                                                                                                                                                       | 104 kB  00:00:00     
(26/34): python-rados-14.2.4-0.el7.x86_64.rpm                                                                                                                                                                         | 190 kB  00:00:00     
(27/34): python-rbd-14.2.4-0.el7.x86_64.rpm                                                                                                                                                                           | 171 kB  00:00:00     
(28/34): python2-six-1.9.0-0.el7.noarch.rpm                                                                                                                                                                           | 2.9 kB  00:00:00     
(29/34): python2-bcrypt-3.1.6-2.el7.x86_64.rpm                                                                                                                                                                        |  39 kB  00:00:00     
(30/34): python-rgw-14.2.4-0.el7.x86_64.rpm                                                                                                                                                                           |  76 kB  00:00:00     
(31/34): selinux-policy-3.13.1-252.el7.1.noarch.rpm                                                                                                                                                                   | 492 kB  00:00:00     
(32/34): yajl-2.0.4-4.el7.x86_64.rpm                                                                                                                                                                                  |  39 kB  00:00:00     
(33/34): selinux-policy-targeted-3.13.1-252.el7.1.noarch.rpm                                                                                                                                                          | 7.0 MB  00:00:02     
(34/34): ceph-base-14.2.4-0.el7.x86_64.rpm                                                                                                                                                                            | 5.4 MB  00:01:13     
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Total                                                                                                                                                                                                        1.0 MB/s |  72 MB  00:01:13     
Running transaction check
Running transaction test
Transaction test succeeded
Running transaction
  Installing : librdmacm-22.1-3.el7.x86_64                                                                                                                                                                                              1/53 
  Updating   : 2:librados2-14.2.4-0.el7.x86_64                                                                                                                                                                                          2/53 
  Updating   : 2:python-rados-14.2.4-0.el7.x86_64                                                                                                                                                                                       3/53 
  Installing : liboath-2.6.2-1.el7.x86_64                                                                                                                                                                                               4/53 
  Updating   : 2:librbd1-14.2.4-0.el7.x86_64                                                                                                                                                                                            5/53 
  Updating   : 2:libcephfs2-14.2.4-0.el7.x86_64                                                                                                                                                                                         6/53 
  Installing : librabbitmq-0.8.0-2.el7.x86_64                                                                                                                                                                                           7/53 
  Updating   : 2:librgw2-14.2.4-0.el7.x86_64                                                                                                                                                                                            8/53 
  Installing : 2:python-ceph-argparse-14.2.4-0.el7.x86_64                                                                                                                                                                               9/53 
  Installing : yajl-2.0.4-4.el7.x86_64                                                                                                                                                                                                 10/53 
  Updating   : 2:python-cephfs-14.2.4-0.el7.x86_64                                                                                                                                                                                     11/53 
  Updating   : 2:python-rgw-14.2.4-0.el7.x86_64                                                                                                                                                                                        12/53 
  Updating   : 2:python-rbd-14.2.4-0.el7.x86_64                                                                                                                                                                                        13/53 
  Updating   : 2:libradosstriper1-14.2.4-0.el7.x86_64                                                                                                                                                                                  14/53 
  Updating   : 2:ceph-common-14.2.4-0.el7.x86_64                                                                                                                                                                                       15/53 
  Updating   : selinux-policy-3.13.1-252.el7.1.noarch                                                                                                                                                                                  16/53 
  Updating   : selinux-policy-targeted-3.13.1-252.el7.1.noarch                                                                                                                                                                         17/53 
  Updating   : 2:ceph-base-14.2.4-0.el7.x86_64                                                                                                                                                                                         18/53 
  Updating   : 2:ceph-selinux-14.2.4-0.el7.x86_64                                                                                                                                                                                      19/53 
  Updating   : 2:ceph-mds-14.2.4-0.el7.x86_64                                                                                                                                                                                          20/53 
  Updating   : 2:ceph-mon-14.2.4-0.el7.x86_64                                                                                                                                                                                          21/53 
  Installing : libconfig-1.4.9-5.el7.x86_64                                                                                                                                                                                            22/53 
  Installing : libstoragemgmt-1.7.3-3.el7.x86_64                                                                                                                                                                                       23/53 
  Installing : libstoragemgmt-python-clibs-1.7.3-3.el7.x86_64                                                                                                                                                                          24/53 
  Installing : libstoragemgmt-python-1.7.3-3.el7.noarch                                                                                                                                                                                25/53 
  Updating   : 2:ceph-osd-14.2.4-0.el7.x86_64                                                                                                                                                                                          26/53 
  Installing : python-ply-3.4-11.el7.noarch                                                                                                                                                                                            27/53 
  Installing : python-pycparser-2.14-1.el7.noarch                                                                                                                                                                                      28/53 
  Installing : python-cffi-1.6.0-5.el7.x86_64                                                                                                                                                                                          29/53 
  Installing : python2-six-1.9.0-0.el7.noarch                                                                                                                                                                                          30/53 
  Installing : python2-bcrypt-3.1.6-2.el7.x86_64                                                                                                                                                                                       31/53 
  Updating   : 2:ceph-mgr-14.2.4-0.el7.x86_64                                                                                                                                                                                          32/53 
  Installing : 2:ceph-14.2.4-0.el7.x86_64                                                                                                                                                                                              33/53 
  Updating   : 2:ceph-radosgw-14.2.4-0.el7.x86_64                                                                                                                                                                                      34/53 
  Cleanup    : 2:ceph-radosgw-12.2.12-0.el7.x86_64                                                                                                                                                                                     35/53 
  Cleanup    : 2:ceph-mon-12.2.12-0.el7.x86_64                                                                                                                                                                                         36/53 
  Cleanup    : 2:ceph-osd-12.2.12-0.el7.x86_64                                                                                                                                                                                         37/53 
  Cleanup    : 2:ceph-mds-12.2.12-0.el7.x86_64                                                                                                                                                                                         38/53 
  Cleanup    : 2:ceph-mgr-12.2.12-0.el7.x86_64                                                                                                                                                                                         39/53 
  Cleanup    : 2:ceph-selinux-12.2.12-0.el7.x86_64                                                                                                                                                                                     40/53 
  Cleanup    : 2:ceph-base-12.2.12-0.el7.x86_64                                                                                                                                                                                        41/53 
  Cleanup    : selinux-policy-targeted-3.13.1-229.el7.noarch                                                                                                                                                                           42/53 
  Cleanup    : 2:ceph-common-12.2.12-0.el7.x86_64                                                                                                                                                                                      43/53 
  Cleanup    : selinux-policy-3.13.1-229.el7.noarch                                                                                                                                                                                    44/53 
  Cleanup    : 2:python-rbd-12.2.12-0.el7.x86_64                                                                                                                                                                                       45/53 
  Cleanup    : 2:python-rgw-12.2.12-0.el7.x86_64                                                                                                                                                                                       46/53 
  Cleanup    : 2:librgw2-12.2.12-0.el7.x86_64                                                                                                                                                                                          47/53 
  Cleanup    : 2:python-rados-12.2.12-0.el7.x86_64                                                                                                                                                                                     48/53 
  Cleanup    : 2:librbd1-12.2.12-0.el7.x86_64                                                                                                                                                                                          49/53 
  Cleanup    : 2:libradosstriper1-12.2.12-0.el7.x86_64                                                                                                                                                                                 50/53 
  Cleanup    : 2:python-cephfs-12.2.12-0.el7.x86_64                                                                                                                                                                                    51/53 
  Cleanup    : 2:libcephfs2-12.2.12-0.el7.x86_64                                                                                                                                                                                       52/53 
  Cleanup    : 2:librados2-12.2.12-0.el7.x86_64                                                                                                                                                                                        53/53 
  Verifying  : 2:librados2-14.2.4-0.el7.x86_64                                                                                                                                                                                          1/53 
  Verifying  : liboath-2.6.2-1.el7.x86_64                                                                                                                                                                                               2/53 
  Verifying  : 2:python-rgw-14.2.4-0.el7.x86_64                                                                                                                                                                                         3/53 
  Verifying  : python2-six-1.9.0-0.el7.noarch                                                                                                                                                                                           4/53 
  Verifying  : 2:ceph-14.2.4-0.el7.x86_64                                                                                                                                                                                               5/53 
  Verifying  : 2:ceph-mgr-14.2.4-0.el7.x86_64                                                                                                                                                                                           6/53 
  Verifying  : python2-bcrypt-3.1.6-2.el7.x86_64                                                                                                                                                                                        7/53 
  Verifying  : 2:ceph-osd-14.2.4-0.el7.x86_64                                                                                                                                                                                           8/53 
  Verifying  : libstoragemgmt-python-clibs-1.7.3-3.el7.x86_64                                                                                                                                                                           9/53 
  Verifying  : 2:ceph-base-14.2.4-0.el7.x86_64                                                                                                                                                                                         10/53 
  Verifying  : 2:ceph-common-14.2.4-0.el7.x86_64                                                                                                                                                                                       11/53 
  Verifying  : 2:libradosstriper1-14.2.4-0.el7.x86_64                                                                                                                                                                                  12/53 
  Verifying  : 2:python-rados-14.2.4-0.el7.x86_64                                                                                                                                                                                      13/53 
  Verifying  : 2:librbd1-14.2.4-0.el7.x86_64                                                                                                                                                                                           14/53 
  Verifying  : librdmacm-22.1-3.el7.x86_64                                                                                                                                                                                             15/53 
  Verifying  : libstoragemgmt-python-1.7.3-3.el7.noarch                                                                                                                                                                                16/53 
  Verifying  : 2:ceph-mds-14.2.4-0.el7.x86_64                                                                                                                                                                                          17/53 
  Verifying  : python-ply-3.4-11.el7.noarch                                                                                                                                                                                            18/53 
  Verifying  : 2:libcephfs2-14.2.4-0.el7.x86_64                                                                                                                                                                                        19/53 
  Verifying  : libconfig-1.4.9-5.el7.x86_64                                                                                                                                                                                            20/53 
  Verifying  : 2:ceph-selinux-14.2.4-0.el7.x86_64                                                                                                                                                                                      21/53 
  Verifying  : 2:ceph-radosgw-14.2.4-0.el7.x86_64                                                                                                                                                                                      22/53 
  Verifying  : selinux-policy-targeted-3.13.1-252.el7.1.noarch                                                                                                                                                                         23/53 
  Verifying  : 2:ceph-mon-14.2.4-0.el7.x86_64                                                                                                                                                                                          24/53 
  Verifying  : yajl-2.0.4-4.el7.x86_64                                                                                                                                                                                                 25/53 
  Verifying  : 2:librgw2-14.2.4-0.el7.x86_64                                                                                                                                                                                           26/53 
  Verifying  : python-cffi-1.6.0-5.el7.x86_64                                                                                                                                                                                          27/53 
  Verifying  : python-pycparser-2.14-1.el7.noarch                                                                                                                                                                                      28/53 
  Verifying  : libstoragemgmt-1.7.3-3.el7.x86_64                                                                                                                                                                                       29/53 
  Verifying  : 2:python-rbd-14.2.4-0.el7.x86_64                                                                                                                                                                                        30/53 
  Verifying  : 2:python-cephfs-14.2.4-0.el7.x86_64                                                                                                                                                                                     31/53 
  Verifying  : selinux-policy-3.13.1-252.el7.1.noarch                                                                                                                                                                                  32/53 
  Verifying  : 2:python-ceph-argparse-14.2.4-0.el7.x86_64                                                                                                                                                                              33/53 
  Verifying  : librabbitmq-0.8.0-2.el7.x86_64                                                                                                                                                                                          34/53 
  Verifying  : selinux-policy-targeted-3.13.1-229.el7.noarch                                                                                                                                                                           35/53 
  Verifying  : 2:ceph-mgr-12.2.12-0.el7.x86_64                                                                                                                                                                                         36/53 
  Verifying  : 2:ceph-osd-12.2.12-0.el7.x86_64                                                                                                                                                                                         37/53 
  Verifying  : selinux-policy-3.13.1-229.el7.noarch                                                                                                                                                                                    38/53 
  Verifying  : 2:ceph-base-12.2.12-0.el7.x86_64                                                                                                                                                                                        39/53 
  Verifying  : 2:python-rados-12.2.12-0.el7.x86_64                                                                                                                                                                                     40/53 
  Verifying  : 2:python-cephfs-12.2.12-0.el7.x86_64                                                                                                                                                                                    41/53 
  Verifying  : 2:ceph-common-12.2.12-0.el7.x86_64                                                                                                                                                                                      42/53 
  Verifying  : 2:ceph-mon-12.2.12-0.el7.x86_64                                                                                                                                                                                         43/53 
  Verifying  : 2:libradosstriper1-12.2.12-0.el7.x86_64                                                                                                                                                                                 44/53 
  Verifying  : 2:libcephfs2-12.2.12-0.el7.x86_64                                                                                                                                                                                       45/53 
  Verifying  : 2:python-rbd-12.2.12-0.el7.x86_64                                                                                                                                                                                       46/53 
  Verifying  : 2:librbd1-12.2.12-0.el7.x86_64                                                                                                                                                                                          47/53 
  Verifying  : 2:ceph-radosgw-12.2.12-0.el7.x86_64                                                                                                                                                                                     48/53 
  Verifying  : 2:ceph-mds-12.2.12-0.el7.x86_64                                                                                                                                                                                         49/53 
  Verifying  : 2:librgw2-12.2.12-0.el7.x86_64                                                                                                                                                                                          50/53 
  Verifying  : 2:ceph-selinux-12.2.12-0.el7.x86_64                                                                                                                                                                                     51/53 
  Verifying  : 2:python-rgw-12.2.12-0.el7.x86_64                                                                                                                                                                                       52/53 
  Verifying  : 2:librados2-12.2.12-0.el7.x86_64                                                                                                                                                                                        53/53 

Installed:
  ceph.x86_64 2:14.2.4-0.el7                                                                                                                                                                                                                 

Dependency Installed:
  libconfig.x86_64 0:1.4.9-5.el7                   liboath.x86_64 0:2.6.2-1.el7                           librabbitmq.x86_64 0:0.8.0-2.el7                 librdmacm.x86_64 0:22.1-3.el7          libstoragemgmt.x86_64 0:1.7.3-3.el7      
  libstoragemgmt-python.noarch 0:1.7.3-3.el7       libstoragemgmt-python-clibs.x86_64 0:1.7.3-3.el7       python-ceph-argparse.x86_64 2:14.2.4-0.el7       python-cffi.x86_64 0:1.6.0-5.el7       python-ply.noarch 0:3.4-11.el7           
  python-pycparser.noarch 0:2.14-1.el7             python2-bcrypt.x86_64 0:3.1.6-2.el7                    python2-six.noarch 0:1.9.0-0.el7                 yajl.x86_64 0:2.0.4-4.el7             

Dependency Updated:
  ceph-base.x86_64 2:14.2.4-0.el7                   ceph-common.x86_64 2:14.2.4-0.el7   ceph-mds.x86_64 2:14.2.4-0.el7     ceph-mgr.x86_64 2:14.2.4-0.el7   ceph-mon.x86_64 2:14.2.4-0.el7         ceph-osd.x86_64 2:14.2.4-0.el7          
  ceph-radosgw.x86_64 2:14.2.4-0.el7                ceph-selinux.x86_64 2:14.2.4-0.el7  libcephfs2.x86_64 2:14.2.4-0.el7   librados2.x86_64 2:14.2.4-0.el7  libradosstriper1.x86_64 2:14.2.4-0.el7 librbd1.x86_64 2:14.2.4-0.el7           
  librgw2.x86_64 2:14.2.4-0.el7                     python-cephfs.x86_64 2:14.2.4-0.el7 python-rados.x86_64 2:14.2.4-0.el7 python-rbd.x86_64 2:14.2.4-0.el7 python-rgw.x86_64 2:14.2.4-0.el7       selinux-policy.noarch 0:3.13.1-252.el7.1
  selinux-policy-targeted.noarch 0:3.13.1-252.el7.1

Complete!
```

5、检查更新后的rpm packages

```
[root@ceph2 ~]# rpm -qa | grep ceph
python-ceph-argparse-14.2.4-0.el7.x86_64
ceph-mon-14.2.4-0.el7.x86_64
ceph-14.2.4-0.el7.x86_64
libcephfs2-14.2.4-0.el7.x86_64
ceph-base-14.2.4-0.el7.x86_64
ceph-mds-14.2.4-0.el7.x86_64
ceph-osd-14.2.4-0.el7.x86_64
ceph-mgr-14.2.4-0.el7.x86_64
ceph-radosgw-14.2.4-0.el7.x86_64
python-cephfs-14.2.4-0.el7.x86_64
ceph-common-14.2.4-0.el7.x86_64
ceph-selinux-14.2.4-0.el7.x86_64

[root@ceph2 ~]# rpm -qa | grep rbd
librbd1-14.2.4-0.el7.x86_64
python-rbd-14.2.4-0.el7.x86_64

[root@ceph2 ~]# rpm -qa | grep rados
librados2-14.2.4-0.el7.x86_64
libradosstriper1-14.2.4-0.el7.x86_64
ceph-radosgw-14.2.4-0.el7.x86_64
python-rados-14.2.4-0.el7.x86_64
```

6、重启服务每台ceph节点的ceph service并检查service状态

重启顺序

```
1、Ceph Monitors
2、Ceph Mgr
3、Ceph OSD Daemons
4、Ceph Metadata Servers
5、Ceph Object Gateways
```

重启命令

```
# 重启Ceph Monitors
[root@ceph2 ~]# systemctl restart ceph-mon@ceph2.service
[root@ceph2 ~]# ceph mon stat
e1: 3 mons at {ceph1=10.20.10.8:6789/0,ceph2=10.20.10.21:6789/0,ceph3=10.20.10.15:6789/0}, election epoch 6, leader 0 ceph1, quorum 0,1,2 ceph1,ceph3,ceph2

# 重启Ceph Mgr
[root@ceph2 ~]# systemctl restart ceph-mgr@ceph2.service
[root@ceph2 ~]# ceph -s
  cluster:
    id:     c4051efa-1997-43ef-8497-fb02bdf08233
    health: HEALTH_WARN
            insufficient standby MDS daemons available
            noout flag(s) set
 
  services:
    mon: 3 daemons, quorum ceph1,ceph3,ceph2
    mgr: ceph1(active), standbys: ceph3, ceph2
    mds: cephfs-1/1/1 up  {0=ceph2=up:active}
    osd: 6 osds: 6 up, 6 in
         flags noout
    rgw: 2 daemons active
 
  data:
    pools:   7 pools, 176 pgs
    objects: 245 objects, 5.30KiB
    usage:   6.05GiB used, 293GiB / 299GiB avail
    pgs:     176 active+clean

# 重启Ceph OSD Daemons
[root@ceph2 ~]# systemctl restart ceph-osd@2.service && systemctl restart ceph-osd@5.service && ceph osd stat
6 osds: 5 up, 6 in
[root@ceph2 ~]# ceph osd stat
6 osds: 6 up, 6 in

# 重启Ceph Metadata Servers
[root@ceph2 ~]# systemctl restart ceph-mds@ceph2.service
[root@ceph2 ~]# ceph mds stat
cephfs-1/1/1 up  {0=ceph2=up:reconnect}, 1 up:standby
[root@ceph2 ~]# ceph mds stat
cephfs-1/1/1 up  {0=ceph2=up:active}, 1 up:standby

# 重启Ceph Object Gateways
[root@ceph2 ~]# systemctl restart ceph-radosgw@rgw.ceph2.service
[root@ceph2 ~]# ceph -s
  cluster:
    id:     c4051efa-1997-43ef-8497-fb02bdf08233
    health: HEALTH_WARN
            insufficient standby MDS daemons available
            noout flag(s) set
 
  services:
    mon: 3 daemons, quorum ceph1,ceph3,ceph2
    mgr: ceph1(active), standbys: ceph3, ceph2
    mds: cephfs-1/1/1 up  {0=ceph2=up:active}
    osd: 6 osds: 6 up, 6 in
         flags noout
    rgw: 2 daemons active
 
  data:
    pools:   7 pools, 176 pgs
    objects: 245 objects, 5.30KiB
    usage:   6.05GiB used, 293GiB / 299GiB avail
    pgs:     176 active+clean
```

`注意`：当集群升级过程中，刚刚升级第一个MDS会出现`insufficient standby MDS daemons available`，当升级第二个MDS后，警告会自动消失。

7、警告解除`Legacy BlueStore stats reporting detected on 6 OSD(s)`

首先确认集群状态，无其他异常告警，并修复。

```
[root@ceph1 ~]# ceph -s
  cluster:
    id:     c4051efa-1997-43ef-8497-fb02bdf08233
    health: HEALTH_WARN
            noout flag(s) set
            Legacy BlueStore stats reporting detected on 6 OSD(s)
 
  services:
    mon: 3 daemons, quorum ceph1,ceph3,ceph2 (age 111s)
    mgr: ceph2(active, since 6h), standbys: ceph3, ceph1
    mds: cephfs:1 {0=ceph2=up:active} 2 up:standby
    osd: 6 osds: 6 up, 6 in
         flags noout
    rgw: 3 daemons active (ceph1, ceph2, ceph3)
 
  data:
    pools:   7 pools, 176 pgs
    objects: 245 objects, 5.8 KiB
    usage:   6.1 GiB used, 293 GiB / 299 GiB avail
    pgs:     176 active+clean

[root@ceph1 ~]# systemctl stop ceph-osd@1.service
[root@ceph1 ~]# ceph-bluestore-tool repair --path /var/lib/ceph/osd/ceph-1/
repair success
[root@ceph1 ~]# systemctl start ceph-osd@1.service
```

`当前出现问题：https://tracker.ceph.com/issues/42297，正在与社区沟通`

相关ceph users

```
http://lists.ceph.com/pipermail/ceph-users-ceph.com/2019-July/035889.html
http://lists.ceph.com/pipermail/ceph-users-ceph.com/2019-July/036010.html
http://lists.ceph.com/pipermail/ceph-users-ceph.com/2019-July/036002.html
```

8、告警解除`3 monitors have not enabled msgr2`（nautilus版本中mon需要打开v2，监听3300端口），关于msgr2参见http://lnsyyj.github.io/2019/10/14/Ceph-MESSENGER-V2/

```
[root@ceph1 ~]# ceph mon enable-msgr2
```

9、解除noout标志

```
[root@ceph2 ~]# ceph osd unset noout
```

10、确认ceph状态

```
[root@ceph2 ~]# ceph -s
  cluster:
    id:     c4051efa-1997-43ef-8497-fb02bdf08233
    health: HEALTH_OK
 
  services:
    mon: 3 daemons, quorum ceph1,ceph3,ceph2 (age 30m)
    mgr: ceph2(active, since 7h), standbys: ceph3, ceph1
    mds: cephfs:1 {0=ceph2=up:active} 2 up:standby
    osd: 6 osds: 6 up, 6 in
    rgw: 3 daemons active (ceph1, ceph2, ceph3)
 
  data:
    pools:   7 pools, 176 pgs
    objects: 245 objects, 5.8 KiB
    usage:   6.1 GiB used, 293 GiB / 299 GiB avail
    pgs:     176 active+clean
```

## 升级Ceph客户端package

### 升级可能带来的影响

```
（1）客户端升级可能会影响客户业务，导致业务中断一定时间。
（2）可能带来客户端程序不兼容ceph client package的情况。
```

1、升级package

```
[root@ceph1 ~]# yum install ceph-common librados2 librbd1 python-rbd python-rados -y
```

2、确认升级后的版本

```
ceph --version
```

