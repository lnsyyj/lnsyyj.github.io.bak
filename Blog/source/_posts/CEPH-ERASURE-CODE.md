---
title: CEPH ERASURE CODE
date: 2018-05-12 12:31:05
tags: CEPH-Luminous
---

为了承受OSD丢失，一个Ceph pool与一个类型相关联（大多数情况下每个磁盘有一个OSD）。当创建pool时，默认选择replicate，这意味着每个object都被复制到多个磁盘上。可以使用Erasure Code pool类型替代replicate，以节省空间。

## CREATING A SAMPLE ERASURE CODED POOL

最简单的erasure coded pool相当于RAID5，并且至少需要三台主机：

```shell
$ ceph osd pool create ecpool 12 12 erasure
pool 'ecpool' created
$ echo ABCDEFGHI | rados --pool ecpool put NYAN -
$ rados --pool ecpool get NYAN -
ABCDEFGHI
```

请注意，pool 中的12表示placement groups的数量。

## ERASURE CODE PROFILES

默认的erasure code profile会承受丢失1个OSD。它相当于一个大小为2的replicated pool，但需要1.5TB而不是2TB来存储1TB的数据。默认profile显示：

```shell
$ ceph osd erasure-code-profile get default
k=2
m=1
plugin=jerasure
crush-failure-domain=host
technique=reed_sol_van
```

选择正确的profile非常重要，因为在创建pool后无法对其进行修改：需要创建具有不同profile的新pool，并将先前pool中的所有objects都移动到新的pool中。

profile最重要的参数是K，M和crush-failure-domain，因为它们定义了存储开销和数据持久性。例如，期望在系统架构中必须承受两个racks间的损失，并且控制额外增加40％存储开销，则可以定义以下profile：

```shell
$ ceph osd erasure-code-profile set myprofile \
   k=3 \
   m=2 \
   crush-failure-domain=rack
$ ceph osd pool create ecpool 12 12 erasure myprofile
$ echo ABCDEFGHI | rados --pool ecpool put NYAN -
$ rados --pool ecpool get NYAN -
ABCDEFGHI
```

NYAN object将被分成三部分（K = 3），并创建两个附加chunks（M = 2）。M的值定义了可以同时丢失多少OSD而不损失任何数据。crush-failure-domain = rack将创建一个CRUSH rule，以确保没有两个chunks存储在同一个rack中。

![](http://docs.ceph.com/docs/master/_images/ditaa-96fe8c3c73e5e54cf27fa8a4d64ed08d17679ba3.png)

更多信息可以在[erasure code profiles documentation](http://docs.ceph.com/docs/master/rados/operations/erasure-code-profile/)中找到。

## ERASURE CODING WITH OVERWRITES

默认情况下，erasure coded pools仅适用于像RGW那样，完整object writes和appends的场景。Luminous版本，可以为每个pool设置启用erasure coded pool的partial writes。 这让RBD和CephFS可以将他们的数据存储在erasure coded pool中：

```shell
ceph osd pool set ec_pool allow_ec_overwrites true
```

这只能对bluestore OSD上的pool启用，因为bluestore的checksumming用于在deep-scrub过程中检查bitrot或其他corruption。除了不安全之外，使用filestore进行ec overwrites，相对于bluestore性能低。

Erasure coded pools不支持omap（ObjectMap），因此要将它们与RBD和CephFS一起使用，必须明确指示它们将data存储在ec pool中，将metadata存储在replicated pool中。对于RBD，这意味着在创建image时，需要使用--data-pool指定erasure coded pool：

```shell
rbd create --size 1G --data-pool ec_pool replicated_pool/image_name
```

对于CephFS，在创建file system时或者通过[file layouts](http://docs.ceph.com/docs/master/cephfs/file-layouts/)设定一个erasure coded pool作为默认的data pool。

## ERASURE CODED POOL AND CACHE TIERING

Erasure coded pools需要比replicated pools更多的资源，并且缺少一些功能，如omap。为了克服这些限制，可以在erasure coded pool之前设置一个[cache tier](http://docs.ceph.com/docs/master/rados/operations/cache-tiering/)。

例如，hot-storage pool由fast storage设备组成：

```shell
$ ceph osd tier add ecpool hot-storage
$ ceph osd tier cache-mode hot-storage writeback
$ ceph osd tier set-overlay ecpool hot-storage
```

hot-storage pool作为ecpool的tier，将数据以writeback模式回写至ecpool，所以每次write和read这个ecpool的时候，实际使用的是hot-storage。

更多信息可以在[cache tiering documentation](http://docs.ceph.com/docs/master/rados/operations/cache-tiering/)中找到。

## GLOSSARY

chunk

当encoding function被调用时，它返回相同大小的chunks。Data chunks可以连接起来重建original object和coding chunks，这些chunks可以用来重建丢失的chunk。

K

data chunks的数量，即original object被划分成chunks的数量。例如，如果K = 2，10KB objects分为每个5KB的K个objects。

M

coding chunks的数量，即由encoding functions计算的附加chunks的数量。如果有2个coding chunks，这意味着2个OSD可以在不丢失数据的情况下out。

## TABLE OF CONTENT

[Erasure code profiles](http://docs.ceph.com/docs/master/rados/operations/erasure-code-profile/)
[Jerasure erasure code plugin](http://docs.ceph.com/docs/master/rados/operations/erasure-code-jerasure/)
[ISA erasure code plugin](http://docs.ceph.com/docs/master/rados/operations/erasure-code-isa/)
[Locally repairable erasure code plugin](http://docs.ceph.com/docs/master/rados/operations/erasure-code-lrc/)
[SHEC erasure code plugin](http://docs.ceph.com/docs/master/rados/operations/erasure-code-shec/)



##  附录

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/source/_posts/CEPH-ERASURE-CODE/ec.png)



ObjectMap参考链接：

【1】http://bean-li.github.io/ceph-omap

【2】https://my.oschina.net/u/2460844/blog/604530

【3】http://www.wzxue.com/ceph-filestore

ceph中所有块设备、对象存储、文件存储最后都要转化为对象（object），这个object包含3个元素：data、xattr、omap。

- data是保存对象的数据。
- xattr是保存对象的扩展属性，每个对象文件都可以设置文件的属性，这个属性是一个key/value值对，但是受到文件系统的限制，key/value对的个数和每个value的大小都进行了限制。
- 如果要设置的对象的key/value不能存储在文件的扩展属性中，还存在另外一种方式保存omap，omap实际上是保存到了key/vaule  值对的数据库levelDB（L版是RocksDB）中，在这里value的值限制要比xattr中好的多。