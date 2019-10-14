---
title: Ceph MESSENGER V2
date: 2019-10-14 11:00:56
tags: ceph
---

# WHAT IS IT

messenger v2 protocol（msgr2）是Ceph’s on-wire protocol第二次主要修订。它具有几个关键功能：

- 安全模式，对通过网络传递的所有数据进行加密
- 改进了authentication payloads的封装，未来可以集成新的authentication模式（例如Kerberos）
- 改进了早期的advertisement和negotiation（协商）功能，支持未来的protocol（协议）修订

Ceph daemons现在可以绑定到多个端口，从而允许旧版Ceph客户端和支持v2的新客户端连接到同一集群。

默认情况下，monitors现在绑定到新的v2协议的新IANA分配端口3300（ce4h或0xce4），同时还绑定到旧的默认端口6789（旧的v1协议）。

# ADDRESS FORMATS

在nautilus之前，所有网络地址都呈现为1.2.3.4:567/89012，其中有一个IP地址，一个端口和一个随机数，以唯一地标识网络上的客户端或daemon 。 从nautilus开始，我们现在具有三种不同的地址类型：

- v2：`v2:1.2.3.4:578/89012`标识daemon绑定到新v2协议的端口

- v1：`v1:1.2.3.4:578/89012`标识绑定到旧版v1协议的端口的daemon。 以前使用任何前缀显示的任何地址现在都显示为v1: address。

- TYPE_ANY地址标识表示客户端可以支持两种协议版本。 在nautilus之前，客户端将显示为`1.2.3.4:0/123456`，其中端口0表示它们是客户端，并且不接受连接。 从Nautilus开始，这些客户端现在在内部由TYPE_ANY address表示，并且仍显示为不带前缀，因为它们可能会使用v2或v1协议连接到daemons，具体取决于daemons使用的协议。

因为daemons现在绑定到多个端口，所以现在用地址向量而不是单个地址来描述它们。 例如，在Nautilus cluster上dump monitor map会有如下输出：

```
[root@ceph1 ~]# ceph mon dump
epoch 1
fsid 50fcf227-be32-4bcb-8b41-34ca8370bd16
last_changed 2019-02-25 11:10:46.700821
created 2019-02-25 11:10:46.700821
min_mon_release 14 (nautilus)
0: [v2:10.0.0.10:3300/0,v1:10.0.0.10:6789/0] mon.foo
1: [v2:10.0.0.11:3300/0,v1:10.0.0.11:6789/0] mon.bar
2: [v2:10.0.0.12:3300/0,v1:10.0.0.12:6789/0] mon.baz
```

方括号或地址向量表示可以在多个端口（和协议）上访问同一daemon。如果可能，任何连接到该daemon的客户端或其他daemon都将使用v2协议（listed first）； 否则，它将返回到旧版v1协议。 旧版客户端将仅看到v1地址，并且将继续使用v1协议进行连接。

从Nautilus开始，mon_host配置选项和`-m <mon-host>`命令行选项支持相同的带括号的地址向量语法。

## BIND CONFIGURATION OPTIONS

两个新的配置选项控制是否使用v1 and/or v2协议：

- ms_bind_msgr1 [default: true]控制daemon是否绑定到使用v1协议的端口
- ms_bind_msgr2 [default: true]控制daemon是否绑定到使用v2协议的端口

同样，两个选项控制是否使用IPv4和IPv6地址：

- ms_bind_ipv4 [默认值：true]控制daemon是否绑定到IPv4地址
- ms_bind_ipv6 [默认值：false]控制daemon是否绑定到IPv6地址

# CONNECTION MODES

v2协议支持两种连接模式：

- crc模式提供：

  - 建立连接时进行强大的初始身份验证（通过cephx，双方相互认证，防止中间人或窃听者进入）
  - CRC32C完整性检查，以防止由于flaky hardware或cosmic rays引起的位翻转

  crc模式不提供：

  - 加密（网络上的窃听者可以看到所有经过身份验证后的流量）
  - 免受恶意中间人的攻击（只要他们调整crc32c的值以使其匹配，就可以故意修改流量）

- secure模式提供：

  - 建立连接时进行强大的初始身份验证（通过cephx，双方相互认证，防止中间人或窃听者进入）
  - 对所有认证后流量完全加密，包括加密完整性检查。

  在Nautilus中，secure模式使用[AES-GCM](https://en.wikipedia.org/wiki/Galois/Counter_Mode) stream cipher，这在现代处理器上通常非常快（例如，比SHA-256 cryptographic hash更快）。

## CONNECTION MODE CONFIGURATION OPTIONS

对于大多数连接，有一些选项可以控制使用哪种模式：

- ms_cluster_mode用于Ceph daemons之间的集群内通信的连接模式（或允许的模式）。 如果列出了多个模式，则首选第一个列出的模式。
- ms_service_mode是客户端连接到群集时允许使用的模式的列表。
- ms_client_mode是按优先顺序排列的连接模式列表，供客户端在与Ceph群集通信时使用（或允许）。

有一组并行的选项专门适用于monitors，允许管理员设置与monitors通信的不同（通常更安全）要求。

- ms_mon_cluster_mode是monitors之间使用的连接模式（或允许的模式）。
- ms_mon_service_mode是客户端或其他Ceph daemons连接到monitors时允许使用的模式的列表。
- ms_mon_client_mode是按优先顺序排列的连接模式列表，供客户端或non-monitor daemons在连接monitors时使用。

# TRANSITIONING FROM V1-ONLY TO V2-PLUS-V1

默认情况下，从Nautilus 14.2.z开始，ms_bind_msgr2为true。 但是，在monitors开始使用v2之前，只有有限的服务可以使用v2地址。

对于大多数用户，monitors已绑定到v1协议的默认旧版端口6789。 在这种情况下，启用v2非常简单：

```
ceph mon enable-msgr2
```

如果monitors绑定到non-standard端口，则需要为v2明确指定其端口。例如，如果monitors mon.a绑定到1.2.3.4:1111，并且您想要在端口1112上添加v2，则：

```
ceph mon set-addrs a [v2:1.2.3.4:1112,v1:1.2.3.4:1111]
```

monitors绑定到v2后，每个daemon将在下一次重新启动时开始使用v2地址。

# UPDATING CEPH.CONF AND MON_HOST

在Nautilus之前，CLI用户或daemon通常将通过/etc/ceph/ceph.conf中的mon_host选项发现monitors。 此选项的语法从Nautilus开始扩展，以允许支持新的方括号列表格式。 例如，像这样的旧行：

```
mon_host = 10.0.0.1:6789,10.0.0.2:6789,10.0.0.3:6789
```

可以更改为：

```
mon_host = [v2:10.0.0.1:3300/0,v1:10.0.0.1:6789/0],[v2:10.0.0.2:3300/0,v1:10.0.0.2:6789/0],[v2:10.0.0.3:3300/0,v1:10.0.0.3:6789/0]
```

但是，使用默认端口（3300和6789）时，可以将其省略：

```
mon_host = 10.0.0.1,10.0.0.2,10.0.0.3
```

一旦在monitors上启用了v2，可能需要更新ceph.conf以不指定任何端口（这通常是最简单的），或者显式指定v2和v1地址。 但是请注意，Nautilus和更高版本才能理解新的带括号语法，因此请不要在尚未升级其ceph packages的主机上进行此更改。

当更新ceph.conf时，请注意新的`ceph config generate-minimal-conf`命令（它生成一个简单的配置文件，其中包含足够的信息来访问monitors）而`ceph config assimilate-conf`（将配置文件选项移动到monitors’配置数据库中）可能会有所帮助。 例如，：

```
# ceph config assimilate-conf < /etc/ceph/ceph.conf
# ceph config generate-minimal-config > /etc/ceph/ceph.conf.new
# cat /etc/ceph/ceph.conf.new
# minimal ceph.conf for 0e5a806b-0ce5-4bc6-b949-aa6f68f5c2a3
[global]
        fsid = 0e5a806b-0ce5-4bc6-b949-aa6f68f5c2a3
        mon_host = [v2:10.0.0.1:3300/0,v1:10.0.0.1:6789/0]
# mv /etc/ceph/ceph.conf.new /etc/ceph/ceph.conf
```

# PROTOCOL

有关v2 wire protocol的详细说明，请参见[msgr2 protocol](https://docs.ceph.com/docs/nautilus/dev/msgr2/#msgr2-protocol)。
