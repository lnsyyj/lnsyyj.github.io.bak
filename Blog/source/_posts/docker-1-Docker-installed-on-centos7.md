---
title: docker-1 Docker installed on centos7
date: 2018-12-12 23:01:20
tags: docker
---

学习视频地址：https://coding.imooc.com/class/189.html

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

2、下载Vagrant box

```
yujiangdeMBP-13:centos7 yujiang$ ls ~/.vagrant.d/downloadboxes/centos7/
virtualbox.box
```

3、添加Vagrant box到镜像列表

```
yujiangdeMBP-13:centos7 yujiang$ vagrant box add centos/centos7 ~/.vagrant.d/downloadboxes/centos7/virtualbox.box 
==> box: Box file was not detected as metadata. Adding it directly...
==> box: Adding box 'centos/centos7' (v0) for provider: 
    box: Unpacking necessary files from: file:///Users/yujiang/.vagrant.d/downloadboxes/centos7/virtualbox.box
==> box: Successfully added box 'centos/centos7' (v0) for 'virtualbox'!

yujiangdeMBP-13:centos7 yujiang$ vagrant box list
centos/centos7 (virtualbox, 0)
```

4、创建Vagrant虚拟机目录并启动虚拟机

```
yujiangdeMBP-13:~ yujiang$ mkdir Vagrant && cd Vagrant/
yujiangdeMBP-13:Vagrant yujiang$ vagrant init centos/centos7
yujiangdeMBP-13:Vagrant yujiang$ vagrant up
yujiangdeMBP-13:Vagrant yujiang$ vagrant ssh
[vagrant@localhost ~]$ exit
yujiangdeMBP-13:Vagrant yujiang$ vagrant destroy
```

5、之后使用《在centos7上安装docker》安装docker

## 编辑Vagrantfile，启动虚拟机时自动安装docker

```
# -*- mode: ruby -*-
Vagrant.configure("2") do |config|
  config.vm.box = "centos/centos7"
  config.vm.provision "shell", inline: <<-SHELL
  sudo yum remove docker docker-client docker-client-latest docker-common docker-latest docker-latest-logrotate docker-logrotate docker-selinux docker-engine-selinux docker-engine
  sudo yum install -y yum-utils device-mapper-persistent-data lvm2
  sudo yum-config-manager --add-repo https://download.docker.com/linux/centos/docker-ce.repo
  sudo yum install docker-ce -y
  sudo systemctl start docker
  sudo systemctl enable docker
  SHELL
end

yujiangdeMBP-13:Vagrant yujiang$ vagrant up
yujiangdeMBP-13:Vagrant yujiang$ vagrant ssh
[vagrant@localhost ~]$ sudo docker version
Client:
 Version:           18.09.0
 API version:       1.39
 Go version:        go1.10.4
 Git commit:        4d60db4
 Built:             Wed Nov  7 00:48:22 2018
 OS/Arch:           linux/amd64
 Experimental:      false

Server: Docker Engine - Community
 Engine:
  Version:          18.09.0
  API version:      1.39 (minimum version 1.12)
  Go version:       go1.10.4
  Git commit:       4d60db4
  Built:            Wed Nov  7 00:19:08 2018
  OS/Arch:          linux/amd64
  Experimental:     false
```

## 使用docker命令时，去掉sudo

1、添加docker group

```
[vagrant@localhost ~]$ sudo groupadd docker
groupadd: group 'docker' already exists
```

2、添加当前用户到docker group

```
[vagrant@localhost ~]$ sudo gpasswd -a vagrant docker
Adding user vagrant to group docker
```

3、退出当前终端，重新连接

```
[vagrant@localhost ~]$ exit

yujiangdeMBP-13:Vagrant yujiang$ vagrant ssh

[vagrant@localhost ~]$ docker version
Client:
 Version:           18.09.0
 API version:       1.39
 Go version:        go1.10.4
 Git commit:        4d60db4
 Built:             Wed Nov  7 00:48:22 2018
 OS/Arch:           linux/amd64
 Experimental:      false

Server: Docker Engine - Community
 Engine:
  Version:          18.09.0
  API version:      1.39 (minimum version 1.12)
  Go version:       go1.10.4
  Git commit:       4d60db4
  Built:            Wed Nov  7 00:19:08 2018
  OS/Arch:          linux/amd64
  Experimental:     false
```

