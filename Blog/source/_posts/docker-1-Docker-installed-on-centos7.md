---
title: docker-1 Docker installed on centos7
date: 2018-12-12 23:01:20
tags: docker
---

# 在centos7上安装docker

```
https://docs.docker.com/install/linux/docker-ce/centos/
```

新建docker用户，并安装依赖

```
[root@docker ~]# adduser docker
[root@docker ~]# hostnamectl set-hostname docker
[root@docker ~]# vi /etc/sudoers
添加
docker  ALL=(ALL)       ALL
[root@docker ~]# su - docker
上一次登录：三 12月 12 10:26:46 EST 2018pts/0 上
[docker@docker ~]$ 
[docker@docker ~]$ sudo yum install epel-release vim -y
```

安装社区版

```
卸载已安装的docker
sudo yum remove docker \
                  docker-client \
                  docker-client-latest \
                  docker-common \
                  docker-latest \
                  docker-latest-logrotate \
                  docker-logrotate \
                  docker-selinux \
                  docker-engine-selinux \
                  docker-engine

安装required packages
sudo yum install -y yum-utils \
  device-mapper-persistent-data \
  lvm2

安装docker源
sudo yum-config-manager \
    --add-repo \
    https://download.docker.com/linux/centos/docker-ce.repo

安装docker
[docker@docker ~]$ sudo yum install docker-ce -y

启动docker进程，并设置开机启动
[docker@docker ~]$ sudo systemctl start docker
[docker@docker ~]$ sudo systemctl enable docker
Created symlink from /etc/systemd/system/multi-user.target.wants/docker.service to /usr/lib/systemd/system/docker.service.

测试docker安装
[docker@docker ~]$ sudo docker run hello-world
```

# Vagrant建立虚拟机，并在虚拟机中安装docker

1、安装VirtualBox然后安装Vagrant

2、创建Vagrant虚拟机目录

```
yujiangdeMBP-13:~ yujiang$ mkdir Vagrant
```



创建Vagrantfile

```

```

