---
title: Linux NFS文件共享
date: 2019-01-09 22:21:13
tags: NFS
---

# NFS

NFS（Network File System）是Linux系统之间（类Unix系统之间）使用最为广泛的文件共享协议，不同于ftp或http，需要将文件下载后使用，NFS方式的共享是可以直接使用而不需要下载的。

# NFS协议

参考[Wiki](https://zh.wikipedia.org/wiki/%E7%BD%91%E7%BB%9C%E6%96%87%E4%BB%B6%E7%B3%BB%E7%BB%9F)

## NFSv1

只在SUN公司内部用作实验目的

## NFSv2

- 基于UDP
- 由于 32-bit 的限制，NFSv2 只允读写文件起始2G大小的内容

## NFSv3

其主要目的是解决NFSv2进行同步写操作的性能问题。

- 支持 64 bit 文件大小和偏移量，即突破 2GB 文件大小的限制
- 支持服务端的异步写操作，提升写入性能
- 在许多响应报文中额外增加文件属性，避免用到这些属性时重新获取
- 增加READDIRPLUS调用，用于在遍历目录时获取文件描述符和文件属性
- 增加了将TCP作为传输层的支持

## NFSv4

协议的实现/开发工作不再是由SUN公司主导开发，而是改为由互联网工程任务组（IETF）开发。

- 性能提升
- 强制安全策略
- 引入有状态的协议

## NFSv4.1

- 为并行访问可横向扩展的集群服务（pNFS扩展）提供协议支持

## NFSv4.2

NFSv4.2 当前正在开发中

# 如何使用

假设一个Unix风格的场景，其中一台计算机（客户端）需要访问存储在其他机器上的数据（NFS 服务器）：

1、服务端实现 NFS 守护进程，默认运行 nfsd，用来使得数据可以被客户端访问。
2、服务端系统管理员可以决定哪些资源可以被访问，导出目录的名字和参数，通常使用 /etc/exports 配置文件 和 exportfs 命令。
3、服务端安全-管理员保证它可以组织和认证合法的客户端。
4、服务端网络配置保证可以跟客户端透过防火墙进行协商。
5、客户端请求导出的数据，通常调用一个 mount 命令。
6、如果一切顺利，客户端的用户就可以通过已经挂载的文件系统查看和访问服务端的文件了。
提醒：NFS自动挂载可以通过—可能是 /etc/fstab 或者自动安装管理进程。

## 环境信息

NFS服务端IP地址为：192.168.56.101

NFS客户端IP地址为：192.168.56.240

## NFS服务端

1、在Centos7上安装软件包

```
[root@centos ~]# yum install -y nfs-utils rpcbind
```

2、创建共享目录并更改权限

```
[root@centos ~]# mkdir /var/nfsshare && chmod -R 755 /var/nfsshare

[root@centos ~]# chown nfsnobody:nfsnobody /var/nfsshare
[root@centos ~]# ll -l /var/
drwxr-xr-x.  2 nfsnobody nfsnobody    6 1月   9 10:41 nfsshare
```

3、启动服务并设置开机自启

```
[root@centos ~]# systemctl enable rpcbind && systemctl enable nfs-server && systemctl enable nfs-lock && systemctl enable nfs-idmap && systemctl start rpcbind && systemctl start nfs-server && systemctl start nfs-lock && systemctl start nfs-idmap

重启所有服务命令
[root@centos ~]# systemctl enable rpcbind && systemctl enable nfs-server && systemctl enable nfs-lock && systemctl enable nfs-idmap && systemctl restart rpcbind && systemctl restart nfs-server && systemctl restart nfs-lock && systemctl restart nfs-idmap

无需重启NFS服务，使exports文件生效
[root@centos ~]# exportfs -ra
```

4、编辑/etc/exports文件，通过网络共享NFS目录

```
注意：192.168.56.240是NFS客户端IP
```

```
[root@centos ~]# vi /etc/exports
/var/nfsshare    192.168.56.240(rw,sync,no_root_squash,no_all_squash)
/home            192.168.56.240(rw,sync,no_root_squash,no_all_squash)

# 配置文件分三段
# 第一段是将要共享出去的本地目录
# 第二段是允许访问的主机(可以是一个IP也可以是一个IP段192.168.56.0/24)
# 第三段是权限

客户端							示例
使用IP地址指定单一主机		10.20.30.40
使用IP地址指定范围主机		172.16.0.0/16
使用IP地址指定范围主机		192.168.1.*
使用域名指定单一主机			Test.ice.apple
使用域名指定范围主机			*.ice.apple
使用通配符指定所有主机			*

参数						说明
ro						设置共享权限为只读
rw						设置共享权限为读写
root_squash				共享目录的使用者是root时，将被映射为匿名账号（nobody）
no_root_squash			当使用NFS服务器共享目录的使用者是root时，将不被映射为匿名账号
all_squash				将所有使用NFS服务器共享目录的使用者都映射为匿名账号
anonuid					设置匿名账号的UID
anongid					设置匿名账号的GID
sync					将数据同步写入内存和硬盘。这可能导致效率降低
async					先将数据保存在内存中，而不是直接保存在硬盘
```

```
选项含义：
	rw                  允许对共享目录进行读写
	sync                实时同步共享目录
	no_root_squash      允许root访问
	no_all_squash       允许用户授权

默认选项如下：
	ro                  只读共享
	sync                所有操作返回前必须已经写入磁盘
	wdelay              延迟写操作，等待更多的写操作一起执行
	root_squash         防止远程root用户使用root权限对共享进行访问。NFS客户端连接服务端时如果使用的是root的话，那么对服务端分享的目录来说，拥有匿名用户权限，通常他将使用nobody或nfsnobody身份。
	
常用选项：
	rw                  读写共享
	async               操作可以延迟写入硬盘，立即响应，可提高速度
	no_wdelay           关闭写延迟，需要与sync选项同时使用
	no_root_squash      关闭root_squash，NFS客户端连接服务端时如果使用的是root的话，那么对服务端分享的目录来说，也拥有root权限。显然开启这项是不安全的。
	secure              此选项要求请求源自小于IPPORT_RESERVED（1024）的Internet端口。限制客户端只能从小于1024的tcp/ip端口连接nfs服务器（默认设置）
	insecure            此选项接受所有端口，允许客户端从大于1024的tcp/ip端口连接服务器
```

5、重启服务

```
[root@centos ~]# systemctl restart nfs-server
```

6、修改防火墙

```
[root@centos ~]# firewall-cmd --permanent --zone=public --add-service=nfs
[root@centos ~]# firewall-cmd --permanent --zone=public --add-service=mountd
[root@centos ~]# firewall-cmd --permanent --zone=public --add-service=rpc-bind
[root@centos ~]# firewall-cmd --reload
```

```
挂载选项：
	ro                  只读挂载
	rw                  读写挂载
	vers=4              指定使用NFSv4方式挂载
```

## NFS客户端

1、在Centos7上安装软件包

```
[root@docker ~]# yum install -y nfs-utils
```

2、创建NFS挂载点

```
[root@docker ~]# mkdir -p /mnt/nfs/home
[root@docker ~]# mkdir -p /mnt/nfs/var/nfsshare
```

3、挂载服务端的NFS共享目录到客户端

```
[root@docker ~]# mount -t nfs 192.168.56.101:/home /mnt/nfs/home/
[root@docker ~]# mount -t nfs 192.168.56.101:/var/nfsshare /mnt/nfs/var/nfsshare/
```

4、检查

```
[root@docker ~]# df -Tkh
文件系统                     类型      容量  已用  可用 已用% 挂载点
192.168.56.101:/home         nfs4       17G  4.6G   13G   27% /mnt/nfs/home
192.168.56.101:/var/nfsshare nfs4       17G  4.6G   13G   27% /mnt/nfs/var/nfsshare
```

5、读写文件

在客户端向挂载点写入文件

```
[root@docker ~]# touch /mnt/nfs/var/nfsshare/test_nfs
[root@docker ~]# ll /mnt/nfs/var/nfsshare/
总用量 0
-rw-r--r--. 1 root root 0 1月   9 11:01 test_nfs
```

在服务端检查写入的文件

```
[root@centos ~]# ll /var/nfsshare/
总用量 0
-rw-r--r--. 1 root root 0 1月   9 11:01 test_nfs
```





# 参考文章

```
【1】https://www.linuxidc.com/Linux/2013-10/91137.htm
```

