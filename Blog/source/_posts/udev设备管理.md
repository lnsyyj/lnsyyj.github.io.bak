---
title: udev设备管理
date: 2019-10-29 15:19:32
tags: udev
---



Linuxcast学习笔记，视频地址：https://www.youtube.com/watch?v=eV4InZop--0

# udev是什么

udev是动态管理设备的机制（/dev/目录下的设备）。udev允许我们自己写一些rule配置文件来控制udev默认的行为动作。默认配置文件在/etc/udev/目录下，在/etc/udev/rules.d/下为默认rule。如70-persistent-net.rules文件，udev在工作时，每次检查/etc/udev/rules.d/目录下的配置文件，并且按照数字的顺序来加载并应用这些配置。

udev允许我们在一个设备连接到计算机的时候，或者已经连接上，或者卸载的时候执行一些特殊的动作。（设备连接时、设备连接上、设备断开时）

# 如何使用udev修改设备默认名称

如果我们想修改一个设备，那么我们需要唯一的定位一个设备，通过udevadm命令。通常serial（串号）

```
[root@ceph2 ~]# udevadm info -a -n /dev/vdc

Udevadm info starts with the device specified by the devpath and then
walks up the chain of parent devices. It prints for every device
found, all possible attributes in the udev rules key format.
A rule to match, can be composed by the attributes of the device
and the attributes from one single parent device.

  looking at device '/devices/pci0000:00/0000:00:09.0/virtio4/block/vdc':
    KERNEL=="vdc"
    SUBSYSTEM=="block"
    DRIVER==""
    ATTR{ro}=="0"
    ATTR{size}=="104857600"
    ATTR{stat}=="    7223       11   474136    19244   400088    30660  1866945   871549        0   840441   872404"
    ATTR{cache_type}=="write back"
    ATTR{range}=="16"
    ATTR{discard_alignment}=="0"
    ATTR{ext_range}=="256"
    ATTR{serial}=="e850ae75-fcb2-4432-a"
    ATTR{alignment_offset}=="0"
    ATTR{inflight}=="       0        0"
    ATTR{removable}=="0"
    ATTR{capability}=="50"

  looking at parent device '/devices/pci0000:00/0000:00:09.0/virtio4':
    KERNELS=="virtio4"
    SUBSYSTEMS=="virtio"
    DRIVERS=="virtio_blk"
    ATTRS{device}=="0x0002"
    ATTRS{features}=="0010101001110000000000000000110010000000000000000000000000000000"
    ATTRS{status}=="0x0000000f"
    ATTRS{vendor}=="0x1af4"

  looking at parent device '/devices/pci0000:00/0000:00:09.0':
    KERNELS=="0000:00:09.0"
    SUBSYSTEMS=="pci"
    DRIVERS=="virtio-pci"
    ATTRS{irq}=="10"
    ATTRS{subsystem_vendor}=="0x1af4"
    ATTRS{broken_parity_status}=="0"
    ATTRS{class}=="0x010000"
    ATTRS{driver_override}=="(null)"
    ATTRS{consistent_dma_mask_bits}=="64"
    ATTRS{dma_mask_bits}=="64"
    ATTRS{local_cpus}=="f"
    ATTRS{device}=="0x1001"
    ATTRS{enable}=="1"
    ATTRS{msi_bus}==""
    ATTRS{local_cpulist}=="0-3"
    ATTRS{vendor}=="0x1af4"
    ATTRS{subsystem_device}=="0x0002"
    ATTRS{numa_node}=="-1"
    ATTRS{d3cold_allowed}=="0"

  looking at parent device '/devices/pci0000:00':
    KERNELS=="pci0000:00"
    SUBSYSTEMS==""
    DRIVERS==""
```

 在/etc/udev/rules.d/下创建rule，99-linuxcast.rules，编辑规则。

```
KERNEL=="sd*", ATTR{serial}=="e850ae75-fcb2-4432-a", NAME="yujiangvdc%n"
```

KERNEL意思是内核识别出来这个设备是什么名字，两个等号==是做比较，一个等号=是赋值。

通过KERNEL与ATTR{serial}就可以唯一定位到一个设备，要把这个设备修改为其他名字用NAME，udev会自动把%n替换成分区号。