---
title: The Ceph Benchmarking Tool
date: 2018-10-16 22:22:17
tags: performance
---

# CBT - The Ceph Benchmarking Tool

## INTRODUCTION（介绍）

CBT是一个用python编写的测试工具，可以自动执行并测试Ceph集群性能相关的任务。CBT不安装Ceph软件包，这应该在使用CBT之前完成。CBT可以在测试运行开始时创建 OSDs，也可以在测试运行期间重新创建OSD，或者只针对现有集群运行。CBT使用collectl记录系统指标，它可以选择使用perf，blktrace和valgrind在内的多种工具收集更多信息。除基本基准测试外，CBT还可以进行高级测试，包括自动OSD中断，erasure coded pools和cache tier配置。主要基准模块解释如下。

### radosbench

RADOS基准测试使用ceph-common软件包附带的rados二进制文件。它包含一个基准测试工具，它通过librados来运行集群，librados是Ceph提供的底层object storage API。目前，RADOS基准模块为每个client创建一个pool。

### librbdfio

librbdfio基准模块是测试Ceph集群的块存储性能的最简单方法。最新版本的flexible IO tester (fio)提供了RBD引擎。这允许fio通过用户态librbd库测试RBD volumes的块存储性能，而无需配置KVM/QEMU。这些库与QEMU后端使用的库相同，因此它很近似于KVM/QEMU性能。

### kvmrbdfio

kvmrbdfio基准测试使用flexible IO tester (fio) 来运行已attache到KVM instance的RBD volume。它要求在使用CBT之前创建instance，并attache RBD volumes。此模块用于对通过Cinder创建的RBD并attache到OpenStack instance进行基准测试。或者，可以使用Vagrant或Virtual Machine Manager来提供instance。

### rbdfio

rbdfio基准测试使用flexible IO tester (fio) 来执行使用KRBD内核驱动映射到块设备的RBD volume。此模块最适用于模拟需要直接使用块设备的应用程序，而不是在虚拟机内运行。

## PREREQUISITES（必备条件）

CBT使用多个库和工具来运行：

1. python-yaml - 用于读取配置文件的python的YAML库。
2. python-lxml - 功能强大的Pythonic XML处理库，将libxml2/libxslt与ElementTree API相结合
3. ssh (and scp) - 远程安全命令执行和数据传输
4. pdsh (and pdcp) - 并行ssh和scp实现
5. ceph - 可扩展的分布式存储系统

请注意，目前没有为基于RHEL7和CentOS 7发行版的pdsh。这些RPM包可在此处获得：

- ftp://rpmfind.net/linux/fedora/linux/releases/23/Everything/x86_64/os/Packages/p/pdsh-2.31-4.fc23.x86_64.rpm
- ftp://rpmfind.net/linux/fedora/linux/releases/23/Everything/x86_64/os/Packages/p/pdsh-rcmd-rsh-2.31-4.fc23.x86_64.rpm
- ftp://rpmfind.net/linux/fedora/linux/releases/23/Everything/x86_64/os/Packages/p/pdsh-rcmd-ssh-2.31-4.fc23.x86_64.rpm

如果需要，可以使用可选工具和基准：

1. collectl - 系统数据收集
2. blktrace - 块设备IO跟踪
3. seekwatcher - 通过blktrace数据创建图表
4. perf - 系统和进程分析
5. valgrind - 特定进程的runtime memory和cpu分析
6. fio - 基准suite，集成了posix，libaio和librbd的支持
7. cosbench - Intel的对象存储基准

## USER AND NODE SETUP（用户和节点设置）

除上述软件外，还必须有许多节点才能运行测试。这些节点可以划分为多个类别。多个类别可以包含相同的host，如果它承担多个角色（例如，运行OSD和mon）。

1. head - 运行常规ceph命令的节点
2. clients - 运行基准测试或其他客户端工具的节点
3. osds - OSD存在的节点
4. rgws - rgw servers节点
5. mons - mons节点

还可以指定用户运行所有远程命令。用于运行cbt的主机必须能够以指定用户身份发出无密码ssh命令。这可以通过创建无密码ssh密钥来完成：

```
ssh-keygen -t dsa
```

并将~/.ssh中生成的公钥复制到所有远程主机上的~/.ssh/authorized_key文件中。

此用户还必须能够使用sudo运行某些命令。实现此目的的最简单方法是为该用户简单地启用全局无密码sudo访问，但这仅适用于实验室环境。这可以通过运行visudo并添加如下内容来实现：

```
# passwordless sudo for cbt
<user>    ALL=(ALL)       NOPASSWD:ALL
```

其中`<user>`是将具有密码sudo访问权限的用户。

有关具体细节，请参阅操作系统文档。

除了上述内容之外，还需要将所有osds和mons添加到ssh的已知主机列表中（/etc/hosts文件）才能正常执行。 否则，基准测试将无法运行。

请注意，如果sudoers文件需要tty，则pdsh命令可能会有困难。如果是这种情况，推荐在visudo中添加`Defaults requiretty`行。

## DISK PARTITIONING（磁盘分区）

目前，CBT为Ceph OSD data和journal分区在/dev/disk/by-partlabel中寻找指定分区labels。在未来的某个时刻，这将变得更加灵活，因为现在这是预期的行为。每个OSD主机分区应该用下面的gpt labels指定：

```
osd-device-<num>-data
osd-device-<num>-journal
```

其`<num>`是从0开始并以系统上最后一个设备结束的device序列。目前，cbt假设系统中的所有节点具有相同数量的device。有一个脚本可以显示我们在测试环境中如何创建分区labels的示例：

```
https://github.com/ceph/cbt/blob/master/tools/mkpartmagna.sh
```

## CREATING A YAML FILE（创建一个YAML文件）

CBT yaml文件具有一个基本结构，您可以在其中定义一个cluster和一组针对它运行的基准测试。例如, 下面的 yaml 文件在具有主机名  "burnupiX " 的节点上创建单节点群集。使用256个PG为1x replication pool定义pool的配置文件，并且该pool用于使用带有librbd引擎的fio运行RBD性能测试。

```
cluster:
  user: 'nhm'
  head: "burnupiX"
  clients: ["burnupiX"]
  osds: ["burnupiX"]
  mons:
    burnupiX:
      a: "127.0.0.1:6789"
  osds_per_node: 1
  fs: 'xfs'
  mkfs_opts: '-f -i size=2048'
  mount_opts: '-o inode64,noatime,logbsize=256k'
  conf_file: '/home/nhm/src/ceph-tools/cbt/newstore/ceph.conf.1osd'
  iterations: 1
  use_existing: False
  clusterid: "ceph"
  tmp_dir: "/tmp/cbt"
  pool_profiles:
    rbd:
      pg_size: 256
      pgp_size: 256
      replication: 1
benchmarks:
  librbdfio:
    time: 300
    vol_size: 16384
    mode: [read, write, randread, randwrite]
    op_size: [4194304, 2097152, 1048576]
    concurrent_procs: [1]
    iodepth: [64]
    osd_ra: [4096]
    cmd_path: '/home/nhm/src/fio/fio'
    pool_profile: 'rbd'
```

还定义了一个关联的ceph.conf.1osd文件，其中包含要在此测试中使用的各种设置：

```
[global]
        osd pool default size = 1
        auth cluster required = none
        auth service required = none
        auth client required = none
        keyring = /tmp/cbt/ceph/keyring
        osd pg bits = 8  
        osd pgp bits = 8
        log to syslog = false
        log file = /tmp/cbt/ceph/log/$name.log
        public network = 192.168.10.0/24
        cluster network = 192.168.10.0/24
        rbd cache = true
        osd scrub load threshold = 0.01
        osd scrub min interval = 137438953472
        osd scrub max interval = 137438953472
        osd deep scrub interval = 137438953472
        osd max scrubs = 16
        filestore merge threshold = 40
        filestore split multiple = 8
        osd op threads = 8
        mon pg warn max object skew = 100000
        mon pg warn min per osd = 0
        mon pg warn max per osd = 32768

[mon]
        mon data = /tmp/cbt/ceph/mon.$id
        
[mon.a]
        host = burnupiX 
        mon addr = 127.0.0.1:6789

[osd.0]
        host = burnupiX
        osd data = /tmp/cbt/mnt/osd-device-0-data
        osd journal = /dev/disk/by-partlabel/osd-device-0-journal
```

要运行此基准测试suite，cbt启动时带参数来指定用于存储结果archive目录和要使用的配置文件：

```
cbt.py --archive=<archive dir> ./mytests.yaml
```

您还可以通过在命令行上指定ceph.conf文件来指定要使用的文件：

```
cbt.py -archive=<archive dir> --conf=./ceph.conf.1osd ./mytests.yaml
```

通过这种方式，可以混合或匹配ceph.conf文件和yaml测试文件，以创建测试的参数化扫描。tools目录中名为mkcephconf.py的脚本可以自动生成数百或数千个ceph.conf文件（需要自定义选项范围），然后以这种方式与cbt一起使用。

## CONCLUSION（结论）

还有许多其他功能强大的方法可以使用，本文档中尚未涵盖所有cbt的功能。随着时间的推移，我们将尝试为这些功能提供更好的示例和文档。现在，最好查看示例，查看代码，并提出问题！

## 实验

该实验没有使用nhm账户，以下实验都是root用户，所以ssh互信需要把nhm改为root

```
cbt节点
1、安装pdsh
[root@cbt ~]# yum -y install epel-release
[root@cbt ~]# yum install -y pdsh
与ceph节点做免密
[root@cbt ~]# ssh-keygen
[root@cbt cbt]# ssh-copy-id nhm@192.168.0.45
测试
[root@cbt ~]# pdsh -w 192.168.0.45 -l nhm uptime
192.168.0.45:  14:28:43 up 32 min,  1 user,  load average: 0.00, 0.01, 0.05
2、安装piip
[root@cbt ~]# curl "https://bootstrap.pypa.io/get-pip.py" -o "get-pip.py"
[root@cbt ~]# python get-pip.py
[root@cbt ~]# python -m pip install -U pip
[root@cbt ~]# pip install --upgrade setuptools
3、安装python依赖库
[root@cbt cbt]# pip install lxml numpy PyYAML
4、配置/etc/hosts文件
[root@cbt cbt]# cat /etc/hosts
192.168.0.45    cbt-ceph-1
192.168.0.46    cbt-ceph-2
192.168.0.47    cbt-ceph-3


ceph节点
[root@cbt-ceph-1 ~]# yum -y install epel-release
[root@cbt-ceph-1 ~]# yum -y install collectl perf blktrace
[root@cbt-ceph-1 ~]# useradd -m nhm -g root -G root
[root@cbt-ceph-1 ~]# echo nhm:nhm | chpasswd
[root@cbt-ceph-1 ~]# vi /etc/sudoers
添加
nhm   ALL=(ALL)         NOPASSWD: ALL

[root@cbt-ceph-1 ~]# yum -y install epel-release
[root@cbt-ceph-1 ~]# yum install -y pdsh
[root@cbt-ceph-1 ~]# yum install -y fio



cbt单节点radosbench配置文件
[root@cbt ~]# cat /root/mytests.yaml
cluster:
  user: 'root'
  head: "cbt-ceph-1"
  clients: ["cbt-ceph-1"]
  osds: ["cbt-ceph-1"]
  mons: ["cbt-ceph-1"]
  osds_per_node: 1
  fs: 'xfs'
  mkfs_opts: '-f -i size=2048'
  mount_opts: '-o inode64,noatime,logbsize=256k'
  conf_file: '/etc/ceph/ceph.conf'
  iterations: 1
  use_existing: True
  version_compat: jewel
  clusterid: "ceph"
  tmp_dir: "/tmp/cbt"
  pool_profiles:
    rbd:
      pg_size: 256
      pgp_size: 256
      replication: 1
benchmarks:
  radosbench:
    op_size: [ 4194304, 524288, 4096 ]
    write_only: False
    time: 5
    concurrent_ops: [ 128 ]
    concurrent_procs: 1
    use_existing: True
    pool_profile: replicated


cbt单节点librbdfio配置文件
[root@cbt cbt]# cat /root/mytests.yaml
cluster:
  user: 'root'
  head: "cbt-ceph-1"
  clients: ["cbt-ceph-1"]
  osds: ["cbt-ceph-1"]
  mons: ["cbt-ceph-1"]
  osds_per_node: 1
  fs: 'xfs'
  mkfs_opts: '-f -i size=2048'
  mount_opts: '-o inode64,noatime,logbsize=256k'
  conf_file: '/etc/ceph/ceph.conf'
  iterations: 1
  use_existing: True
  version_compat: jewel
  clusterid: "ceph"
  tmp_dir: "/tmp/cbt"
  pool_profiles:
    rbd:
      pg_size: 256
      pgp_size: 256
      replication: 1
benchmarks:
  librbdfio:
    time: 300
    vol_size: 16384
    mode: [read, write, randread, randwrite]
    op_size: [4194304, 2097152, 1048576]
    concurrent_procs: [1]
    iodepth: [64]
    osd_ra: [4096]
    pool_profile: 'rbd'
    
运行cbt
[root@cbt cbt]# python cbt.py --archive=yujiangresult   /root/mytests.yaml
```

