---
title: docker regsitry搭建
date: 2018-08-18 22:49:14
tags: docker
---

参考文章：https://www.jianshu.com/p/fc36368b5c44

mac virtualbox 制作两台虚拟机进行测试

```
机器名：k8s-master
[root@k8s-master ~]# cat /etc/hostname 
k8s-master.localdomain

[root@k8s-master ~]# systemctl stop firewalld.service
[root@k8s-master ~]# systemctl disable firewalld.service

[root@k8s-master ~]# cat /etc/sysconfig/selinux 
# This file controls the state of SELinux on the system.
# SELINUX= can take one of these three values:
#     enforcing - SELinux security policy is enforced.
#     permissive - SELinux prints warnings instead of enforcing.
#     disabled - No SELinux policy is loaded.
SELINUX=disabled
# SELINUXTYPE= can take one of three two values:
#     targeted - Targeted processes are protected,
#     minimum - Modification of targeted policy. Only selected processes are protected. 
#     mls - Multi Level Security protection.
SELINUXTYPE=targeted

[root@k8s-master ~]# cat /etc/sysconfig/network-scripts/ifcfg-enp0s8 
TYPE=Ethernet
PROXY_METHOD=none
BROWSER_ONLY=no
BOOTPROTO=static
DEFROUTE=yes
IPV4_FAILURE_FATAL=no
IPV6INIT=yes
IPV6_AUTOCONF=yes
IPV6_DEFROUTE=yes
IPV6_FAILURE_FATAL=no
IPV6_ADDR_GEN_MODE=stable-privacy
NAME=enp0s8
DEVICE=enp0s8
ONBOOT=yes
IPADDR=192.168.56.200 #静态IP
GATEWAY=192.168.56.1 #默认网关
NETMASK=255.255.255.0 #子网掩码
DNS1=192.168.56.1 #DNS 配置

机器名：k8s-slave
[root@k8s-slave ~]# cat /etc/hostname 
k8s-slave.localdomain

[root@k8s-slave ~]# systemctl stop firewalld.service
[root@k8s-slave ~]# systemctl disable firewalld.service

[root@k8s-slave ~]# cat /etc/sysconfig/selinux
# This file controls the state of SELinux on the system.
# SELINUX= can take one of these three values:
#     enforcing - SELinux security policy is enforced.
#     permissive - SELinux prints warnings instead of enforcing.
#     disabled - No SELinux policy is loaded.
SELINUX=disabled
# SELINUXTYPE= can take one of three two values:
#     targeted - Targeted processes are protected,
#     minimum - Modification of targeted policy. Only selected processes are protected. 
#     mls - Multi Level Security protection.
SELINUXTYPE=targeted

[root@k8s-slave ~]# cat /etc/sysconfig/network-scripts/ifcfg-enp0s8 
TYPE=Ethernet
PROXY_METHOD=none
BROWSER_ONLY=no
BOOTPROTO=static
DEFROUTE=yes
IPV4_FAILURE_FATAL=no
IPV6INIT=yes
IPV6_AUTOCONF=yes
IPV6_DEFROUTE=yes
IPV6_FAILURE_FATAL=no
IPV6_ADDR_GEN_MODE=stable-privacy
NAME=enp0s8
DEVICE=enp0s8
ONBOOT=yes
IPADDR=192.168.56.201 #静态IP
GATEWAY=192.168.56.1 #默认网关
NETMASK=255.255.255.0 #子网掩码
DNS1=192.168.56.1 #DNS 配置
```

两台虚拟机安装docker并启动服务

```
[root@k8s-master ~]# yum -y install docker
[root@k8s-master ~]# systemctl start docker.service
[root@k8s-master ~]# systemctl status docker.service
● docker.service - Docker Application Container Engine
   Loaded: loaded (/usr/lib/systemd/system/docker.service; disabled; vendor preset: disabled)
   Active: active (running) since 六 2018-08-18 11:10:58 EDT; 6s ago
     Docs: http://docs.docker.com
 Main PID: 1522 (dockerd-current)
   CGroup: /system.slice/docker.service
           ├─1522 /usr/bin/dockerd-current --add-runtime docker-runc=/usr/libexec/docker/docker-runc-current --default-runtime=docker-runc --exec-opt native.cgroupdriver=systemd ...
           └─1526 /usr/bin/docker-containerd-current -l unix:///var/run/docker/libcontainerd/docker-containerd.sock --metrics-interval=0 --start-timeout 2m --state-dir /var/run/d...

8月 18 11:10:58 k8s-master.localdomain dockerd-current[1522]: time="2018-08-18T11:10:58.149875557-04:00" level=warning msg="Docker could not enable SELinux on the host system"
8月 18 11:10:58 k8s-master.localdomain dockerd-current[1522]: time="2018-08-18T11:10:58.178512067-04:00" level=info msg="Graph migration to content-addressability took ... seconds"
8月 18 11:10:58 k8s-master.localdomain dockerd-current[1522]: time="2018-08-18T11:10:58.179123980-04:00" level=info msg="Loading containers: start."
8月 18 11:10:58 k8s-master.localdomain dockerd-current[1522]: time="2018-08-18T11:10:58.229107163-04:00" level=info msg="Firewalld running: false"
8月 18 11:10:58 k8s-master.localdomain dockerd-current[1522]: time="2018-08-18T11:10:58.312308773-04:00" level=info msg="Default bridge (docker0) is assigned with an IP... address"
8月 18 11:10:58 k8s-master.localdomain dockerd-current[1522]: time="2018-08-18T11:10:58.386055892-04:00" level=info msg="Loading containers: done."
8月 18 11:10:58 k8s-master.localdomain dockerd-current[1522]: time="2018-08-18T11:10:58.403922614-04:00" level=info msg="Daemon has completed initialization"
8月 18 11:10:58 k8s-master.localdomain dockerd-current[1522]: time="2018-08-18T11:10:58.403960859-04:00" level=info msg="Docker daemon" commit="dded712/1.13.1" graphdri...on=1.13.1
8月 18 11:10:58 k8s-master.localdomain systemd[1]: Started Docker Application Container Engine.
8月 18 11:10:58 k8s-master.localdomain dockerd-current[1522]: time="2018-08-18T11:10:58.427377761-04:00" level=info msg="API listen on /var/run/docker.sock"
Hint: Some lines were ellipsized, use -l to show in full.

[root@k8s-slave ~]# yum -y install docker
[root@k8s-slave ~]# systemctl start docker.service
[root@k8s-slave ~]# systemctl status docker.service
● docker.service - Docker Application Container Engine
   Loaded: loaded (/usr/lib/systemd/system/docker.service; disabled; vendor preset: disabled)
   Active: active (running) since 六 2018-08-18 11:11:14 EDT; 3s ago
     Docs: http://docs.docker.com
 Main PID: 1517 (dockerd-current)
   CGroup: /system.slice/docker.service
           ├─1517 /usr/bin/dockerd-current --add-runtime docker-runc=/usr/libexec/docker/docker-runc-current --default-runtime=docker-runc --exec-opt native.cgroupdriver=systemd ...
           └─1521 /usr/bin/docker-containerd-current -l unix:///var/run/docker/libcontainerd/docker-containerd.sock --metrics-interval=0 --start-timeout 2m --state-dir /var/run/d...

8月 18 11:11:14 k8s-slave.localdomain dockerd-current[1517]: time="2018-08-18T11:11:14.647943371-04:00" level=warning msg="Docker could not enable SELinux on the host system"
8月 18 11:11:14 k8s-slave.localdomain dockerd-current[1517]: time="2018-08-18T11:11:14.678117451-04:00" level=info msg="Graph migration to content-addressability took 0.00 seconds"
8月 18 11:11:14 k8s-slave.localdomain dockerd-current[1517]: time="2018-08-18T11:11:14.678885165-04:00" level=info msg="Loading containers: start."
8月 18 11:11:14 k8s-slave.localdomain dockerd-current[1517]: time="2018-08-18T11:11:14.732225305-04:00" level=info msg="Firewalld running: false"
8月 18 11:11:14 k8s-slave.localdomain dockerd-current[1517]: time="2018-08-18T11:11:14.822182514-04:00" level=info msg="Default bridge (docker0) is assigned with an IP ... address"
8月 18 11:11:14 k8s-slave.localdomain dockerd-current[1517]: time="2018-08-18T11:11:14.903524638-04:00" level=info msg="Loading containers: done."
8月 18 11:11:14 k8s-slave.localdomain dockerd-current[1517]: time="2018-08-18T11:11:14.927271723-04:00" level=info msg="Daemon has completed initialization"
8月 18 11:11:14 k8s-slave.localdomain dockerd-current[1517]: time="2018-08-18T11:11:14.927306463-04:00" level=info msg="Docker daemon" commit="dded712/1.13.1" graphdriv...on=1.13.1
8月 18 11:11:14 k8s-slave.localdomain systemd[1]: Started Docker Application Container Engine.
8月 18 11:11:14 k8s-slave.localdomain dockerd-current[1517]: time="2018-08-18T11:11:14.952806378-04:00" level=info msg="API listen on /var/run/docker.sock"
Hint: Some lines were ellipsized, use -l to show in full.
```

下载registry镜像并启动registry容器

```
[root@k8s-master ~]# docker pull registry
[root@k8s-master ~]# docker run -d --name registry-container --restart always -p 5000:5000 -v /data/docker/registry:/tmp/registry docker.io/registry
98dfeb9dc851f54c725ee4a27f058f71ef926b3832f61a4baf8514d354f63531
[root@k8s-master ~]# curl -X GET 127.0.0.1:5000/v2/_catalog
{"repositories":[]}
```

测试镜像仓库

```
[root@k8s-master ~]# docker pull centos:7.5.1804
[root@k8s-master ~]# docker images
REPOSITORY           TAG                 IMAGE ID            CREATED             SIZE
docker.io/centos     7.5.1804            fdf13fa91c6e        11 days ago         200 MB
docker.io/registry   latest              b2b03e9146e1        6 weeks ago         33.3 MB

[root@k8s-master ~]# docker tag fdf13fa91c6e 192.168.56.200:5000/centos

[root@k8s-master ~]# vi /etc/sysconfig/docker
OPTIONS='--selinux-enabled --log-driver=journald --signature-verification=false --insecure-registry 192.168.56.200:5000'
[root@k8s-master ~]# systemctl restart docker

[root@k8s-master ~]# docker push 192.168.56.200:5000/centos
The push refers to a repository [192.168.56.200:5000/centos]
bcc97fbfc9e1: Pushed 
latest: digest: sha256:7c14180942615fef85cb5c8b1388e028be1a8f79694a5fa30a4025173e42ad61 size: 529

[root@k8s-master ~]# curl -X GET http://192.168.56.200:5000/v2/_catalog
{"repositories":["centos"]}
[root@k8s-master ~]# curl -X GET http://192.168.56.200:5000/v2/centos/tags/list
{"name":"centos","tags":["latest"]}
```

客户端测试

```
[root@k8s-slave ~]# vi /etc/sysconfig/docker
OPTIONS='--selinux-enabled --log-driver=journald --signature-verification=false --insecure-registry 192.168.56.200:5000'
[root@k8s-slave ~]# systemctl restart docker

[root@k8s-slave ~]# docker pull 192.168.56.200:5000/centos
Using default tag: latest
Trying to pull repository 192.168.56.200:5000/centos ... 
latest: Pulling from 192.168.56.200:5000/centos
7dc0dca2b151: Extracting [================================================>  ] 71.86 MB/74.69 MB

[root@k8s-slave ~]# docker images
REPOSITORY                   TAG                 IMAGE ID            CREATED             SIZE
192.168.56.200:5000/centos   latest              fdf13fa91c6e        11 days ago         200 MB
```

制作nginx镜像并上传到私有仓库

```
[root@k8s-master ~]# docker pull ubuntu:18.04
[root@k8s-master ubuntu-nginx-dockerfile]# docker images
REPOSITORY                   TAG                 IMAGE ID            CREATED             SIZE
192.168.56.200:5000/centos   latest              fdf13fa91c6e        11 days ago         200 MB
docker.io/centos             7.5.1804            fdf13fa91c6e        11 days ago         200 MB
docker.io/ubuntu             18.04               735f80812f90        3 weeks ago         83.5 MB
docker.io/registry           latest              b2b03e9146e1        6 weeks ago         33.3 MB

[root@k8s-master ubuntu-nginx-dockerfile]# vi Dockerfile
# 指定基于的基础镜像
FROM ubuntu:18.04
# 维护者信息
MAINTAINER yujiang
# 更新软件
RUN sed -i 's/http:\/\/archive\.ubuntu\.com\/ubuntu\//http:\/\/mirrors\.aliyun\.com\/ubuntu\//g' /etc/apt/sources.list
RUN apt-get update
# 安装软件
RUN apt-get install nginx net-tools curl vim -y
# 允许指定的端口
EXPOSE 80
CMD ["nginx", "-g", "daemon off;"]

[root@k8s-master ubuntu-nginx-dockerfile]# docker build -t 192.168.56.200/nginx:v1.0 .

```

