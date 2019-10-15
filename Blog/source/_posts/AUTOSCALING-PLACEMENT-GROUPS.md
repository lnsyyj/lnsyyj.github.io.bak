---
title: AUTOSCALING PLACEMENT GROUPS
date: 2019-10-15 14:26:54
tags: ceph
---

# AUTOSCALING PLACEMENT GROUPS

Placement groups (PGs)是ceph分布数据的内部实现。您可以通过启用pg-autoscaling允许根据群集的使用方式提出建议或自动调整PG。

系统中的每个pool都有一个pg_autoscale_mode属性，可以将其设置为off，on或warn。

- off: Disable该pool的autoscaling，[Choosing the number of Placement Groups](https://docs.ceph.com/docs/master/rados/operations/placement-groups/#choosing-number-of-placement-groups)

- on: 为指定的pool启用PG count自动调整。

- warn: 当调整PG count时提出health alerts

要为已有的pool设置autoscaling mode，请执行以下操作：

```
ceph osd pool set <pool-name> pg_autoscale_mode <mode>
```

例如，要对池foo启用autoscaling，请执行以下操作：

```
ceph osd pool set foo pg_autoscale_mode on
```

您还可以使用以下命令配置默认pg_autoscale_mode，该默认pg_autoscale_mode应用于以后创建的任何pool：

```
ceph config set global osd_pool_default_pg_autoscale_mode <mode>
```

### VIEWING PG SCALING RECOMMENDATIONS

您可以使用以下命令查看每个pool，pool的相对利用率以及对PG count的任何建议更改：

```
ceph osd pool autoscale-status
```

输出将类似于：

```
 POOL                         SIZE  TARGET SIZE  RATE  RAW CAPACITY   RATIO  TARGET RATIO  BIAS  PG_NUM  NEW PG_NUM  AUTOSCALE 
 cephfs_metadata             1536k                3.0        594.0G  0.0000                 4.0       8              warn      
 default.rgw.meta            1536k                3.0        594.0G  0.0000                 1.0       8              warn      
 cephfs_data                    0                 3.0        594.0G  0.0000                 1.0       8              warn      
 default.rgw.buckets.index      0                 3.0        594.0G  0.0000                 1.0       8              warn      
 default.rgw.control            0                 3.0        594.0G  0.0000                 1.0       8              warn      
 .rgw.root                   1344k                3.0        594.0G  0.0000                 1.0       8              warn      
 rbd                            0                 3.0        594.0G  0.0000                 1.0      16           4  on        
 default.rgw.log                0                 3.0        594.0G  0.0000                 1.0       8              warn      
```

SIZE是存储在pool中的数据量。 TARGET SIZE（如果存在）是管理员指定的数据量，管理员希望最终将其存储在该pool中。系统使用两个值中的较大者进行计算。

RATE是pool的multiplier（乘数或倍数），它确定要消耗多少raw（原始） storage capacity。例如，3个副本池的比率为3.0，而k=4，m=2擦除编码池的比率为1.5。

RAW CAPACITY是OSD上负责存储该pool（可能还有其他pool）数据的raw storage capacity的总量。RATIO是该pool消耗的总容量的比率（即ratio = size * rate / raw capacity）。

TARGET RATIO（如果存在）是管理员指定他们希望该pool使用的存储空间的比率。系统使用actual ratio和target ratio中的较大者进行计算。 如果同时指定了target size bytes和ratio ，则ratio优先。

PG_NUM是该pool的当前PG数。系统认为应将pool的pg_num更改为NEW PG_NUM（如果存在）。它始终是2的幂，并且仅在“理想”值与当前值的差异大于3时才存在。

最后一列，AUTOSCALE，是pool pg_autoscale_mode，将on, off或warn。

### AUTOMATED SCALING

最简单的方法是允许群集根据使用情况自动扩展PG。Ceph将查看整个系统PG的总可用存储量和目标数量，查看每个pool中存储了多少数据，并尝试相应地分配PG。该系统的方法相对保守，仅当当前PG数量（pg_num）偏离其认为的3倍以上时才对池进行更改。





















