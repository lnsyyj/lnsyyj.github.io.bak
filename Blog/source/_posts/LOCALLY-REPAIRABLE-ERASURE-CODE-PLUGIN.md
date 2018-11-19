---
title: LOCALLY REPAIRABLE ERASURE CODE PLUGIN
date: 2018-05-17 18:48:13
tags: CEPH-Luminous
---

通过jerasure plugin，当erasure coded的object存储在多个OSD中时，恢复丢失的1个OSD数据需要从所有其他OSD读取数据并修复。例如，如果jerasure配置为k = 8和m = 4，则丢失1个OSD需要从其他11个OSD读取并修复。

lrc erasure code plugin创建本地奇偶校验chunks，以便能够使用较少的OSD恢复。例如，如果lrc配置为k = 8，m = 4和l = 4，则会为每4个OSD创建一个附加奇偶校验chunk。当1个OSD丢失时，只需要使用4个OSD而不是11个来恢复。

## ERASURE CODE PROFILE EXAMPLES

### REDUCE RECOVERY BANDWIDTH BETWEEN HOSTS

当所有主机连接到相同的交换机时，实际上可以观察到减少的带宽。

````
$ ceph osd erasure-code-profile set LRCprofile \
     plugin=lrc \
     k=4 m=2 l=3 \
     crush-failure-domain=host
$ ceph osd pool create lrcpool 12 12 erasure LRCprofile
````

### REDUCE RECOVERY BANDWIDTH BETWEEN RACKS

在Firefly中，只有当primary OSD与丢失chunk在同一个rack中时，才会观察到减少的带宽。

````
$ ceph osd erasure-code-profile set LRCprofile \
     plugin=lrc \
     k=4 m=2 l=3 \
     crush-locality=rack \
     crush-failure-domain=host
$ ceph osd pool create lrcpool 12 12 erasure LRCprofile
````

## CREATE AN LRC PROFILE

创建新的lrc erasure code profile：

````
ceph osd erasure-code-profile set {name} \
     plugin=lrc \
     k={data-chunks} \
     m={coding-chunks} \
     l={locality} \
     [crush-root={root}] \
     [crush-locality={bucket-type}] \
     [crush-failure-domain={bucket-type}] \
     [crush-device-class={device-class}] \
     [directory={directory}] \
     [--force]
````

其中：

k={data chunks}

````
Description:	每个object都分为多个data-chunks parts，每个part存储在不同的OSD上。
Type:	Integer
Required:	Yes.
Example:	4
````

m={coding-chunks}

````
Description:	计算每个object的coding chunks并将它们存储在不同的OSD上。coding chunks的数量也是在不丢失数据的情况下，允许损失OSD的数量。
Type:	Integer
Required:	Yes.
Example:	2
````

l={locality}

````
Description:	将coding和data chunks分组为指定size locality的sets。例如，对于k = 4和m = 2，当locality = 3时，创建2个groups，每个group中3个。每个set都可以在不读取另一set中chunks的情况下恢复。

Group the coding and data chunks into sets of size locality. For instance, for k=4 and m=2, when locality=3 two groups of three are created. Each set can be recovered without reading chunks from another set.
Type:	Integer
Required:	Yes.
Example:	3
````

crush-root={root}

````
Description:	crush bucket名字用于CRUSH rule的first step。"take"为step的默认值。
Type:	String
Required:	No.
Default:	default
````

crush-locality={bucket-type}

````
Description:	crush bucket的type，其中由l定义的每个chunks集合将被存储。例如，如果它被设置为rack，则每个l chunks将被放置在不同的rack中。它用于创建CRUSH rule step，如step选择rack。如果没有设置，则不会进行这样的分组。
The type of the crush bucket in which each set of chunks defined by l will be stored. For instance, if it is set to rack, each group of l chunks will be placed in a different rack. It is used to create a CRUSH rule step such as step choose rack. If it is not set, no such grouping is done.
Type:	String
Required:	No.
````

crush-failure-domain={bucket-type}

````
Description:	确保不要有两个chunks位于相同故障域的bucket中。例如，如果故障域是host，则不会在同一主机上存储两个chunks。它用于创建CRUSH rule step，比如step chooseleaf host。

Ensure that no two chunks are in a bucket with the same failure domain. For instance, if the failure domain is host no two chunks will be stored on the same host. It is used to create a CRUSH rule step such as step chooseleaf host.
Type:	String
Required:	No.
Default:	host
````

crush-device-class={device-class}

````
Description:	使用CRUSH map中的crush device class名称，限定存储到特定class devices（例如SSD或HDD）。
Type:	String
Required:	No.
Default:	
````

directory={directory}

````
Description:	设置加载erasure code plugin的目录名。
Type:	String
Required:	No.
Default:	/usr/lib/ceph/erasure-code
````

--force

````
Description:	用相同的名称覆盖已有的profile。
Type:	String
Required:	No.
````

## LOW LEVEL PLUGIN CONFIGURATION

k和m的和必须是l parameter的倍数。low level configuration parameters不会施加这样的限制，并且为了特定目的而使用它可能更加方便。例如可以定义两个groups，一个group有4个chunks，另一个group有3个chunks。还可以递归地定义locality sets，例如datacenters和racks到datacenters。通过生成low level configuration来实现k/m/l。

lrc erasure code plugin递归地应用erasure code技术，以便在大多数情况下从某些chunks的丢失中仅恢复需要的可用chunks的子集。例如，three coding steps被描述为：

````
chunk nr    01234567
step 1      _cDD_cDD
step 2      cDDD____
step 3      ____cDDD
````

其中c是从data chunks D计算出的coding chunks，丢失的chunk 7，可以用后4个chunks恢复。丢失的chunk 2的chunk可以用前4个chunks恢复。

## ERASURE CODE PROFILE EXAMPLES USING LOW LEVEL CONFIGURATION

### MINIMAL TESTING

它严格等同于使用默认的erasure code profile。DD意味着K = 2，c意味着M = 1，并且使用默认的jerasure  plugin。

````
$ ceph osd erasure-code-profile set LRCprofile \
     plugin=lrc \
     mapping=DD_ \
     layers='[ [ "DDc", "" ] ]'
$ ceph osd pool create lrcpool 12 12 erasure LRCprofile
````

### REDUCE RECOVERY BANDWIDTH BETWEEN HOSTS

当所有主机连接到同一个交换机时，可以观察到减少的带宽使用情况。它相当于k = 4，m = 2和l = 3，尽管chunks的layout不同：

````
$ ceph osd erasure-code-profile set LRCprofile \
     plugin=lrc \
     mapping=__DD__DD \
     layers='[
               [ "_cDD_cDD", "" ],
               [ "cDDD____", "" ],
               [ "____cDDD", "" ],
             ]'
$ ceph osd pool create lrcpool 12 12 erasure LRCprofile
````

### REDUCE RECOVERY BANDWIDTH BETWEEN RACKS

在Firefly中，只有当primary OSD与丢失chunk在同一个rack中时，才会观察到减少的带宽。

````
$ ceph osd erasure-code-profile set LRCprofile \
     plugin=lrc \
     mapping=__DD__DD \
     layers='[
               [ "_cDD_cDD", "" ],
               [ "cDDD____", "" ],
               [ "____cDDD", "" ],
             ]' \
     crush-steps='[
                     [ "choose", "rack", 2 ],
                     [ "chooseleaf", "host", 4 ],
                    ]'
$ ceph osd pool create lrcpool 12 12 erasure LRCprofile
````

### TESTING WITH DIFFERENT ERASURE CODE BACKENDS

LRC现在使用jerasure作为默认的EC backend。可以使用low level configuration在每个layer基础上指定EC backend/algorithm。layers=’[ [ “DDc”, “” ] ]’ 的第二个参数实际上是用于此level的erasure code profile。下面的例子指定了在lrcpool中使用cauchy technique的ISA backend。

````
$ ceph osd erasure-code-profile set LRCprofile \
     plugin=lrc \
     mapping=DD_ \
     layers='[ [ "DDc", "plugin=isa technique=cauchy" ] ]'
$ ceph osd pool create lrcpool 12 12 erasure LRCprofile
````

您还可以为每个layer使用不同的erasure code profile。

````
$ ceph osd erasure-code-profile set LRCprofile \
     plugin=lrc \
     mapping=__DD__DD \
     layers='[
               [ "_cDD_cDD", "plugin=isa technique=cauchy" ],
               [ "cDDD____", "plugin=isa" ],
               [ "____cDDD", "plugin=jerasure" ],
             ]'
$ ceph osd pool create lrcpool 12 12 erasure LRCprofile
````

## ERASURE CODING AND DECODING ALGORITHM

在layers中找到的steps描述：

````
chunk nr    01234567

step 1      _cDD_cDD
step 2      cDDD____
step 3      ____cDDD
````

按顺序应用。例如，如果一个4K的object被encoded，它将首先通过step 1并被分成4个1K的chunk（4个大写D）。它们按顺序存储在chunk 2,3,6和7中。由此计算出两个coding chunks（两个小写字母c）。coding chunks分别存储在chunk 1和5中。

step 2以类似的方式重新使用由step 1创建的内容，并在位置0处存储单个coding chunk c。为了便于阅读，最后四个标记有下划线（_）的chunks将被忽略。

step 3在位置4存储单个coding chunk c。由step 1创建的3个chunks被用于计算该coding chunk，即来自step 1的coding chunk在step 3中变为data chunk。

如果chunk 2丢失：

````
chunk nr    01234567

step 1      _c D_cDD
step 2      cD D____
step 3      __ _cDDD
````

decoding将尝试通过以相反的顺序来执行steps恢复它：step 3然后是step 2，最后是step 1。

step 3对chunk 2（即它是下划线）一无所知并被跳过。

存储在chunk 0中来自step 2的coding chunk允许其恢复chunk 2的内容。没有更多的chunks要恢复，过程停止，不考虑step 1。

恢复chunk 2需要读取chunks 0,1,3，并写回chunk 2。

如果chunk 2,3,6丢失：

````
chunk nr    01234567

step 1      _c  _c D
step 2      cD  __ _
step 3      __  cD D
````

step 3可以恢复chunk 6的内容：

````
chunk nr    01234567

step 1      _c  _cDD
step 2      cD  ____
step 3      __  cDDD
````

step 2无法恢复并被跳过，因为缺少两个chunks（2,3），并且它只能从一个缺失的chunk中恢复。

存储在chunk 1,5中的来自step 1的coding chunk允许它恢复chunk 2,3的内容：

````
chunk nr    01234567

step 1      _cDD_cDD
step 2      cDDD____
step 3      ____cDDD
````

## CONTROLLING CRUSH PLACEMENT

默认的CRUSH rule提供位于不同hosts上隔离OSD。 例如：

````
chunk nr    01234567

step 1      _cDD_cDD
step 2      cDDD____
step 3      ____cDDD
````

需要8个OSD，每个chunk需要一个OSD。如果hosts位于两个相邻的racks中，则前4个chunks可以放在第一个rack中，最后4个放在第二个rack中。因此，从丢失的单个OSD中恢复并不需要在两个racks之间使用带宽。

例如：

````
crush-steps='[ [ "choose", "rack", 2 ], [ "chooseleaf", "host", 4 ] ]'
````

将创建一个rule，将选择两个类型rack的crush buckets，为它们每个选择4个OSD，它们中的每一个都位于类型host的不同buckets中。

CRUSH rule也可以手动制作以实现更精细的控制。

