---
title: redis笔记
date: 2019-01-15 01:16:51
tags: redis
---

对于redis的用处不必多说，互联网上有很多资料。文章只做为笔记使用，记录redis操作与学习过程。

# redis安装

安装redis很简单，`https://redis.io/download`官方文档有教程。

```
[root@centos ~]# wget http://download.redis.io/releases/redis-5.0.3.tar.gz && tar xzf redis-5.0.3.tar.gz && cd redis-5.0.3 && make && make install
```

# redis启动方式

非守护进程启动方式

```
[root@centos ~]# redis-server

或者传递一些配置参数
[root@centos ~]# redis-server --port 6380
```

守护进程启动方式

```
配置文件中开启daemonize yes
[root@centos ~]# redis-server <configPath>

[root@centos ~]# mkdir -p /var/log/redis
[root@centos redis-5.0.3]# vim myredis.conf 
port 6379
daemonize yes
logfile "redis.log"
dir "/var/log/redis"

[root@centos redis-5.0.3]# redis-server ./myredis.conf 
[root@centos ~]# ps -ef | grep redis
root      2447     1  0 11:01 ?        00:00:00 redis-server *:6379
```

# redis常用配置

```
daemonize	是否是守护进程(no|yes)
port		redis对外端口号
logfile		redis系统日志
dir			redis工作目录

redis默认端口是6379
```

从默认配置文件中导出常用的配置项

```
去掉"#"，去掉空格
[root@centos redis-5.0.3]# cat redis.conf | grep -v "#" | grep -v "^$" > myredis.conf
```

# redis API

通过`redis-cli`命令进入客户端，测试API

## 通用命令

### 通用命令

keys [pattern]（一般不在生产环境使用，O(n)，redis单线程会阻塞其他命令）

```
127.0.0.1:6379> set hello world
OK
127.0.0.1:6379> set php good
OK
127.0.0.1:6379> set java best
OK
127.0.0.1:6379> keys *
1) "hello"
2) "java"
3) "php"

127.0.0.1:6379> mset 1 1 2 2 3 3 4 4 5 5 12 12 13 13 123 123 1234 1234
OK
127.0.0.1:6379> keys 1*
1) "1"
2) "12"
3) "13"
4) "123"
5) "1234"
127.0.0.1:6379> keys 1[2-3]*
1) "12"
2) "13"
3) "123"
4) "1234"
127.0.0.1:6379> keys 12?
1) "123"
```

dbsize

```
127.0.0.1:6379> set hello world
OK
127.0.0.1:6379> set php good
OK
127.0.0.1:6379> set java best
OK
127.0.0.1:6379> keys *
1) "hello"
2) "java"
3) "php"
127.0.0.1:6379> dbsize
(integer) 3
```

exists key

```

```

del key [key ...]

```

```

expire key seconds

```

```

type key

```

```

### 数据结构和内部编码



### 单线程架构



字符串类型

```

```

哈希类型

```

```

列表类型

```

```

集合类型

```

```

有序集合类型

```

```





























