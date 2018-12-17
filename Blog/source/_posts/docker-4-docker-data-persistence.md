---
title: docker-4 docker data persistence
date: 2018-12-17 23:41:43
tags: docker
---

# Docker的持久化存储和数据共享

- 基于本地文件系统的volume。可以在执行docker create或docker run时，通过-v参数将主机的目录作为容器的数据卷。这部分功能便是基于本地文件系统的volume管理。

- 基于plugin的volume，支持第三方的存储方案，比如NAS，aws。

## volume的类型

- 受管理的data volume，由docker后台自动创建。

- 绑定挂载的volume，具体挂载位置可以由用户指定。

# data volume

启动mysql Container，会在宿主机/var/lib/docker/volumes/目录创建持久化目录。在mysql的Dockerfile中有`VOLUME /var/lib/mysql`，对应的是Container里面的路径。

```
[vagrant@docker-node1 ~]$ sudo docker run -d --name mysql1 -e MYSQL_ALLOW_EMPTY_PASSWORD=true mysql
9f423f85765546cb5f2f748b99e221c371794e9ae2ee910aabc685462506a23e
[vagrant@docker-node1 ~]$ docker volume ls
DRIVER              VOLUME NAME
local               5625ef5f9b225dfab10577dbf0521c4244e6ebc60bc1951befac708b196082db
[vagrant@docker-node1 ~]$ docker volume inspect 5625ef5f9b225dfab10577dbf0521c4244e6ebc60bc1951befac708b196082db
[
    {
        "CreatedAt": "2018-12-17T16:21:09Z",
        "Driver": "local",
        "Labels": null,
        "Mountpoint": "/var/lib/docker/volumes/5625ef5f9b225dfab10577dbf0521c4244e6ebc60bc1951befac708b196082db/_data",
        "Name": "5625ef5f9b225dfab10577dbf0521c4244e6ebc60bc1951befac708b196082db",
        "Options": null,
        "Scope": "local"
    }
]

[vagrant@docker-node1 ~]$ sudo docker run -d --name mysql2 -e MYSQL_ALLOW_EMPTY_PASSWORD=true mysql
12dc76117789421e6d9fbe43b6ceeb38791b94848baa0506a1f90bbccd9050fc
[vagrant@docker-node1 ~]$ docker volume ls
DRIVER              VOLUME NAME
local               595c8cee9d6483ae8a4e2006e18103298bf02f940ef05d43261071af74aa6a42
local               5625ef5f9b225dfab10577dbf0521c4244e6ebc60bc1951befac708b196082db
[vagrant@docker-node1 ~]$ docker volume inspect 595c8cee9d6483ae8a4e2006e18103298bf02f940ef05d43261071af74aa6a42
[
    {
        "CreatedAt": "2018-12-17T16:24:15Z",
        "Driver": "local",
        "Labels": null,
        "Mountpoint": "/var/lib/docker/volumes/595c8cee9d6483ae8a4e2006e18103298bf02f940ef05d43261071af74aa6a42/_data",
        "Name": "595c8cee9d6483ae8a4e2006e18103298bf02f940ef05d43261071af74aa6a42",
        "Options": null,
        "Scope": "local"
    }
]

以上的方式volume不能很好的区分volume对应哪个应用，可以在启动Container时指定-v参数。
[vagrant@docker-node1 ~]$ sudo docker run -d -v mysql:/var/lib/mysql  --name mysql1 -e MYSQL_ALLOW_EMPTY_PASSWORD=true mysql
6b768f033c3f2c17c2da569ed3a9d4490ac7f3bd17e551a24e0fd7c9f7522ed2
[vagrant@docker-node1 ~]$ docker volume ls
DRIVER              VOLUME NAME
local               mysql
创建mysql1 Container，并在Container中的mysql创建docker database，然后停止并删除Container，这时mysql的volume依然存在。
[vagrant@docker-node1 ~]$ docker exec -it mysql1 /bin/bash
root@6b768f033c3f:/# mysql -uroot
mysql> create database docker;
mysql> exit
root@6b768f033c3f:/# exit
[vagrant@docker-node1 ~]$ docker stop mysql1
[vagrant@docker-node1 ~]$ docker rm $(docker ps -qa)
创建mysql2 Container，并挂载之前名为mysql的volume，发现docker database依然存在，说明volume起作用了。
[vagrant@docker-node1 ~]$ sudo docker run -d -v mysql:/var/lib/mysql  --name mysql2 -e MYSQL_ALLOW_EMPTY_PASSWORD=true mysql
f574294e26818667095b78a7141e8eb5309b7aa27d4b027e6811e4cca3368fdf
[vagrant@docker-node1 ~]$ docker exec -it mysql2 /bin/bash
root@f574294e2681:/# mysql -u root
mysql> show databases;
+--------------------+
| Database           |
+--------------------+
| docker             |
| information_schema |
| mysql              |
| performance_schema |
| sys                |
+--------------------+
5 rows in set (0.05 sec)
```

# Bind Mouting

data volume方式需要在Dockerfile中定义VOLUME，bind mouting方式则不需要。在docker run时，指定宿主机目录与Container内目录的对应关系即可（`docker run -v /home/aaa:/root/aaa`）。