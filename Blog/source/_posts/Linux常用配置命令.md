---
title: Linux常用配置命令
date: 2018-06-08 20:23:59
tags: Linux
---

## 系统环境

```
LSB Version:	:core-4.1-amd64:core-4.1-noarch:cxx-4.1-amd64:cxx-4.1-noarch:desktop-4.1-amd64:desktop-4.1-noarch:languages-4.1-amd64:languages-4.1-noarch:printing-4.1-amd64:printing-4.1-noarch
Distributor ID:	CentOS
Description:	CentOS Linux release 7.4.1708 (Core) 
Release:	7.4.1708
Codename:	Core
```

## ip

```
[root@cephL ~]# ip  -h
Usage: ip [ OPTIONS ] OBJECT { COMMAND | help }
       ip [ -force ] -batch filename
where  OBJECT := { link | address | addrlabel | route | rule | neigh | ntable |
                   tunnel | tuntap | maddress | mroute | mrule | monitor | xfrm |
                   netns | l2tp | macsec | tcp_metrics | token }
       OPTIONS := { -V[ersion] | -s[tatistics] | -d[etails] | -r[esolve] |
                    -h[uman-readable] | -iec |
                    -f[amily] { inet | inet6 | ipx | dnet | bridge | link } |
                    -4 | -6 | -I | -D | -B | -0 |
                    -l[oops] { maximum-addr-flush-attempts } |
                    -o[neline] | -t[imestamp] | -ts[hort] | -b[atch] [filename] |
                    -rc[vbuf] [size] | -n[etns] name | -a[ll] }
```

### 获取系统上所有网络接口的信息

```
[root@cephL ~]# ip a
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN qlen 1
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
    inet 127.0.0.1/8 scope host lo
       valid_lft forever preferred_lft forever
    inet6 ::1/128 scope host 
       valid_lft forever preferred_lft forever
2: enp0s3: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP qlen 1000
    link/ether 08:00:27:3a:98:1d brd ff:ff:ff:ff:ff:ff
3: enp0s8: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP qlen 1000
    link/ether 08:00:27:52:03:aa brd ff:ff:ff:ff:ff:ff
    inet 192.168.56.102/24 brd 192.168.56.255 scope global dynamic enp0s8
       valid_lft 945sec preferred_lft 945sec
    inet6 fe80::4b2b:b766:22c:f503/64 scope link 
       valid_lft forever preferred_lft forever
4: enp0s9: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP qlen 1000
    link/ether 08:00:27:3e:3e:e2 brd ff:ff:ff:ff:ff:ff
5: virbr0: <NO-CARRIER,BROADCAST,MULTICAST,UP> mtu 1500 qdisc noqueue state DOWN qlen 1000
    link/ether 52:54:00:66:bf:e6 brd ff:ff:ff:ff:ff:ff
    inet 192.168.122.1/24 brd 192.168.122.255 scope global virbr0
       valid_lft forever preferred_lft forever
6: virbr0-nic: <BROADCAST,MULTICAST> mtu 1500 qdisc pfifo_fast master virbr0 state DOWN qlen 1000
    link/ether 52:54:00:66:bf:e6 brd ff:ff:ff:ff:ff:ff

scope解释，ip地址和路由都有一个域的概念，主要是告诉内核他们起作用的上下文。
在linux中，路由的域表明了目的网络和本机的距离。
而ip地址的域说明了这个ip地址的作用域。
在linux中一共有3种作用域，分别是HOST，LINK(对于ip地址来说说明这个地址是只在lan中使用，而对于route来说目的的地址在lan中),Universe/global(广域网的地址,对于route来说要到达这个地址肯定会多于1跳)。
要注意，域的默认值是Universe，也就是说你不显示指明，则默认都是Universe。

查看IP地址
[root@cephL ~]# ip addr show enp0s8
3: enp0s8: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP qlen 1000
    link/ether 08:00:27:52:03:aa brd ff:ff:ff:ff:ff:ff
    inet 192.168.56.102/24 brd 192.168.56.255 scope global dynamic enp0s8
       valid_lft 847sec preferred_lft 847sec
    inet6 fe80::4b2b:b766:22c:f503/64 scope link 
       valid_lft forever preferred_lft forever

查看IPv6地址
[root@cephL ~]# ip -6 addr show dev enp0s8
3: enp0s8: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 state UP qlen 1000
    inet6 fe80::4b2b:b766:22c:f503/64 scope link 
       valid_lft forever preferred_lft forever

添加IP地址
[root@cephL ~]# ip addr add 192.168.56.103/24 dev enp0s8
删除IP地址
[root@cephL ~]# ip addr del 192.168.56.103/24 dev enp0s8
```

路由

```
[root@cephL ~]# ip route show
192.168.56.0/24 dev enp0s8 proto kernel scope link src 192.168.56.102 metric 100 
192.168.122.0/24 dev virbr0 proto kernel scope link src 192.168.122.1

让kernel报告路由，它会发送一个数据包到指定的地址
[root@cephL ~]# ip route get 61.135.169.125
61.135.169.125 via 10.116.21.254 dev enp0s9 src 10.116.21.135 
    cache
（解释：61.135.169.125是访问的地址，10.116.21.254是默认路由，10.116.21.135是enp0s9设备IP地址）

添加默认路由
[root@cephL ~]# ip route add default via 10.116.21.253 dev enp0s9
删除默认路由
[root@cephL ~]# ip route del default via 10.116.21.253 dev enp0s9
```

不同网络接口的统计信息

```
[root@cephL ~]# ip -s link
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN mode DEFAULT qlen 1
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
    RX: bytes  packets  errors  dropped overrun mcast   
    189813592  61558    0       0       0       0       
    TX: bytes  packets  errors  dropped carrier collsns 
    189813592  61558    0       0       0       0       
2: enp0s3: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP mode DEFAULT qlen 1000
    link/ether 08:00:27:3a:98:1d brd ff:ff:ff:ff:ff:ff
    RX: bytes  packets  errors  dropped overrun mcast   
    0          0        0       0       0       0       
    TX: bytes  packets  errors  dropped carrier collsns 
    0          0        0       0       0       0       
3: enp0s8: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP mode DEFAULT qlen 1000
    link/ether 08:00:27:52:03:aa brd ff:ff:ff:ff:ff:ff
    RX: bytes  packets  errors  dropped overrun mcast   
    104188     1104     0       0       0       0       
    TX: bytes  packets  errors  dropped carrier collsns 
    159594     673      0       0       0       0       
4: enp0s9: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP mode DEFAULT qlen 1000
    link/ether 08:00:27:3e:3e:e2 brd ff:ff:ff:ff:ff:ff
    RX: bytes  packets  errors  dropped overrun mcast   
    53777      574      0       0       0       0       
    TX: bytes  packets  errors  dropped carrier collsns 
    60106      623      0       0       0       0       
5: virbr0: <NO-CARRIER,BROADCAST,MULTICAST,UP> mtu 1500 qdisc noqueue state DOWN mode DEFAULT qlen 1000
    link/ether 52:54:00:66:bf:e6 brd ff:ff:ff:ff:ff:ff
    RX: bytes  packets  errors  dropped overrun mcast   
    0          0        0       0       0       0       
    TX: bytes  packets  errors  dropped carrier collsns 
    0          0        0       0       0       0       
6: virbr0-nic: <BROADCAST,MULTICAST> mtu 1500 qdisc pfifo_fast master virbr0 state DOWN mode DEFAULT qlen 1000
    link/ether 52:54:00:66:bf:e6 brd ff:ff:ff:ff:ff:ff
    RX: bytes  packets  errors  dropped overrun mcast   
    0          0        0       0       0       0       
    TX: bytes  packets  errors  dropped carrier collsns 
    0          0        0       0       0       0       

查询特定接口统计信息
[root@cephL ~]# ip -s link ls enp0s9
4: enp0s9: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP mode DEFAULT qlen 1000
    link/ether 08:00:27:3e:3e:e2 brd ff:ff:ff:ff:ff:ff
    RX: bytes  packets  errors  dropped overrun mcast   
    54527      583      0       0       0       0       
    TX: bytes  packets  errors  dropped carrier collsns 
    60946      633      0       0       0       0 
```

ARP，查看接入的MAC地址

```
[root@cephL ~]# ip neighbour
192.168.56.1 dev enp0s8 lladdr 0a:00:27:00:00:00 DELAY
10.116.21.254 dev enp0s9 lladdr 00:00:0c:07:ac:0c DELAY
10.116.21.253 dev enp0s9 lladdr b4:de:31:c7:e7:47 STALE
192.168.56.100 dev enp0s8 lladdr 08:00:27:3e:18:34 STALE
10.116.21.252 dev enp0s9 lladdr b4:de:31:c7:d8:e7 STALE
```

ip工具可以连续监视devices, addresses和routes的状态。

```
[root@cephL ~]# ip monitor all
[nsid current]192.168.56.1 dev enp0s8 lladdr 0a:00:27:00:00:00 REACHABLE
[nsid current]10.116.21.254 dev enp0s9 lladdr 00:00:0c:07:ac:0c STALE
[nsid current]192.168.56.1 dev enp0s8 lladdr 0a:00:27:00:00:00 REACHABLE
[nsid current]10.116.21.254 dev enp0s9 lladdr 00:00:0c:07:ac:0c REACHABLE
[nsid current]3: enp0s8    inet 192.168.56.102/24 brd 192.168.56.255 scope global dynamic enp0s8
       valid_lft 1200sec preferred_lft 1200sec
```

up和down网络设备

```
[root@cephL ~]# ip link set enp0s9 down 
[root@cephL ~]# ip route show
192.168.56.0/24 dev enp0s8 proto kernel scope link src 192.168.56.102 metric 100 
192.168.122.0/24 dev virbr0 proto kernel scope link src 192.168.122.1
[root@cephL ~]# ping www.baidu.com
ping: www.baidu.com: 未知的名称或服务

[root@cephL ~]# ip link set enp0s9 up
[root@cephL ~]# ip route show
default via 10.116.21.254 dev enp0s9 proto static metric 100 
10.116.20.0/23 dev enp0s9 proto kernel scope link src 10.116.21.135 metric 100 
192.168.56.0/24 dev enp0s8 proto kernel scope link src 192.168.56.102 metric 100 
192.168.122.0/24 dev virbr0 proto kernel scope link src 192.168.122.1
[root@cephL ~]# ping www.baidu.com
PING www.a.shifen.com (61.135.169.121) 56(84) bytes of data.
64 bytes from 61.135.169.121 (61.135.169.121): icmp_seq=1 ttl=50 time=4.35 ms
```

获取帮助信息

```
[root@cephL ~]# ip route help
Usage: ip route { list | flush } SELECTOR
       ip route save SELECTOR
       ip route restore
       ip route showdump
       ip route get ADDRESS [ from ADDRESS iif STRING ]
                            [ oif STRING ]  [ tos TOS ]
                            [ mark NUMBER ]
       ip route { add | del | change | append | replace } ROUTE
SELECTOR := [ root PREFIX ] [ match PREFIX ] [ exact PREFIX ]
            [ table TABLE_ID ] [ proto RTPROTO ]
            [ type TYPE ] [ scope SCOPE ]
ROUTE := NODE_SPEC [ INFO_SPEC ]
NODE_SPEC := [ TYPE ] PREFIX [ tos TOS ]
             [ table TABLE_ID ] [ proto RTPROTO ]
             [ scope SCOPE ] [ metric METRIC ]
INFO_SPEC := NH OPTIONS FLAGS [ nexthop NH ]...
NH := [ via ADDRESS ] [ dev STRING ] [ weight NUMBER ] NHFLAGS
OPTIONS := FLAGS [ mtu NUMBER ] [ advmss NUMBER ]
           [ rtt TIME ] [ rttvar TIME ] [reordering NUMBER ]
           [ window NUMBER ] [ cwnd NUMBER ] [ initcwnd NUMBER ]
           [ ssthresh NUMBER ] [ realms REALM ] [ src ADDRESS ]
           [ rto_min TIME ] [ hoplimit NUMBER ] [ initrwnd NUMBER ]
           [ features FEATURES ] [ quickack BOOL ] [ congctl NAME ]
           [ expires TIME ]
TYPE := { unicast | local | broadcast | multicast | throw |
          unreachable | prohibit | blackhole | nat }
TABLE_ID := [ local | main | default | all | NUMBER ]
SCOPE := [ host | link | global | NUMBER ]
NHFLAGS := [ onlink | pervasive ]
RTPROTO := [ kernel | boot | static | NUMBER ]
TIME := NUMBER[s|ms]
BOOL := [1|0]
FEATURES := ecn
```





## 参考链接

【1】https://linux.cn/article-3144-1.html

【2】https://www.jianshu.com/p/d89cc7ecb305

【3】http://www.pagefault.info/?p=240