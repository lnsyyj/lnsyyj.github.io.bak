---
title: ceph-ansible project
date: 2018-08-29 23:18:24
tags: ceph
---

近期工作是调研teuthology，目标是把teuthology enable起来做ceph的测试使用，但没想到teuthology只是冰山一角，其关联了ceph社区的整套CI/CD，这套CI/CD关联项目多的力不从心，其关联的项目大致如下：

```
https://github.com/ceph/shaman.git
https://github.com/ceph/teuthology.git
https://github.com/ceph/ceph-cm-ansible.git
https://github.com/ceph/pulpito.git
https://github.com/ceph/ceph-qa-suite.git
https://github.com/ceph/paddles.git
https://github.com/ceph/ceph.git
还有很多未知的，shaman社区明确说是内部使用的，不提供文档，所以只能撸代码，从代码中找数据库的表结构，到目前还没时间找出来。https://github.com/ceph/shaman/issues/110
```

既然社区使用ansible，并且Sébastien Han一直维护，毕竟有他的道理，ansible在自动化运维领域也是很常用的（尤其在大规模部署的情况下），倒不如借着这个机会学习一下。

ansible基础语法看以下两个文章差不多够用，再有问题可以查ansible官网。

```
https://www.w3cschool.cn/automate_with_ansible/
https://www.cnblogs.com/f-ck-need-u/p/7567417.html
```

接着来看一下ceph-ansible项目，README中给出了文档，一个是[master](http://docs.ceph.com/ceph-ansible/master/)一个是[stable-3.0](http://docs.ceph.com/ceph-ansible/stable-3.0/)。我没有那么傻，当然是看stable-3.0了，哈哈。

```
https://github.com/ceph/ceph-ansible
```

首先是把ceph-ansible的playbooks clone下来。

```
[root@k8s-master ~]# yum install git -y
[root@k8s-master ~]# git clone https://github.com/ceph/ceph-ansible.git
[root@k8s-master ~]# cd ceph-ansible/
# 当然branch也要对应文档，这样后续困难应该少一点
[root@k8s-master ceph-ansible]# git checkout -b mystable-3.0 origin/stable-3.0
```

接着需要安装ansible，官方stable-3.0文档中说明了ceph-ansible项目各个branch，支持ansible版本和ceph版本。我们只看现在的branch，其他branch有需要再说。

```
stable-3.0支持ceph版本jewel和luminous。该branch支持ansible版本2.3.1,2.3.2和2.4.2。
```

