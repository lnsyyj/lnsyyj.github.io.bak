---
title: ceph运维笔记
date: 2020-03-11 10:23:44
tags: ceph
---

# OSD

## 换盘操作

1、确保删除OSD时，集群没有接近near full

2、检查删除OSD时，是否有数据丢失， ceph osd ok-to-stop {osd-num}

```
无数据丢失情况
root@ceph:~# ceph osd ok-to-stop 2
OSD(s) 2 are ok to stop without reducing availability or risking data, provided there are no other concurrent failures or interventions.
70 PGs are likely to be degraded (but remain available) as a result.

有数据丢失情况
root@ceph:~# ceph osd ok-to-stop 2
Error EBUSY: 18 PGs are already too degraded, would become too degraded or might become unavailable
```

3、标记OSD out， ceph osd out {osd-num}

```
root@ceph:~# ceph osd out 3
marked out osd.3. 
 
root@ceph:~# ceph osd tree
ID CLASS WEIGHT  TYPE NAME     STATUS REWEIGHT PRI-AFF 
-1       0.08997 root default                          
-3       0.08997     host ceph                         
 0   ssd 0.02249         osd.0     up  1.00000 1.00000 
 1   ssd 0.02249         osd.1     up  1.00000 1.00000 
 2   ssd 0.02249         osd.2     up  1.00000 1.00000 
 3   ssd 0.02249         osd.3   down        0 1.00000 
```

4、停止OSD进程， systemctl stop ceph-osd@{osd-num}

```
root@ceph:~# systemctl stop ceph-osd@3

root@ceph:~# ps -ef | grep ceph-osd
ceph        3334       1  0 10:19 ?        00:00:13 /usr/bin/ceph-osd -f --cluster ceph --id 2 --setuser ceph --setgroup ceph
ceph        3336       1  0 10:19 ?        00:00:10 /usr/bin/ceph-osd -f --cluster ceph --id 0 --setuser ceph --setgroup ceph
ceph        3337       1  0 10:19 ?        00:00:10 /usr/bin/ceph-osd -f --cluster ceph --id 1 --setuser ceph --setgroup ceph
```

5、从crush map中删除OSD， ceph osd purge {osd-num} --yes-i-really-mean-it

```
root@ceph:~# ceph osd purge 3 --yes-i-really-mean-it
purged osd.3

root@ceph:~# ceph osd tree
ID CLASS WEIGHT  TYPE NAME     STATUS REWEIGHT PRI-AFF 
-1       0.06747 root default                          
-3       0.06747     host ceph                         
 0   ssd 0.02249         osd.0     up  1.00000 1.00000 
 1   ssd 0.02249         osd.1     up  1.00000 1.00000 
 2   ssd 0.02249         osd.2     up  1.00000 1.00000 
```

6、从ceph.conf中删除OSD

7、卸载OSD挂载点， umount /var/lib/ceph/osd/ceph-{osd-num}

```
umount /var/lib/ceph/osd/ceph-3
```

8、删除/var/lib/ceph/osd/ceph-{osd-num}/目录

```
rm -rf /var/lib/ceph/osd/ceph-3/
```

9、换盘后，重新添加OSD





















