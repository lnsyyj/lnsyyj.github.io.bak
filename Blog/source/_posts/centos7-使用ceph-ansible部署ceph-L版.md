---
title: centos7 使用ceph-ansible部署ceph L版
date: 2019-01-06 19:24:39
tags: ceph-ansible
---

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
[root@ansible-master ceph-ansible]# cp site.yml.sample site.yml

4、修改ansible机器清单(inventory)
[root@ansible-master ~]# vim /etc/ansible/hosts
[mons]
ansible-ceph-[1:3]	ansible_ssh_pass=yujiang2
[osds]
ansible-ceph-[1:3]	ansible_ssh_pass=yujiang2
[rgws]
ansible-ceph-[1:3]	ansible_ssh_pass=yujiang2
[mgrs]
ansible-ceph-[1:3]  ansible_ssh_pass=yujiang2

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
ansible-ceph-1             : ok=2    changed=1    unreachable=0    failed=0   
ansible-ceph-2             : ok=2    changed=1    unreachable=0    failed=0   
ansible-ceph-3             : ok=2    changed=1    unreachable=0    failed=0 

6、安装pip并安装ceph-ansible依赖
[root@ansible-master ~]# curl "https://bootstrap.pypa.io/get-pip.py" -o "get-pip.py" && python get-pip.py && pip install --upgrade setuptools
[root@ansible-master ceph-ansible]# pip install -r requirements.txt

7、修改ansible变量
[root@ansible-master group_vars]# pwd
/root/ceph-ansible/group_vars

[root@ansible-master group_vars]# cp all.yml.sample all.yml && cp osds.yml.sample osds.yml

all.yml修改如下：
mon_group_name: mons
osd_group_name: osds
ntp_daemon_type: chronyd
ceph_origin: repository
ceph_repository: community
ceph_mirror: http://mirrors.163.com/ceph
ceph_stable_key: http://mirrors.163.com/ceph/keys/release.asc
ceph_stable_release: luminous
ceph_stable_repo: "{{ ceph_mirror }}/rpm-{{ ceph_stable_release }}/el7/x86_64"
monitor_interface: eth0
public_network: 192.168.0.0/24
cluster_network: 192.168.0.0/24
radosgw_interface: eth0
radosgw_address: 0.0.0.0

osds.yml修改如下：
copy_admin_key: true
devices:
 - /dev/vdb
osd_scenario: collocated

8、执行ansible开始部署ceph，部署哪些模块会在/etc/ansible/hosts中定义（下面是我们之前定义的），如果在该文件中没有对应模块定义，ansible会忽略该模块的部署。
[mons]
ansible-ceph-[1:3]	ansible_ssh_pass=yujiang2
[osds]
ansible-ceph-[1:3]	ansible_ssh_pass=yujiang2
[rgws]
ansible-ceph-[1:3]	ansible_ssh_pass=yujiang2
[mgrs]
ansible-ceph-[1:3]  ansible_ssh_pass=yujiang2

[root@ansible-master ceph-ansible]# pwd
/root/ceph-ansible
[root@ansible-master ceph-ansible]# ansible-playbook site.yml
PLAY RECAP *******************************************************************************************************************************************************************************************************************************************************************
ansible-ceph-1             : ok=310  changed=17   unreachable=0    failed=0   
ansible-ceph-2             : ok=287  changed=17   unreachable=0    failed=0   
ansible-ceph-3             : ok=289  changed=20   unreachable=0    failed=0   

INSTALLER STATUS *************************************************************************************************************************************************************************************************************************************************************
Install Ceph Monitor        : Complete (0:00:37)
Install Ceph Manager        : Complete (0:01:05)
Install Ceph OSD            : Complete (0:00:38)
Install Ceph RGW            : Complete (0:00:30)
```