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

