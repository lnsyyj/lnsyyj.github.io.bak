---
title: Ceph纠删码bluestore与filestore对比调研
date: 2018-05-25 11:02:06
tags: 
---

最近学习Ceph Erasure code，查阅了网络上有许多资料。Erasure code的概念与理论相对成熟，但目前在Ceph中实现与应用上有诸多困难。所以这里只对Ceph Erasure code应用场景做比较。

使用Erasure code的目的是为了节省存储空间（以牺牲计算资源换取存储资源）。

**bluestore OSD**

在luminous版本之前，只适用于像RGW那样的应用场景（full object writes and appends），不支持partial writes。

（1）用法一

在luminous版本，为了适用于RBD和CephFS的场景，增加了partial writes功能，但是只有在bluestore OSD上可以启用。

Erasure coded pools不支持omap，因此要将它们与RBD和CephFS一起使用，必须明确指示它们将data存储在ec pool中，将metadata存储在replicated pool中。

对于CephFS，在创建file system时通过[file layouts](http://docs.ceph.com/docs/master/cephfs/file-layouts/)设定一个erasure coded pool作为默认的data pool。

（2）用法二

在erasure coded pool之前设置一个cache tier。

**filestore OSD**

（1）用法一

Erasure coded pools需要比replicated pools更多的资源，并且缺少一些功能，如omap。为了克服这些限制，可以在erasure coded pool之前设置一个cache tier。

由于使用cache tier并且与Erasure coded相关联，那么Erasure coded的性能同时与cache tier特性相关。这是一个纠结的问题！

**Ceph Cache Tiering**

![](http://docs.ceph.com/docs/master/_images/ditaa-2982c5ed3031cac4f9e40545139e51fdb0b33897.png)

如上图，cache tier为客户端读取存储在后端的数据子集提供更好的I/O性能。在创建pool时，Cache Tier层pool作为Storage Tier层pool的缓存。 Ceph objecter handles 负责处理objects放置位置。 tiering agent 决定何时将缓存中的objects刷新到后端存储层。所以cache tier和后端存储层对Ceph clients来说是完全透明的。

但是，管理员可以配置此迁移的发生方式。 主要有两种情况：

Writeback Mode:

当管理员以writeback mode配置tiers时，Ceph clients将数据写入缓存层并从缓存层接收ACK。经过一段时间后，写入缓存层的数据将迁移到存储层，并从缓存层中清除。从概念上讲，缓存层被覆盖在后端存储层的“前面”。当Ceph client需要驻留在存储层中的数据时，cache tiering agent会在读取数据时将数据迁移到缓存层，然后将其发送到Ceph client。此后，Ceph client可以使用缓存层执行I/O，直到数据变为非活动状态。这对于易变数据（例如照片/视频编辑，交易数据等）是理想的。

Read-proxy Mode:

此模式将使用已存在于缓存层中的任何objects，但如果缓存中没有objects，则将请求代理到存储层。这对于从writeback mode转换为禁用缓存非常有用，因为它允许负载在缓存耗尽时正常运行，而无需向缓存中添加任何新objects。

cache tier所有迁移模式：

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/Blog/source/_posts/Ceph%E7%BA%A0%E5%88%A0%E7%A0%81bluestore%E4%B8%8Efilestore%E5%AF%B9%E6%AF%94%E8%B0%83%E7%A0%94/AAA30A7C3BB8C148411DB8D2C01C175D.jpg)

**官方文档明确指出：**

**A WORD OF CAUTION**

Cache tiering会降低大多数workloads的性能。用户在使用此功能之前应特别小心。

**Workload dependent:**

缓存是否会提高性能，高度依赖于负载。由于将objects移入或移出缓存会产生额外成本，因此只有在数据集中访问存在较大偏差时才会生效，这样大多数请求才会命中少量objects。缓存池应该大到足以捕获你的负载，以避免抖动。

**Difficult to benchmark:**

使用cache tiering，用户常规衡量性能的基准测试将显得很糟糕，部分原因是由于很少有人将请求集中在一小部分objects上，所以缓存“预热”可能需要很长时间，同时预热可能带来更高的成本。

**Usually slower:**

对于缓存分层不友好的负载，性能通常比没有设置cache tiering enabled的普通RADOS pool慢。

**librados object enumeration:**

在这种情况下，librados级别的object enumeration API并不一致。如果您的应用程序直接使用librados并依赖于object enumeration，则缓存分层可能无法按预期工作。 

**Complexity:**

启用缓存分层会带来额外的复杂性。这会增加其他用户尚未遇到的错误的可能性，并且会使您的部署处于更高级别的风险中。

**KNOWN GOOD WORKLOADS**

**RGW time-skewed:**

如果RGW负载几乎所有读取操作都针对最近写入的objects，可配置为一段时间后，将最近写入的对象从缓存层迁移到存储层，这种场景可以很好地工作。

**KNOWN BAD WORKLOADS**

已知下列配置对cache tiering效果不佳。

**RBD with replicated cache and erasure-coded base:**

这是一个常见的要求，但通常表现不佳。即使合理偏差的负载仍然会向cold objects发送一些small writes操作，由于erasure-coded pool尚不支持small writes操作，因此必须将整个（通常为4 MB）objects迁移到缓存中以满足small write（通常为4 KB）。只有少数用户成功部署了此配置，并且仅适用于他们，因为他们的数据extremely cold（备份）并且对性能没有任何要求。

**RBD with replicated cache and base:**

具有replicated base tier（存储层）的RBD效果要好于erasure coded base tier（存储层）时的RBD，但它仍高度依赖于负载中的偏差量，而且很难验证。用户需要很好地理解他们的负载，并需要调整缓存分层参数。

**总结**

1、 架构角度，如果想用Ceph Erasure code，推荐使用bluestore OSD，而不推荐filestore OSD。

抛开bluestore的稳定性不谈（待验证），filestore OSD的Ceph Erasure code依赖Ceph Cache Tiering会带来额外的复杂性，并且限制了应用场景。

摘自李运华：

组件越多，就越有可能其中某个组件出现故障，从而导致系统故障。这个概率可以算出来，假设组件的故障率是10%（有10%的时间不可用），那么有1个组件的系统可用性是(1-10%)=90%，有2个组件的系统的可用性是(1-10%)*(1-10%)=81%，两者的可用性相差9%。

2、如果使用filestore OSD的Ceph Erasure code，推荐应用在RGW time-skewed场景（官方推荐）。而Ceph Cache Tiering据生产环境落地还有一定距离。 

3、近来很多公司都在搞云计算、搞研发，有些是自主研发而有些是基于开源定制开发。如果是基于开源，根据之前的经验，代码直接提交到社区根据社区的发布流程会省去很多额外的工作。

自主研发版本控制是痛苦的，需要考虑社区base更新，自主研发的代码移植到社区最新base（查分代码到吐。。。），如果社区框架变动大，还需要调试改代码。如果是聪明的开发会找到一条扩展的路，尽量更改原有逻辑，如果更改原有逻辑，对测试来说是个很大的挑战。

4、对于产品化一个开源项目，如ceph，还是寻找一个稳定的方案，然后再搞研发，至少研发失败还给自己留条后路，因为你背后有一个强大的社区团队。







**附录**

中文文档，如看不懂请看英语原文

https://lnsyyj.github.io/2018/05/12/CEPH-ERASURE-CODE/

https://lnsyyj.github.io/2018/05/12/CACHE-TIERING/

https://lnsyyj.github.io/2018/05/15/ERASURE-CODE-PROFILES/

https://lnsyyj.github.io/2018/05/16/JERASURE-ERASURE-CODE-PLUGIN/

https://lnsyyj.github.io/2018/05/16/ISA-ERASURE-CODE-PLUGIN/

https://lnsyyj.github.io/2018/05/17/LOCALLY-REPAIRABLE-ERASURE-CODE-PLUGIN/

https://lnsyyj.github.io/2018/05/18/SHEC-ERASURE-CODE-PLUGIN/