---
title: ERASURE CODE PROFILES
date: 2018-05-15 21:41:29
tags: CEPH-Luminous
---

Erasure code由profile定义，并在erasure coded pool关联CRUSH rule时使用。

默认的erasure code profile（在Ceph集群初始化时被创建）提供了与两个副本相同的冗余级别，但相比两副本需要的磁盘空间减少了25％。它被描述为k = 2和m = 1的profile，这意味着信息分布在三个OSD（k + m == 3）上，其中一份可以被丢失。

为了在不增加raw space的情况下提高冗余度，可以创建新的profile。例如，k = 10和m = 4的profile可以通过在14（k + m = 14）个OSD上分散一个object来支撑4个（m = 4）OSD的丢失。首先将object分成10个chunks（如果object为10MB，每个chunk为1MB）并且计算4个coding chunks用于recovery（每个coding chunk具有与data chunk相同的大小，即1MB）。raw space开销只有40％，即使四个OSD同时中断，object也不会丢失。

- [Jerasure erasure code plugin](http://docs.ceph.com/docs/master/rados/operations/erasure-code-jerasure/)
- [ISA erasure code plugin](http://docs.ceph.com/docs/master/rados/operations/erasure-code-isa/)
- [Locally repairable erasure code plugin](http://docs.ceph.com/docs/master/rados/operations/erasure-code-lrc/)
- [SHEC erasure code plugin](http://docs.ceph.com/docs/master/rados/operations/erasure-code-shec/)

## OSD ERASURE-CODE-PROFILE SET

创建新的erasure code profile：

````
ceph osd erasure-code-profile set {name} \
     [{directory=directory}] \
     [{plugin=plugin}] \
     [{stripe_unit=stripe_unit}] \
     [{key=value} ...] \
     [--force]
````

其中：

{directory=directory}

````
Description:	设置加载erasure code plugin的目录名。
Type:	String
Required:	No.
Default:	/usr/lib/ceph/erasure-code
````

{plugin=plugin}

````
Description:	使用erasure code plugin计算coding chunks和recover missing chunks。
Type:	String
Required:	No.
Default:	jerasure
````

{stripe_unit=stripe_unit}

````
Description:	data chunk中每个stripe的数据量。例如，具有2个data chunks并且StruPeyUng=4K的profile将在chunk 0中放置0-4K，在chunk 1中放置4K-8K，然后再在chunk 0中放置8K-12K。4K的倍数可以获得最佳的性能。当创建pool时，默认值取自monitor config中的osd_pool_erasure_code_stripe_unit。使用此profile的pool的stripe_width将是data chunks数乘以此stripe_unit的数量。
Type:	String
Required:	No.
````

{key=value}

````
Description:	其余的key/value对semantic由erasure code plugin定义。
Type:	String
Required:	No.
````

--force

````
Description:	以相同的名称覆盖现有的profile，并允许设置非4K对齐的stripe_unit。
Type:	String
Required:	No.
````

## OSD ERASURE-CODE-PROFILE RM

删除一个erasure code profile：

````
ceph osd erasure-code-profile rm {name}
````

如果profile被某个pool引用，删除将失败。

## OSD ERASURE-CODE-PROFILE GET

显示一个erasure code profile：

````
ceph osd erasure-code-profile get {name}
````

## OSD ERASURE-CODE-PROFILE LS

列出所有erasure code profiles的名称：

````
ceph osd erasure-code-profile ls
````