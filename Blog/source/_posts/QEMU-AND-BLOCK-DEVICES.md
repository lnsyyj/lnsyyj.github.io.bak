---
title: QEMU AND BLOCK DEVICES
date: 2019-11-15 10:42:46
tags: ceph
---

最常见的Ceph Block Device用例是向虚拟机提供block device images。例如，用户可以创建"golden" image。 然后，对image做snapshot。最后，用户clone snapshot（通常多次）。 有关详细信息，请参见 [Snapshots](https://docs.ceph.com/docs/hammer/rbd/rbd-snapshot/)。snapshot的clone具有copy-on-write能力，意味着Ceph可以快速将block device images配置给虚拟机，因为客户端不必在每次启动新虚拟机时都下载整个映像。

![](https://docs.ceph.com/docs/hammer/_images/ditaa-4733472b605d45db3caa492c9fa5900204396a2b.png)

Ceph Block Devices可以与QEMU虚拟机集成。 有关QEMU的详细信息，请参阅[QEMU Open Source Processor Emulator](http://wiki.qemu.org/Main_Page)。 有关QEMU文档，请参见[QEMU Manual](http://wiki.qemu.org/Manual)。 有关安装的详细信息，请参见 [Installation](https://docs.ceph.com/docs/hammer/install)。

```
重要说明要将Ceph Block Devices与QEMU一起使用，你必须能够访问正在运行的Ceph的集群。
```

# USAGE（用法）

QEMU命令行要求您指定pool名称和image名称。 您也可以指定snapshot名称。

QEMU将假设Ceph configuration文件位于默认位置（例如，`/etc/ceph/$cluster.conf`），并且您以默认的`client.admin` user身份执行命令，除非您明确指定另一个Ceph configuration文件路径或另一个user。指定user时，QEMU使用`ID`而不是完整的`TYPE:ID`。有关详细信息，请参见[User Management - User](https://docs.ceph.com/docs/hammer/rados/operations/user-management#user)。请勿在user ID的开头添加客户端类型（即，`client`），否则您将收到验证错误。您应该具有admin user的key，或者使用`:id={user}`选项指定的另一个user的key，存储在默认路径中的keyring文件中。（即，/etc/ceph或具有适当文件所有权和权限的本地目录）用法如下：

```
qemu-img {command} [options] rbd:{pool-name}/{image-name}[@snapshot-name][:option1=value1][:option2=value2...]
```

例如，指定id和conf选项如下所示：

```
qemu-img {command} [options] rbd:glance-pool/maipo:id=glance:conf=/etc/ceph/ceph.conf
```

提示：包含`:`, `@`, 或 `=`可以使用\开头的字符进行转义。

# CREATING IMAGES WITH QEMU（使用QEMU创建IMAGES）

可以从QEMU创建block device image。 必须指定rbd，pool名称和要创建的image名称。 还必须指定image的size。

```
qemu-img create -f raw rbd:{pool-name}/{image-name} {size}
```

例如：

```
qemu-img create -f raw rbd:data/foo 10G
```

重要说明：raw data format（原始数据格式）实际上是与RBD一起使用的唯一明智格式选项。从技术上讲，您可以使用其他QEMU支持的格式（例如qcow2或vmdk），但是这样做会增加额外的开销，并且在启用缓存（请参阅下文）时，对于虚拟机实时迁移也将使该卷不安全。

# RESIZING IMAGES WITH QEMU（使用QEMU调整IMAGES大小）

您可以从QEMU调整block device image的大小。 您必须指定rbd，pool名称以及要调整大小的image名称。 您还必须指定image的size。

```
qemu-img resize rbd:{pool-name}/{image-name} {size}
```

例如：

```
qemu-img resize rbd:data/foo 10G
```

# RETRIEVING IMAGE INFO WITH QEMU（使用QEMU检索IMAGE信息）

您可以从QEMU中检索block device image信息。 您必须指定rbd，pool名称和image名称。

```
qemu-img info rbd:{pool-name}/{image-name}
```

例如：

```
qemu-img info rbd:data/foo
```

# RUNNING QEMU WITH RBD（运行QEMU与RBD）

QEMU可以将block device从host传递到guest，但是从QEMU 0.15开始，无需将image映射为host上的block device。相反，QEMU可以直接通过librbd作为virtual block device访问image。这样会更好，因为它避免了额外的context（上下文）切换，并且可以利用[RBD caching](https://docs.ceph.com/docs/hammer/rbd/rbd-config-ref/#rbd-cache-config-settings)。

您可以使用qemu-img将现有的virtual machine images转换为Ceph block device images。例如，如果您有一个qcow2 image，则可以运行：

```
qemu-img convert -f qcow2 -O raw debian_squeeze.qcow2 rbd:data/squeeze
```

要运行从该image启动的virtual machine，可以运行：

```
qemu -m 1024 -drive format=raw,file=rbd:data/squeeze
```

[RBD caching](https://docs.ceph.com/docs/hammer/rbd/rbd-config-ref/#rbd-cache-config-settings)可以明显提高性能。 从QEMU 1.2开始，QEMU的cache选项控制`librbd` caching：

```
qemu -m 1024 -drive format=rbd,file=rbd:data/squeeze,cache=writeback
```

如果您使用的是较旧版本的QEMU，则可以将librbd cache configuration设置为"file"参数的一部分（如任何Ceph配置选项一样）：

```
qemu -m 1024 -drive format=raw,file=rbd:data/squeeze:rbd_cache=true,cache=writeback
```

重要说明：如果设置rbd_cache=true，则必须设置cache=writeback。如果不使用cache=writeback，则QEMU不会将刷新请求发送到librbd，如果QEMU在此配置中异常退出，则 rbd 顶部的文件系统可能会损坏。

# ENABLING DISCARD/TRIM（启用DISCARD/TRIM）

从Ceph 0.46版和QEMU 1.1版开始，Ceph Block Devices支持discard（丢弃）操作。这意味着guest可以发送TRIM请求，让Ceph block device回收未使用的空间。在客户机中mount `ext4`或`XFS`时，通过`discard`选项启用它。

为了使此功能可供guest使用，必须为block device显式启用它。 为此，必须指定与drive关联的`dispatch_granularity`：

```
qemu -m 1024 -drive format=raw,file=rbd:data/squeeze,id=drive1,if=none \
     -device driver=ide-hd,drive=drive1,discard_granularity=512
```

请注意，这使用了IDE driver。 virtio driver不支持discard。

如果使用libvirt，请使用`virsh edit`编辑domain的配置文件，以包含`xmlns:qemu`值。然后，添加`qemu:commandline` block作为该domain的子级。以下示例为如何将`qemu id=`设置到两个不同的devices，并且discard_granularity值不同。

```
<domain type='kvm' xmlns:qemu='http://libvirt.org/schemas/domain/qemu/1.0'>
        <qemu:commandline>
                <qemu:arg value='-set'/>
                <qemu:arg value='block.scsi0-0-0.discard_granularity=4096'/>
                <qemu:arg value='-set'/>
                <qemu:arg value='block.scsi0-0-1.discard_granularity=65536'/>
        </qemu:commandline>
</domain>
```

# QEMU CACHE OPTIONS（QEMU缓存选项）

QEMU的cache选项与以下Ceph [RBD Cache](https://docs.ceph.com/docs/master/rbd/rbd-config-ref/)设置相对应。

Writeback:

```
rbd_cache = true
```

Writethrough:

```
rbd_cache = true
rbd_cache_max_dirty = 0
```

None:

```
rbd_cache = false
```

QEMU的cache设置会覆盖Ceph的cache设置（包括在Ceph配置文件中设置）。

# 实验



```
[root@ceph ~]# qemu-img create -f raw rbd:rbd/yujiang 10G
Formatting 'rbd:rbd/yujiang', fmt=raw size=10737418240 cluster_size=0 
[root@ceph ~]# rbd info yujiang
rbd image 'yujiang':
	size 10GiB in 2560 objects
	order 22 (4MiB objects)
	block_name_prefix: rbd_data.25dbd6b8b4567
	format: 2
	features: layering, exclusive-lock, object-map, fast-diff, deep-flatten
	flags: 
	create_timestamp: Fri Nov 15 15:15:20 2019

[root@ceph ~]# qemu-img resize rbd:rbd/yujiang 20G
Image resized.
[root@ceph ~]# rbd info yujiang
rbd image 'yujiang':
	size 20GiB in 5120 objects
	order 22 (4MiB objects)
	block_name_prefix: rbd_data.25dbd6b8b4567
	format: 2
	features: layering, exclusive-lock, object-map, fast-diff, deep-flatten
	flags: 
	create_timestamp: Fri Nov 15 15:15:20 2019

[root@ceph ~]# qemu-img info rbd:rbd/yujiang
image: rbd:rbd/yujiang
file format: raw
virtual size: 20G (21474836480 bytes)
disk size: unavailable

```

