---
title: setup a Ceph cluster using ceph-ansible
date: 2018-07-29 20:43:54
tags: ansible
---

原文地址：https://gist.github.com/elsonrodriguez/a26cdb7f04a9e94c480457e12458954a

# Quick and Dirty Ceph Cluster

这是一个关于如何使用ceph-ansible设置Ceph集群的简明指南。

本指南假设：

- Ansible安装在您的本地计算机上
- 8个Centos 7.2节点使用XFS进行配置和格式化
- 您对节点有ssh和sudo访问权限

### Ansible Setup

首先，克隆ceph-ansible repo。

```
git clone https://github.com/ceph/ceph-ansible.git
cd ceph-ansible
git checkout a41eddf6f0d840580bc842fa59751defef10fa69
```

接下来，在repo的基础上添加一个inventory文件。使用以下内容作为指南，将示例IP替换为您自己的计算机IP。

```
[mons]
10.0.0.1
10.0.0.2
10.0.0.3

[osds]
10.0.0.4
10.0.0.5
10.0.0.6
10.0.0.7

[clients]
10.0.0.8
```

我们需要至少3个MON（cluster state daemons）4个OSD（object storage daemons）和1个client（to mount the storage）。

接下来，我们需要复制samples。

```
cp site.yml.sample site.yml
cp group_vars/all.sample group_vars/all
cp group_vars/mons.sample group_vars/mons
cp group_vars/osds.sample group_vars/osds
```

现在，我们需要修改group_vars/all中的一些变量：

```
ceph_origin: 'upstream' 
ceph_stable: true

monitor_interface: eth0
public_network: 172.20.0.0/16

journal_size: 5120
```

在groups_vars/osd中：

```
osd_directory: true
osd_directories:
  - /var/lib/ceph/osd/mydir1
```

### Running ceph-ansible playbook

创建inventory文件并修改变量后：

```
ansible-playbook site.yml -vvvv -i inventory -u centos
```

完成后，您应该有一个正常运行的Ceph集群。

### Interacting with Ceph

