---
title: CACHE TIERING
date: 2018-05-12 19:53:27
tags: CEPH-Luminous
---

cache tier为Ceph Clients读取存储在后端的数据子集提供更好的I/O性能。Cache tiering意味着创建pool时，可以为pool配置一个相对更快/更昂贵的存储设备作为缓存层，并且配置一个经济的后端数据存储pool（由erasure-coded或相对较慢/较便宜的设备组成）。Ceph objecter handles 负责处理放置objects位置，并且tiering agent 决定何时将缓存中的objects刷新到后端存储层。所以cache tier和后端存储层对Ceph clients来说是完全透明的。

![](http://docs.ceph.com/docs/master/_images/ditaa-2982c5ed3031cac4f9e40545139e51fdb0b33897.png)

cache tiering agent handles自动处理缓存层和后端存储层之间的数据迁移。但是，管理员可以配置此迁移的发生方式。 主要有两种情况：

- Writeback Mode: 

当管理员以writeback mode配置tiers时，Ceph clients将数据写入缓存层并从缓存层接收ACK。经过一段时间后，写入缓存层的数据将迁移到存储层，并从缓存层中清除。从概念上讲，缓存层被覆盖在后端存储层的“前面”。当Ceph client需要驻留在存储层中的数据时，cache tiering agent会在读取数据时将数据迁移到缓存层，然后将其发送到Ceph client。此后，Ceph client可以使用缓存层执行I/O，直到数据变为非活动状态。这对于易变数据（例如照片/视频编辑，交易数据等）是理想的。

- Read-proxy Mode: 

此模式将使用已存在于缓存层中的任何objects，但如果缓存中没有objects，则将请求代理到存储层。这对于从writeback mode转换为禁用缓存非常有用，因为它允许负载在缓存耗尽时正常运行，而无需向缓存中添加任何新objects。

## A WORD OF CAUTION

Cache tiering会降低大多数负载的性能。用户在使用此功能之前应特别小心

- Workload dependent: 

缓存是否会提高性能，高度依赖于负载。由于将objects移入或移出缓存会产生额外成本，因此只有在数据集中访问存在较大偏差时才会生效，这样大多数请求才会命中少量objects。缓存池应该大到足以捕获你的负载，以避免抖动。

- Difficult to benchmark: 

使用cache tiering，用户常规衡量性能的基准测试将显得很糟糕，部分原因是由于很少有人将请求集中在一小部分objects上，所以缓存“预热”可能需要很长时间，同时预热可能带来更高的成本。

- Usually slower: 

对于缓存分层不友好的负载，性能通常比没有设置cache tiering enabled的普通RADOS pool慢。

- librados object enumeration: 

在这种情况下，librados级别的object enumeration API并不一致。如果您的应用程序直接使用librados并依赖于object enumeration，则缓存分层可能无法按预期工作。 （这对于RGW，RBD或CephFS来说不是问题。）

- Complexity: 

启用缓存分层会带来额外的复杂性。这会增加其他用户尚未遇到的错误的可能性，并且会使您的部署处于更高级别的风险中。

### KNOWN GOOD WORKLOADS

- RGW time-skewed: 

如果RGW负载几乎所有读取操作都针对最近写入的objects，可配置为一段时间后，将最近写入的对象从缓存层迁移到存储层，这种场景可以很好地工作。

### KNOWN BAD WORKLOADS

已知下列配置对cache tiering效果不佳。

- RBD with replicated cache and erasure-coded base: 

这是一个常见的要求，但通常表现不佳。即使合理偏差的负载仍然会向cold objects发送一些small writes操作，由于erasure-coded pool尚不支持small writes操作，因此必须将整个（通常为4 MB）objects迁移到缓存中以满足small write（通常为4 KB）。只有少数用户成功部署了此配置，并且仅适用于他们，因为他们的数据extremely cold（备份）并且对性能没有任何要求。

- RBD with replicated cache and base: 

具有replicated base tier（存储层）的RBD，效果要好于erasure coded base tier（存储层）时的RBD，但它仍高度依赖于负载中的偏差量，而且很难验证。用户需要很好地理解他们的负载，并需要调整缓存分层参数。

## SETTING UP POOLS

要设置cache tiering，您必须有两个pool。 一个pool充当backing storage，另一个pool充当cache。

### SETTING UP A BACKING STORAGE POOL

设置backing storage pool通常涉及以下两种方案之一：

- Standard Storage: 

在这种情况下，pool在Ceph Storage Cluster中存储object的多个副本。

- Erasure Coding: 

在这种情况下，pool使用erasure coding来更有效地存储数据，并且性能折衷很小。

在standard storage方案中，您可以设置CRUSH rule来建立故障域（例如，osd, host, chassis, rack, row, 等）。当rule中的所有存储驱动器都具有相同的大小，速度（RPM-转速和吞吐量）和类型时，Ceph OSD Daemons会达到最佳性能。有关创建rule的详细信息，请参阅[CRUSH Maps](http://docs.ceph.com/docs/master/rados/operations/crush-map/)。一旦你创建了rule，就可以创建backing storage pool了。

在erasure coding方案中，pool创建参数将自动生成适当的rule。 有关详细信息，请参阅[Create a Pool](http://docs.ceph.com/docs/master/rados/operations/pools/#create-a-pool)。

在随后的例子中，我们将backing storage pool称为cold-storage。

### SETTING UP A CACHE POOL

设置cache pool遵循与standard storage方案相同的过程，但有这样的区别：作为cache tier（缓存层）的驱动器通常是服务器中的高性能驱动器，并具有自己的CRUSH rule。在设置这样的rule时，应该考虑具有高性能驱动器的主机，忽略不具有高性能驱动器的主机。有关详细信息，请参阅[Placing Different Pools on Different OSDs](http://docs.ceph.com/docs/master/rados/operations/crush-map-edits/#placing-different-pools-on-different-osds)。

在后面的例子中，我们将把cache pool称为hot-storage，将backing pool称为cold-storage。

有关cache tier配置和默认值，请参阅[Pools - Set Pool Values](http://docs.ceph.com/docs/master/rados/operations/pools/#set-pool-values)。

## CREATING A CACHE TIER

设置cache tier涉及到关联backing storage pool和cache pool。

```
ceph osd tier add {storagepool} {cachepool}
```

例如：

```
ceph osd tier add cold-storage hot-storage
```

要设置cache模式，请执行以下操作：

```
ceph osd tier cache-mode {cachepool} {cache-mode}
```

例如：

```
ceph osd tier cache-mode hot-storage writeback
```

cache tiers覆盖backing storage tier，他们需要一个额外的步骤：您必须将所有客户端流量从storage pool引导到cache pool。要将客户端流量直接指向cache pool，请执行以下操作：

```
ceph osd tier set-overlay {storagepool} {cachepool}
```

例如：

```
ceph osd tier set-overlay cold-storage hot-storage
```

## CONFIGURING A CACHE TIER

Cache tiers有几个配置选项。您可以使用以下方法来设置cache tier配置选项：

```
ceph osd pool set {cachepool} {key} {value}
```

有关详细信息，请参阅[Pools - Set Pool Values](http://docs.ceph.com/docs/master/rados/operations/pools/#set-pool-values)。

### TARGET SIZE AND TYPE

Ceph的cache tiers对hit_set_type使用[Bloom Filter](https://en.wikipedia.org/wiki/Bloom_filter)：

```
ceph osd pool set {cachepool} hit_set_type bloom
```

例如：

```
ceph osd pool set hot-storage hit_set_type bloom
```

hit_set_count和hit_set_period限定要存储多少个这样的HitSet，以及每个HitSet应该cover多长时间。

```
ceph osd pool set {cachepool} hit_set_count 12
ceph osd pool set {cachepool} hit_set_period 14400
ceph osd pool set {cachepool} target_max_bytes 1000000000000
```

```
注意：更大的hit_set_count会导致ceph-osd进程消耗更多的内存。
```

分级访问，随着时间的推移，可以让Ceph确定是否一个Ceph client访问某个object至少一次，或者在一段时间内（“age” vs “temperature”）超过一次。

min_read_recency_for_promote决定在处理读取操作时，有多少HitSet检查object的存在。检查结果用于决定是否异步提升object。它的值应该在0到hit_set_count之间。如果它设置为0，则该object总是被提升。如果它设置为1，则检查当前的HitSet。如果这个object在当前的HitSet中，它会被提升。否则不是。对于其他值，将检查存档HitSet的确切数量。如果在最近的min_read_recency_for_promote HitSets中找到该object，则会提升该object。

可以为写入操作设置一个类似的参数，即min_write_recency_for_promote。

```
ceph osd pool set {cachepool} min_read_recency_for_promote 2
ceph osd pool set {cachepool} min_write_recency_for_promote 2
```

```
注意：时间越长，min_read_recency_for_promote和min_write_recency_for_promote`value值越高，ceph-osd daemon消耗的内存就越多。特别是，当agent处于active状态刷新或逐出缓存objects时，所有hit_set_count HitSets都将加载到内存中。
```

### CACHE SIZING

cache tiering agent执行两个主要功能：

- Flushing: 

agent识别修改的（或脏的）objects，并将它们转发到storage pool以进行长期存储。

- Evicting: 

agent标识未被修改（或干净的）的objects，并从缓存中逐出其中不常用的objects。

### ABSOLUTE SIZING

cache tiering agent可以基于字节总数或objects总数来flush或者evict objects。要指定最大字节数，请执行以下操作：

```
ceph osd pool set {cachepool} target_max_bytes {#bytes}
```

例如，以1TB flush或evict，请执行以下操作：

```
ceph osd pool set hot-storage target_max_bytes 1099511627776
```

要指定objects的最大数量，请执行以下操作：

```
ceph osd pool set {cachepool} target_max_objects {#objects}
```

例如，要flush或evict 1M个对象，请执行以下操作：

```
ceph osd pool set hot-storage target_max_objects 1000000
```

```
注意：Ceph无法自动确定cache pool的大小，因此在此需要配置一个绝对值大小，否则flush/evict将不起作用。 如果您指定了这两个限制，则在触发阈值时，cache tiering agent将开始flushing或evicting。
```

```
注意：只有在达到target_max_bytes或target_max_objects时，所有client请求才会被blocked
```

### RELATIVE SIZING

cache tiering agent可以根据cache pool大小flush或evict objects（由target_max_bytes / target_max_objects指定绝对大小）。当cache pool由（有）一定比例修改过的（或脏）objects组成时，cache tiering agent将其flush到storage pool。要设置cache_target_dirty_ratio，请执行以下操作：

```
ceph osd pool set {cachepool} cache_target_dirty_ratio {0.0..1.0}
```

例如，将值设置为0.4将在修改过的（脏的）objects达到cache pool的容量40％时开始flushing：

```
ceph osd pool set hot-storage cache_target_dirty_ratio 0.4
```

当脏objects达到其容量的一定比例时，以较高的速度flush脏objects。要设置cache_target_dirty_high_ratio：

```
ceph osd pool set {cachepool} cache_target_dirty_high_ratio {0.0..1.0}
```

例如，将值设置为0.6将在脏objects达到cache pool容量的60％时开始大量flush脏objects。显然，我们最好设置dirty_ratio和full_ratio之间的值：

```
ceph osd pool set hot-storage cache_target_dirty_high_ratio 0.6
```

当cache pool达到其容量的一定比例时，cache tiering agent将驱逐objects以保持可用容量。要设置cache_target_full_ratio，请执行以下操作：

```
ceph osd pool set {cachepool} cache_target_full_ratio {0.0..1.0}
```

例如，将值设置为0.8，当它们达到cache pool容量的80％时，开始flushing未修改的（干净）objects：

```
ceph osd pool set hot-storage cache_target_full_ratio 0.8
```

### CACHE AGE

您可以在cache tiering agent将最近修改的（或脏的）object刷新到backing storage pool之前指定object的最小age：

```
ceph osd pool set {cachepool} cache_min_flush_age {#seconds}
```

例如，要在10分钟后flush修改的（或脏的）objects，请执行以下操作：

```
ceph osd pool set hot-storage cache_min_flush_age 600
```

您可以指定object在缓存层被evict之前的最小age：

```
ceph osd pool {cache-tier} cache_min_evict_age {#seconds}
```

例如，要在30分钟后evict object，请执行以下操作：

```
ceph osd pool set hot-storage cache_min_evict_age 1800
```

## REMOVING A CACHE TIER

删除cache tier取决于它是writeback cache还是read-only cache。

### REMOVING A READ-ONLY CACHE

由于read-only cache中没有修改过的（脏的）数据，因此可以直接禁用并删除它，在缓存中objects的最新更改不会丢失。

1、将cache-mode更改为none以禁用它。

```
ceph osd tier cache-mode {cachepool} none
```

例如：

```
ceph osd tier cache-mode hot-storage none
```

2、从backing pool上移除cache pool。

```
ceph osd tier remove {storagepool} {cachepool}
```

例如：

```
ceph osd tier remove cold-storage hot-storage
```

### REMOVING A WRITEBACK CACHE

由于writeback cache中可能修改了数据，因此必须采取措施，确保在禁用和删除缓存中的objects之前不会丢失任何最新更改。

1、将cache mode更改为forward，以便最新的和修改的objects将flush到backing storage pool。

 ```
ceph osd tier cache-mode {cachepool} forward
 ```

例如：

```
ceph osd tier cache-mode hot-storage forward
```

2、确保cache pool已被flush。 这可能需要几分钟的时间：

```
rados -p {cachepool} ls
```

如果cache pool仍有objects，则可以手动flush它们。 例如：

```
rados -p {cachepool} cache-flush-evict-all
```

3、删除overlay，以便client不会将流量导向缓存。

```
ceph osd tier remove-overlay {storagetier}
```

例如：

```
ceph osd tier remove-overlay cold-storage
```

4、最后，从backing storage pool上移除cache tier pool。

```
ceph osd tier remove {storagepool} {cachepool}
```

例如：

```
ceph osd tier remove cold-storage hot-storage
```
