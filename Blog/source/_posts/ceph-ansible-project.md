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

# 安装ansible 2.4.2

使用yum安装指定版本的ansible

```
[root@cephJ ~]# yum -h | grep show
  --showduplicates      在 list/search 命令下，显示源里重复的条目

[root@cephJ ~]# yum --showduplicates list ansible
已加载插件：fastestmirror, priorities
Loading mirror speeds from cached hostfile
 * base: mirrors.cn99.com
 * epel: www.ftp.ne.jp
 * extras: mirrors.nwsuaf.edu.cn
 * nux-dextop: li.nux.ro
 * updates: mirrors.nwsuaf.edu.cn
12 packages excluded due to repository priority protections
可安装的软件包
ansible.noarch                                                                          2.4.2.0-2.el7                                                                          extras
ansible.noarch                                                                          2.7.5-1.el7                                                                            epel  

### sudo yum install <package name>-<version info>
[root@cephJ ~]# sudo yum install -y ansible-2.4.2.0

[root@cephJ ~]# ansible --version
ansible 2.4.2.0
  config file = /etc/ansible/ansible.cfg
  configured module search path = [u'/root/.ansible/plugins/modules', u'/usr/share/ansible/plugins/modules']
  ansible python module location = /usr/lib/python2.7/site-packages/ansible
  executable location = /usr/bin/ansible
  python version = 2.7.5 (default, Apr 11 2018, 07:36:10) [GCC 4.8.5 20150623 (Red Hat 4.8.5-28)]
```

# 配置ceph-ansible

准备四台机器

```
ansible节点
	ansible-master

ceph节点
	ansible-ceph-1
	ansible-ceph-2
	ansible-ceph-3
```

配置ansible

```
ansible节点执行

1、首先clone代码
[root@ansible-master ~]# git clone https://github.com/ceph/ceph-ansible.git && cd ceph-ansible/

2、创建本地分支并切换分支
[root@ansible-master ceph-ansible]# git checkout -b myv3.2.0 v3.2.0 

3、copy模板文件
[root@ansible-master ceph-ansible]# cp site.yml.sample site.yml && cd group_vars/ && cp all.yml.sample all.yml && cp osds.yml.sample osds.yml

4、修改ansible机器清单(inventory)
[root@ansible-master ~]# vim /etc/ansible/hosts
[mons]
ansible-ceph-[1:3]	ansible_ssh_pass=yujiang2
[osds]
ansible-ceph-[1:3]	ansible_ssh_pass=yujiang2
[rgws]
ansible-ceph-[1:3]	ansible_ssh_pass=yujiang2

5、批量推送sshkey
[root@ansible-master ~]# ssh-keygen -t rsa
[root@ansible-master ~]# cat push-ssh.yaml 
- hosts: all
  user: root
  tasks:
    - name: ssh-key-copy
      authorized_key: user=root key="{{ lookup('file','/root/.ssh/id_rsa.pub')}}"
      tags:
        - sshkey
[root@ansible-master ~]# ansible-playbook push-ssh.yaml

PLAY [all] *******************************************************************************************************************************************************************************************************************************************************************

TASK [Gathering Facts] *******************************************************************************************************************************************************************************************************************************************************
ok: [ansible-ceph-1]
ok: [ansible-ceph-2]
ok: [ansible-ceph-3]

TASK [ssh-key-copy] **********************************************************************************************************************************************************************************************************************************************************
changed: [ansible-ceph-3]
changed: [ansible-ceph-1]
changed: [ansible-ceph-2]

PLAY RECAP *******************************************************************************************************************************************************************************************************************************************************************
ansible-ceph-1             : ok=2    changed=1    unreachable=0    failed=0   
ansible-ceph-2             : ok=2    changed=1    unreachable=0    failed=0   
ansible-ceph-3             : ok=2    changed=1    unreachable=0    failed=0 

6、安装pip并安装ceph-ansible依赖
[root@ansible-master ~]# curl "https://bootstrap.pypa.io/get-pip.py" -o "get-pip.py" && python get-pip.py && pip install --upgrade setuptools
[root@ansible-master ceph-ansible]# pip install -r requirements.txt


```

