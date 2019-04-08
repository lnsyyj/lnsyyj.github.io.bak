---
title: LVM逻辑卷基本概念
date: 2019-04-08 10:06:35
tags: LVM
---

LinuxCast视频教程笔记

# 传统磁盘管理的问题

当分区大小不够用时无法扩展其大小，只能通过添加硬盘、创建新的分区来扩展空间，但是新添加进来的硬盘是作为独立文件系统存在的，原有的文件系统并未得到扩充，上层应用很多时候只能访问一个文件系统。只能让现有磁盘下线，换上新的磁盘之后，再将原始数据导入。

# LVM

LVM（Logical volume Manager）逻辑卷管理通过将底层物理硬盘抽象封装起来，以逻辑卷的形式表现给上层系统，逻辑卷的大小可以动态调整，而且不会丢失现有数据。新加入的硬盘也不会改变现有上层的逻辑卷。

作为一种动态磁盘管理机制，逻辑卷技术大大提高了磁盘管理的灵活性。

![](<https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/Blog/source/_posts/LVM%E9%80%BB%E8%BE%91%E5%8D%B7%E5%9F%BA%E6%9C%AC%E6%A6%82%E5%BF%B5/LVM-1.png>)

上图黄色为：VG    橙色为：LV

（1）首先把物理硬盘格式化（存储里面叫条带化）为物理卷（PV），格式化为物理卷的过程实际上是把硬盘空间化成一个一个的PE（PE是逻辑卷空间管理的最基本单位，默认4M）

（2）第二步我们要创建一个VG，VG的作用是用来装PE的，就像一个空间池。我们可以把一个或者多个PV加到VG当中。加入多少个PV，我们的VG容量就是这些PV大小之和。（当创建VG以后，在/dev目录下会多出一个目录）

（3）最后创建LV（每个LV的空间可能来自不同的物理硬盘）

![](<https://github.com/lnsyyj/lnsyyj.github.io/blob/hexo/Blog/source/_posts/LVM%E9%80%BB%E8%BE%91%E5%8D%B7%E5%9F%BA%E6%9C%AC%E6%A6%82%E5%BF%B5/LVM-2.png?raw=true>)

# 创建LVM

![](<https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/Blog/source/_posts/LVM%E9%80%BB%E8%BE%91%E5%8D%B7%E5%9F%BA%E6%9C%AC%E6%A6%82%E5%BF%B5/LVM-3.png>)

![](<https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/Blog/source/_posts/LVM%E9%80%BB%E8%BE%91%E5%8D%B7%E5%9F%BA%E6%9C%AC%E6%A6%82%E5%BF%B5/LVM-4.png>)

![](<https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/Blog/source/_posts/LVM%E9%80%BB%E8%BE%91%E5%8D%B7%E5%9F%BA%E6%9C%AC%E6%A6%82%E5%BF%B5/LVM-5.png>)

```
[root@teuthology ~]# lsblk
NAME   MAJ:MIN RM  SIZE RO TYPE MOUNTPOINT
vda    253:0    0  200G  0 disk 
├─vda1 253:1    0    1G  0 part /boot
├─vda2 253:2    0    4G  0 part [SWAP]
└─vda3 253:3    0  195G  0 part /
vdb    253:16   0  100G  0 disk 
vdc    253:32   0  100G  0 disk

[root@teuthology ~]# pvcreate /dev/vdb /dev/vdc
  Physical volume "/dev/vdb" successfully created.
  Physical volume "/dev/vdc" successfully created.
[root@teuthology ~]# pvs
  PV         VG Fmt  Attr PSize   PFree  
  /dev/vdb      lvm2 ---  100.00g 100.00g
  /dev/vdc      lvm2 ---  100.00g 100.00g

[root@teuthology ~]# vgcreate linuxcast /dev/vdb /dev/vdc 
  Volume group "linuxcast" successfully created
[root@teuthology ~]# vgs
  VG        #PV #LV #SN Attr   VSize   VFree  
  linuxcast   2   0   0 wz--n- 199.99g 199.99g

[root@teuthology ~]# lvcreate -n mylv -L 2G linuxcast
  Logical volume "mylv" created.
[root@teuthology ~]# lvs
  LV   VG        Attr       LSize Pool Origin Data%  Meta%  Move Log Cpy%Sync Convert
  mylv linuxcast -wi-a----- 2.00g

[root@teuthology ~]# lvcreate -n mynewlv -L 2G linuxcast
  Logical volume "mynewlv" created.
[root@teuthology ~]# ll /dev/linuxcast/
total 0
lrwxrwxrwx 1 root root 7 Apr  8 14:59 mylv -> ../dm-0
lrwxrwxrwx 1 root root 7 Apr  8 15:03 mynewlv -> ../dm-1

[root@teuthology ~]# mkfs.ext4 /dev/linuxcast/mylv
[root@teuthology ~]# mount /dev/linuxcast/mylv /mnt/
[root@teuthology ~]# df -Th
Filesystem                 Type      Size  Used Avail Use% Mounted on
/dev/vda3                  xfs       195G  2.6G  193G   2% /
devtmpfs                   devtmpfs  3.9G     0  3.9G   0% /dev
tmpfs                      tmpfs     3.9G     0  3.9G   0% /dev/shm
tmpfs                      tmpfs     3.9G  8.6M  3.9G   1% /run
tmpfs                      tmpfs     3.9G     0  3.9G   0% /sys/fs/cgroup
/dev/vda1                  xfs      1014M  172M  843M  17% /boot
tmpfs                      tmpfs     783M     0  783M   0% /run/user/0
/dev/mapper/linuxcast-mylv ext4      2.0G  6.0M  1.8G   1% /mnt

#删除LVM
[root@teuthology ~]# umount  /mnt/
[root@teuthology ~]# lvremove /dev/linuxcast/mylv 
Do you really want to remove active logical volume linuxcast/mylv? [y/n]: y
  Logical volume "mylv" successfully removed
[root@teuthology ~]# lvremove /dev/linuxcast/mynewlv 
Do you really want to remove active logical volume linuxcast/mynewlv? [y/n]: y
  Logical volume "mynewlv" successfully removed
[root@teuthology ~]# lvs
[root@teuthology ~]# vgremove linuxcast
  Volume group "linuxcast" successfully removed
[root@teuthology ~]# vgs
[root@teuthology ~]# pvremove /dev/vdb 
  Labels on physical volume "/dev/vdb" successfully wiped.
[root@teuthology ~]# pvremove /dev/vdc
  Labels on physical volume "/dev/vdc" successfully wiped.
```

# LVM逻辑卷的拉伸与缩小



```
[root@teuthology ~]# lsblk
NAME   MAJ:MIN RM  SIZE RO TYPE MOUNTPOINT
vda    253:0    0  200G  0 disk 
├─vda1 253:1    0    1G  0 part /boot
├─vda2 253:2    0    4G  0 part [SWAP]
└─vda3 253:3    0  195G  0 part /
vdb    253:16   0  100G  0 disk 
vdc    253:32   0  100G  0 disk 

[root@teuthology ~]# pvcreate /dev/vdb /dev/vdc
  Physical volume "/dev/vdb" successfully created.
  Physical volume "/dev/vdc" successfully created.

[root@teuthology ~]# pvs
  PV         VG Fmt  Attr PSize   PFree  
  /dev/vdb      lvm2 ---  100.00g 100.00g
  /dev/vdc      lvm2 ---  100.00g 100.00g

[root@teuthology ~]# vgcreate linuxcast /dev/vdb /dev/vdc
  Volume group "linuxcast" successfully created

[root@teuthology ~]# lvcreate -n mylv -L 2G linuxcast
WARNING: ext4 signature detected on /dev/linuxcast/mylv at offset 1080. Wipe it? [y/n]: y
  Wiping ext4 signature on /dev/linuxcast/mylv.
  Logical volume "mylv" created.

[root@teuthology ~]# mkfs.ext4 /dev/linuxcast/mylv
mke2fs 1.42.9 (28-Dec-2013)
文件系统标签=
OS type: Linux
块大小=4096 (log=2)
分块大小=4096 (log=2)
Stride=0 blocks, Stripe width=0 blocks
131072 inodes, 524288 blocks
26214 blocks (5.00%) reserved for the super user
第一个数据块=0
Maximum filesystem blocks=536870912
16 block groups
32768 blocks per group, 32768 fragments per group
8192 inodes per group
Superblock backups stored on blocks: 
	32768, 98304, 163840, 229376, 294912

Allocating group tables: 完成                            
正在写入inode表: 完成                            
Creating journal (16384 blocks): 完成
Writing superblocks and filesystem accounting information: 完成 

[root@teuthology ~]# mount /dev/linuxcast/mylv /mnt/
[root@teuthology ~]# df -TH
文件系统                   类型      容量  已用  可用 已用% 挂载点
/dev/vda3                  xfs       210G  2.8G  207G    2% /
devtmpfs                   devtmpfs  4.1G     0  4.1G    0% /dev
tmpfs                      tmpfs     4.2G     0  4.2G    0% /dev/shm
tmpfs                      tmpfs     4.2G  9.0M  4.1G    1% /run
tmpfs                      tmpfs     4.2G     0  4.2G    0% /sys/fs/cgroup
/dev/vda1                  xfs       1.1G  180M  884M   17% /boot
tmpfs                      tmpfs     821M     0  821M    0% /run/user/0
/dev/mapper/linuxcast-mylv ext4      2.1G  6.3M  2.0G    1% /mnt

[root@teuthology ~]# vgs
  VG        #PV #LV #SN Attr   VSize   VFree  
  linuxcast   2   1   0 wz--n- 199.99g 197.99g
[root@teuthology ~]# lvs
  LV   VG        Attr       LSize Pool Origin Data%  Meta%  Move Log Cpy%Sync Convert
  mylv linuxcast -wi-ao---- 2.00g
[root@teuthology ~]# lvextend -L +1G /dev/linuxcast/mylv 
  Size of logical volume linuxcast/mylv changed from 2.00 GiB (512 extents) to 3.00 GiB (768 extents).
  Logical volume linuxcast/mylv successfully resized.
[root@teuthology ~]# lvs
  LV   VG        Attr       LSize Pool Origin Data%  Meta%  Move Log Cpy%Sync Convert
  mylv linuxcast -wi-ao---- 3.00g
[root@teuthology ~]# df -Th
文件系统                   类型      容量  已用  可用 已用% 挂载点
/dev/vda3                  xfs       195G  2.6G  193G    2% /
devtmpfs                   devtmpfs  3.9G     0  3.9G    0% /dev
tmpfs                      tmpfs     3.9G     0  3.9G    0% /dev/shm
tmpfs                      tmpfs     3.9G  8.6M  3.9G    1% /run
tmpfs                      tmpfs     3.9G     0  3.9G    0% /sys/fs/cgroup
/dev/vda1                  xfs      1014M  172M  843M   17% /boot
tmpfs                      tmpfs     783M     0  783M    0% /run/user/0
/dev/mapper/linuxcast-mylv ext4      2.0G  6.0M  1.8G    1% /mnt

[root@teuthology ~]# resize2fs /dev/linuxcast/mylv 
resize2fs 1.42.9 (28-Dec-2013)
Filesystem at /dev/linuxcast/mylv is mounted on /mnt; on-line resizing required
old_desc_blocks = 1, new_desc_blocks = 1
The filesystem on /dev/linuxcast/mylv is now 786432 blocks long.
[root@teuthology ~]# df -Th
文件系统                   类型      容量  已用  可用 已用% 挂载点
/dev/vda3                  xfs       195G  2.6G  193G    2% /
devtmpfs                   devtmpfs  3.9G     0  3.9G    0% /dev
tmpfs                      tmpfs     3.9G     0  3.9G    0% /dev/shm
tmpfs                      tmpfs     3.9G  8.6M  3.9G    1% /run
tmpfs                      tmpfs     3.9G     0  3.9G    0% /sys/fs/cgroup
/dev/vda1                  xfs      1014M  172M  843M   17% /boot
tmpfs                      tmpfs     783M     0  783M    0% /run/user/0
/dev/mapper/linuxcast-mylv ext4      2.9G  6.0M  2.8G    1% /mnt


```



```
[root@teuthology ~]# lsblk
NAME             MAJ:MIN RM  SIZE RO TYPE MOUNTPOINT
vda              253:0    0  200G  0 disk 
├─vda1           253:1    0    1G  0 part /boot
├─vda2           253:2    0    4G  0 part [SWAP]
└─vda3           253:3    0  195G  0 part /
vdb              253:16   0  100G  0 disk 
└─linuxcast-mylv 252:0    0    3G  0 lvm  /mnt
vdc              253:32   0  100G  0 disk 
vdd              253:48   0  100G  0 disk 
[root@teuthology ~]# pvcreate /dev/vd
vda   vda1  vda2  vda3  vdb   vdc   vdd   
[root@teuthology ~]# pvcreate /dev/vdd 
  Physical volume "/dev/vdd" successfully created.
[root@teuthology ~]# vgs
  VG        #PV #LV #SN Attr   VSize   VFree  
  linuxcast   2   1   0 wz--n- 199.99g 196.99g
[root@teuthology ~]# vgextend linuxcast /dev/vdd 
  Volume group "linuxcast" successfully extended
[root@teuthology ~]# vgs
  VG        #PV #LV #SN Attr   VSize    VFree   
  linuxcast   3   1   0 wz--n- <299.99g <296.99g
```



```

```



















