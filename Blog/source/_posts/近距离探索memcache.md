---
title: 近距离探索memcache缓存
date: 2018-05-31 17:22:03
tags: memcache
---

近距离探索memcache缓存笔记

课程地址：https://www.imooc.com/learn/527

课程教师：李虎头

## memcache解析

1-1内容解析

```
1.知道什么是Memcache
2.知道在什么时候使用Memcache
3.知道怎样使用Memcache优化网站或API
```

1-2基础介绍

```
Memcache介绍
1.什么是Memcache？
	分布式高速缓存系统

2.Memcache有什么用？
	缓存层，前端频繁操作后端数据库时，减轻后端数据库压力，前端直接访问缓存层
```

1-3理解Memcache

```
只有一张表的数据库
```

| #    | 名字  | 类型         | 整理            |
| ---- | ----- | ------------ | --------------- |
| 1    | key   | varchar(255) | utf8_general_ci |
| 2    | value | text         | utf8_general_ci |

1-4使用场景

```
1.非持久化存储：对数据存储要求不高
2.分布式存储：不适合单机使用
3.Key/Value存储：格式简单，不支持List、Array数据格式
```

## memcache的安装

2-1安装前的说明

```
1.编译安装 Libevent Memcache(需要先安装Libevent依赖)
2.使用依赖管理工具yum、apt-get
-PS：Memcache和Memcached的区别(建议使用Memcached，它是Memcache的升级版)
```

2-2安装服务端实际操作

```
1.编译安装
yum -y install automake libtool wget gcc gcc-c++ && wget https://github.com/libevent/libevent/archive/release-2.1.8-stable.tar.gz && tar zxvf release-2.1.8-stable.tar.gz && cd libevent-release-2.1.8-stable/ && ./autogen.sh && ./configure && make -j 2 && make install

wget http://www.memcached.org/files/memcached-1.5.8.tar.gz && cd memcached-1.5.8 && ./configure && make -j 2 && make install

[root@cephL memcached-1.5.8]# /usr/local/bin/memcached -h
/usr/local/bin/memcached: error while loading shared libraries: libevent-2.1.so.6: cannot open shared object file: No such file or directory
[root@cephL memcached-1.5.8]# ldd /usr/local/bin/memcached 
	linux-vdso.so.1 =>  (0x00007ffdd7d77000)
	libevent-2.1.so.6 => not found
	libpthread.so.0 => /lib64/libpthread.so.0 (0x00007f2d78950000)
	libc.so.6 => /lib64/libc.so.6 (0x00007f2d78583000)
	/lib64/ld-linux-x86-64.so.2 (0x000055b9e254c000)
[root@cephL memcached-1.5.8]# LD_DEBUG=libs /usr/local/bin/memcached -v
     51494:	find library=libevent-2.1.so.6 [0]; searching
     51494:	 search cache=/etc/ld.so.cache
     51494:	 search path=/lib64/tls/x86_64:/lib64/tls:/lib64/x86_64:/lib64:/usr/lib64/tls/x86_64:/usr/lib64/tls:/usr/lib64/x86_64:/usr/lib64		(system search path)
     51494:	  trying file=/lib64/tls/x86_64/libevent-2.1.so.6
     51494:	  trying file=/lib64/tls/libevent-2.1.so.6
     51494:	  trying file=/lib64/x86_64/libevent-2.1.so.6
     51494:	  trying file=/lib64/libevent-2.1.so.6
     51494:	  trying file=/usr/lib64/tls/x86_64/libevent-2.1.so.6
     51494:	  trying file=/usr/lib64/tls/libevent-2.1.so.6
     51494:	  trying file=/usr/lib64/x86_64/libevent-2.1.so.6
     51494:	  trying file=/usr/lib64/libevent-2.1.so.6
     51494:	
/usr/local/bin/memcached: error while loading shared libraries: libevent-2.1.so.6: cannot open shared object file: No such file or directory
[root@cephL memcached-1.5.8]# ll /usr/local/lib/libevent-2.1.so.6
lrwxrwxrwx. 1 root root 21 5月  29 18:48 /usr/local/lib/libevent-2.1.so.6 -> libevent-2.1.so.6.0.2
[root@cephL memcached-1.5.8]# ln -s /usr/local/lib/libevent-2.1.so.6 /usr/lib64/libevent-2.1.so.6
[root@cephL memcached-1.5.8]# /usr/local/bin/memcached -d -l 127.0.0.1 -p 11211 -m 150 -u root
[root@cephL memcached-1.5.8]# ps -ef | grep memcache
root       51810       1  0 19:04 ?        00:00:00 /usr/local/bin/memcached -d -l 127.0.0.1 -p 11211 -m 150 -u root

2.使用yum安装
[root@cephL memcache]# yum install memcached
[root@cephL memcache]# /usr/bin/memcached -d -l 127.0.0.1 -p 11211 -m 150 -u root

[root@cephL memcache]# ps -ef | grep memcache
root       40789       1  0 18:34 ?        00:00:00 /usr/bin/memcached -d -l 127.0.0.1 -p 11211 -m 150 -u root
```

2-3客户端的安装

```
1.安装libmemcached
wget https://launchpad.net/libmemcached/1.0/1.0.18/+download/libmemcached-1.0.18.tar.gz
tar zxvf libmemcached-1.0.18.tar.gz && cd libmemcached-1.0.18
./configure --prefix=/usr/lib/libmemcached
make && make install
```

