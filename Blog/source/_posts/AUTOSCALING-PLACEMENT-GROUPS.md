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

SIZE是存储在pool中的数据量。TARGET SIZE（如果存在）是管理员希望最终存储在该pool中的数据量。系统使用两个值中的较大者进行计算。

RATE是pool的multiplier（乘数或倍数），它确定要消耗多少raw（原始） storage capacity。例如，3个副本池的比率为3.0，而k=4，m=2擦除编码池的比率为1.5。

RAW CAPACITY是OSD上负责存储该pool（可能还有其他pool）数据的raw storage capacity的总量。RATIO是该pool消耗的总容量的比率（即ratio = size * rate / raw capacity）。

TARGET RATIO（如果存在）是管理员指定他们希望该pool使用的存储空间的比率。系统使用actual ratio和target ratio中的较大者进行计算。 如果同时指定了target size bytes和ratio ，则ratio优先。

PG_NUM是该pool的当前PG数。系统认为应将pool的pg_num更改为NEW PG_NUM（如果存在）。它始终是2的幂，并且仅在“理想”值与当前值的差异大于3倍时才存在。

最后一列，AUTOSCALE，是pool pg_autoscale_mode，on, off或warn。

### AUTOMATED SCALING

最简单的方法是允许群集根据使用情况自动扩展PG。Ceph将查看可用的总存储量和整个系统的target PG数量，查看每个pool中存储了多少数据并尝试分配相应的PG。该系统的方法相对保守，只有当当前 PG （pg_num） 的数量比它认为的要小 3 倍以上时，才会对pool进行更改。

每个 OSD 的target PG 数基于可配置的 mon_target_pg_per_osd（默认值：100），可通过以下功能进行调整：

```
ceph config set global mon_target_pg_per_osd 100
```

autoscaler根据每个per-subtree分析pool并进行调整。由于每个pool可能映射到不同的 CRUSH rule，并且每个rule可以跨不同的设备分发数据，所以Ceph将考虑独立使用层次结构的每个subtree。例如，映射到ssd类的OSD的pool和映射到hdd类的OSD的pool将分别具有最佳PG counts，具体取决于这些相应设备类型的数量。

### SPECIFYING EXPECTED POOL SIZE（指定预期的pool大小）

首次创建群集或pool时，它将占用群集总容量的一小部分，并在系统中显示只需要少量的placement groups。但是，在大多数情况下，群集管理员最好知道哪些pool会随着时间的推移消耗大部分系统容量。通过向ceph提供这些信息，可以从一开始就使用更适当数量的pg，从而防止pg-num中的后续更改以及在进行调整时与移动数据相关的开销。

pool的target size*可通过两种方式指定：按pool的绝对大小（即字节）或群集总容量的ratio（比率）指定。

例如：

```
ceph osd pool set mypool target_size_bytes 100T
```

会告诉系统mypool预计会占用100 TiB的空间。 或者：

```
ceph osd pool set mypool target_size_ratio .9
```

告诉系统mypool预计会消耗群集总容量的90％。

您还可以使用ceph osd pool create命令的可选`--target-size-bytes <bytes>`或`--target-size-ratio <ratio>`参数在创建时设置pool的target size。

请注意，如果指定了不可能的target size值（例如，容量大于整个群集的容量或ratio(s)之和大于1.0），则会引发health警告（POOL_TARET_SIZE_RATIO_OVERCOMMITTED或POOL_TARGET_SIZE_BYTES_OVERCOMMITTED）。

### SPECIFYING BOUNDS ON A POOL’S PGS（在pool的PGS上指定界限）

也可以为一个pool指定最小数量的PG。设置下限可防止Ceph将PG编号减少（或建议减少）到配置的编号以下。

您可以使用以下方法设置pool的最小PG数量：

```
ceph osd pool set <pool-name> pg_num_min <num>
```

您还可以使用ceph osd pool create命令的可选`--pg-num-min <num>`参数指定创建pool时的最小PG count。

# A PRESELECTION OF PG_NUM

使用以下方法创建新pool时：

```
ceph osd pool create {pool-name} [pg_num]
```

选择pg_num的值是可选的。 如果您未指定pg_num，则集群可以（默认情况下）根据pool中存储的数据为您自动对其进行调整（请参见上文， [Autoscaling placement groups](https://docs.ceph.com/docs/master/rados/operations/placement-groups/#pg-autoscaler)）。

或者，可以显式提供pg_num。 但是，是否指定pg_num值并不影响群集是否自动调整该值。 要启用或禁用自动调整，请执行以下操作：

```
ceph osd pool set {pool-name} pg_autoscaler_mode (on|off|warn)
```

传统上，每个OSD PG的"rule of thumb"为100。使用balancer的附加功能（默认情况下也启用的），每个OSD大约50 PG可能是合理的。autoscaler通常为您提供：

- 使每个pool中的PG与pool中的数据成比例
- 考虑到每个PG在OSD上的replication或erasuring-coding，最终每个OSD会有50-100个PG

# HOW ARE PLACEMENT GROUPS USED（如何使用PLACEMENT GROUPS）

placement group (PG)聚集pool中的objects，因为以每个object为基础跟踪object placement和object metadata在计算上是昂贵的，即，具有数百万个object的系统无法实际以每个object为基础跟踪placement。

![](https://docs.ceph.com/docs/master/_images/ditaa-1fde157d24b63e3b465d96eb6afea22078c85a90.png)

Ceph客户端将计算object应位于哪个placement group中。它通过hashing object ID并根据定义的pool中PG的数量和pool ID进行操作来实现此目的。有关详细信息，请参见 [Mapping PGs to OSDs](https://docs.ceph.com/docs/master/architecture#mapping-pgs-to-osds)。

placement group中object的内容存储在一组OSD中。 例如，在大小为2的replicated pool中，每个placement group将在两个OSD上存储objects，如下所示。

![](https://docs.ceph.com/docs/master/_images/ditaa-3c86866fb6edc99dad6ccf51e25e536806f0b079.png)

如果OSD #2失败，则将另一个分配给Placement Group #1，并用OSD #1中所有objects的副本填充。 如果pool大小从2更改为3，则会将一个额外的OSD分配给该placement group，并将接收该placement group中所有objects的副本。

Placement groups不拥有OSD； 他们与同一资源pool甚至其他资源pool中的其他placement groups共享它。 如果OSD #2失败，则Placement Group #2还必须使用OSD #3恢复objects的副本。

当placement groups的数量增加时，将为新的placement groups分配OSD。CRUSH函数的结果也将更改，并且先前placement groups中的某些objects将被复制到新的placement groups中，并从旧的placement groups中删除。

# PLACEMENT GROUPS TRADEOFFS（权衡）

数据持久性以及所有OSD之间的均匀分配都需要更多的placement groups，但应将其数量减少到最少，以节省CPU和内存。

### DATA DURABILITY（数据持久性）

OSD发生故障后，数据丢失的风险会增加，直到完全恢复其中包含的数据为止。 假设有一种情况会导致单个placement group中的数据永久丢失：

- OSD失败，并且它包含的object的所有副本均丢失。对于placement group中的所有objects，副本的数量突然从3个减少到2个。
- Ceph通过选择一个新的OSD重新创建所有objects的第三个副本，开始对该placement group的恢复。
- 在同一placement group内的另一个OSD在新OSD完全填充第三份副本之前发生故障。 这样，某些objects将只有一个幸存副本。
- Ceph选择了另一个OSD并保持复制objects以恢复所需的副本数。
- 在同一placement group中的第三个OSD在恢复完成之前发生故障。 如果此OSD包含object的唯一剩余副本，则它将永久丢失。

在三个副本pool中包含10个OSD和512个placement groups的群集中，CRUSH将为每个placement groups提供三个OSD。 最后，每个OSD将托管(512 * 3) / 10 = ~150 Placement Groups。 当第一个OSD发生故障时，以上情况将同时启动所有150个placement groups的恢复。

恢复的150个placement groups可能均匀分布在剩余的9个OSD上。 因此，每个剩余的OSD可能会将objects的副本发送给所有其他objects，并且还可能会接收一些要存储的新objects，因为它们已成为新placement group的一部分。

完成恢复所需的时间完全取决于Ceph集群的架构。 假设每个OSD由一台机器上的1TB SSD托管，并且所有OSD都连接到10Gb/s交换机，并且单个OSD的恢复在M分钟内完成。 如果每台计算机使用不带SSD journal的spinners和1Gb/s交换机的两个OSD，则速度至少要慢一个数量级。

在这种大小的群集中，placement groups的数量几乎对数据持久性没有影响。 可能是128或8192，恢复速度不会变慢或变快。

但是，将相同的Ceph集群增加到20个OSD而不是10个OSD可能会加快恢复速度，从而显着提高数据的持久性。 现在，每个OSD只能参与约75个placement groups，而不是只有10个OSD时的约150个placement groups，并且仍然需要全部19个剩余OSD执行相同数量的object副本才能恢复。 但是，如果10个OSD必须每个复制大约100GB，则现在它们必须每个复制50GB。 如果网络是瓶颈，恢复将以两倍的速度进行。 换句话说，当OSD数量增加时，恢复速度会更快。

如果该群集增长到40个OSD，则每个OSD将仅托管约35个placement groups。 如果OSD死亡，则恢复将保持更快的速度，除非它被另一个瓶颈阻塞。 但是，如果该群集增长到200个OSD，则每个OSD将仅托管约7个placement groups。 如果OSD死亡，则将在这些placement groups中的最多约21 (7 * 3)个OSD之间进行恢复：恢复将比有40个OSD时花费的时间更长，这意味着应增加placement groups的数量。

无论恢复时间有多短，第二个OSD在进行过程中都有可能发生故障。 在上述10个OSD群集中，如果它们中的任何一个失败，则〜17个placement groups（即，正在恢复〜150/9个placement groups）将只有一个幸存副本。 并且如果剩余的8个OSD中的任何一个失败，则两个placement groups的最后一个objects很可能会丢失（即〜17/8个placement groups，仅恢复了一个剩余副本）。

当群集的大小增加到20个OSD时，丢失三个OSD损坏的Placement Groups的数量将减少。 第二个OSD丢失将降级〜4个（即恢复到约75个/ 19个Placement Groups），而不是〜17个，而第三个OSD丢失则仅在它是包含尚存副本的四个OSD之一时才丢失数据。 换句话说，如果在恢复时间范围内丢失一个OSD的概率为0.0001％，则它从具有10个OSD的群集中的17 * 10 * 0.0001％变为具有20个OSD的群集中的4 * 20 * 0.0001％。

简而言之，更多的OSD意味着恢复更快，较低的级联故障风险，从而导致Placement Group的永久丢失。 就数据持久性而言，在少于50个OSD的群集中，具有512或4096个Placement Group大致等效。

注意：向群集添加的新OSD可能需要很长时间才能分配有分配给它的placement groups。 但是，不会降低任何object的质量，也不会影响群集中包含的数据的持久性。

### OBJECT DISTRIBUTION WITHIN A POOL（pool内的object分布）

理想情况下，object均匀地分布在每个placement group中。 由于CRUSH计算每个object的placement group，但实际上不知道该placement group内每个OSD中存储了多少数据，因此placement group数与OSD数之比可能会显着影响数据的分布。

例如，如果在三个副本pool中有一个用于十个OSD的placement group，则仅使用三个OSD，因为CRUSH别无选择。 当有更多的placement group可用时，object更有可能在其中均匀分布。 CRUSH还尽一切努力在所有现有的placement group中平均分配OSD。

只要Placement Groups比OSD多一个或两个数量级，则分布应该均匀。 例如，用于3个OSD的256个Placement Groups，用于10个OSD的512或1024个Placement Groups等。

数据分布不均可能是由OSD与placement groups之间的比率以外的因素引起的。 由于CRUSH未考虑object的大小，因此一些非常大的object可能会造成不平衡。 假设有100万个4K object（总计4GB）均匀分布在10个OSD的1024个placement groups中。 他们将在每个OSD上使用4GB / 10 = 400MB。 如果将一个400MB object添加到pool中，则支持放置object的placement groups的三个OSD将填充400MB + 400MB = 800MB，而其余七个将仅占据400MB。

### MEMORY, CPU AND NETWORK USAGE（内存，CPU和网络使用情况）

对于每个placement group，OSD和MON始终需要内存，网络和CPU，并且在恢复期间甚至更多。 通过对placement group内的object进行聚类objects来共享此开销是它们存在的主要原因之一。

最小化placement groups的数量可以节省大量资源。

# CHOOSING THE NUMBER OF PLACEMENT GROUPS（选择PLACEMENT GROUPS的数量）

如果您有超过50个OSD，我们建议每个OSD大约有50-100个placement groups，以平衡资源使用，数据持久性和分发。 如果OSD少于50个，则最好在上述[preselection](https://docs.ceph.com/docs/master/rados/operations/placement-groups/#preselection)中进行选择。 对于单个objects pool，可以使用以下公式获取baseline：

```
             (OSDs * 100)
Total PGs =  ------------
              pool size
```

pool size是replicated pools的副本数或erasure coded pools的K + M总和（由ceph osd erasure-code-profile get返回）。

然后，您应该检查设计Ceph集群的方式，以最大程度地提高[数据持久性](https://docs.ceph.com/docs/master/rados/operations/placement-groups/#data-durability)，[对象分配](https://docs.ceph.com/docs/master/rados/operations/placement-groups/#object-distribution)并最小化[资源使用](https://docs.ceph.com/docs/master/rados/operations/placement-groups/#resource-usage)。

结果应始终四舍五入到最接近的2的幂。

只有2的幂可以平衡placement groups之间的objects数量。 其他值将导致OSD上的数据分布不均。

例如，对于具有200个OSD和3个副本的pool大小的群集，您可以按以下方式估计PG的数量：

```
(200 * 100)
----------- = 6667. Nearest power of 2: 8192
     3
```

当使用多个data pools存储objects时，需要确保在每个pool的placement groups数量与每个OSD的placement groups数量之间取得平衡，以便获得合理的placement groups总数，从而为每个OSD提供合理的低偏差而不会增加系统资源的负担或使对等进程太慢。

例如，一个10个pool的群集，每个pool在十个OSD上具有512个placement groups，则总共有5120个placement groups分布在十个OSD上，即每个OSD 512个placement groups。 那不会使用太多资源。 但是，如果创建了1000个pool，每个pool有512个placement groups，则OSD将分别处理约50,000个placement groups，这将需要更多的资源和时间来进行对等。

您可能会发现[PGCalc](http://ceph.com/pgcalc/)工具很有帮助。

# SET THE NUMBER OF PLACEMENT GROUPS（设置PLACEMENT GROUPS数）

要设置pool中的placement groups数量，必须在创建pool时指定placement groups的数量。 有关详细信息，请参见[Create a Pool](https://docs.ceph.com/docs/master/rados/operations/pools#createpool)。 即使在创建pool之后，您也可以使用以下方法更改placement groups的数量：

```
ceph osd pool set {pool-name} pg_num {pg_num}
```

增加placement groups的数量之后，还必须增加placement（pgp_num）的数量，然后群集才能重新平衡。 pgp_num将是CRUSH算法考虑placement的placement groups的数量。 pg_num的增加会拆分placement groups，但是数据将不会迁移到较新的placement groups，直到placement的placement groups为止。 pgp_num增加了。 pgp_num应该等于pg_num。 要增加用于placement的placement groups的数量，请执行以下操作：

```
ceph osd pool set {pool-name} pgp_num {pgp_num}
```

当减少PG的数量时，将自动为您调整pgp_num。

# GET THE NUMBER OF PLACEMENT GROUPS（获取PLACEMENT GROUPS数）

要获取pool中的placement groups数，请执行以下操作：

```
ceph osd pool get {pool-name} pg_num
```

# GET A CLUSTER’S PG STATISTICS（获取集群的PG统计信息）

要获取集群中placement groups的统计信息，请执行以下操作：

```
ceph pg dump [--format {format}]
```

有效格式为plain（默认）和json。

# GET STATISTICS FOR STUCK PGS（获取STUCK PGS的统计信息）

要获取所有处于指定状态的placement groups的统计信息，请执行以下操作：

```
ceph pg dump_stuck inactive|unclean|stale|undersized|degraded [--format <format>] [-t|--threshold <seconds>]
```

Inactive Placement groups无法处理读写，因为它们正在等待OSD包含最新数据。

Unclean Placement groups包含的object未复制所需的次数。。 他们应该正在恢复。

Stale Placement groups处于未知状态—承载这些PG的OSD在一段时间内未向monitor报告（由mon_osd_report_timeout配置）。

有效格式为plain（默认）和json。 阈值定义placement group在将其包括在返回的统计信息之前卡住的最小秒数（默认为300秒）。

# GET A PG MAP

要获取特placement group map，请执行以下操作：

```
ceph pg map {pg-id}
```

例如：

```
ceph pg map 1.6c
```

Ceph将返回placement group map，placement group和OSD状态：

```
osdmap e13 pg 1.6c (1.6c) -> up [1,0] acting [1,0]
```

# GET A PGS STATISTICS（获取PGS统计信息）

要检索特定placement group的统计信息，请执行以下操作：

```
ceph pg {pg-id} query
```

# SCRUB A PLACEMENT GROUP

要scrub a placement group，请执行以下操作：

```
ceph pg scrub {pg-id}
```

Ceph检查primary和任何replica nodes生成的placement group中所有objects的目录进行比较，以确保没有丢失或不匹配的objects，并且它们的内容一致。 假设所有副本都匹配，则扫描可确保所有与snapshot-related的object metadata都是一致的。 通过日志报告错误。

要清理特定pool中的所有placement groups，请执行以下操作：

```
ceph osd pool scrub {pool-name}
```

# PRIORITIZE BACKFILL/RECOVERY OF A PLACEMENT GROUP(S)（优先考虑PLACEMENT GROUP的BACKFILL/RECOVERY）

您可能会遇到这样的情况，即一堆placement groups需要recovery和/或backfill，并且某些特定的groups保存的数据比其他的更为重要（例如，那些PG可能保存正在运行的机器使用的images的数据，而其他PG可能由不活动的机器使用/较少的相关数据）。 在这种情况下，您可能希望优先考虑恢复这些groups，以便更早恢复存储在这些groups上的数据的性能和/或可用性。 为此（在backfill或recovery期间将特定的placement group(s)标记为优先），请执行以下操作：

```
ceph pg force-recovery {pg-id} [{pg-id #2}] [{pg-id #3} ...]
ceph pg force-backfill {pg-id} [{pg-id #2}] [{pg-id #3} ...]
```

这将导致Ceph首先在其他placement groups之前对指定的placement groups执行recovery或backfill。 这不会中断当前正在进行的backfill或recovery，但会导致尽快处理指定的PG。 如果您改变主意或优先考虑wrong groups，请使用：

```
ceph pg cancel-force-recovery {pg-id} [{pg-id #2}] [{pg-id #3} ...]
ceph pg cancel-force-backfill {pg-id} [{pg-id #2}] [{pg-id #3} ...]
```

这将从这些PG中删除“force” flag，并将以默认顺序对其进行处理。 同样，这不会影响当前正在处理的placement groups，只会影响仍在排队的placement groups。

recovery或backfill group后，将自动清除“force” flag。

同样，您可以使用以下命令强制Ceph首先对指定pool中的所有placement groups执行recovery或backfill：

```
ceph osd pool force-recovery {pool-name}
ceph osd pool force-backfill {pool-name}
```

或

```
ceph osd pool cancel-force-recovery {pool-name}
ceph osd pool cancel-force-backfill {pool-name}
```

如果您改变主意，则可以恢复到默认的recovery或backfill优先级。

请注意，这些命令可能会破坏Ceph内部优先级计算的顺序，因此请谨慎使用！ 特别是，如果您有多个当前共享相同底层OSD的pool，并且某些特定pool中的数据比其他pool更重要，我们建议您使用以下命令以更好的顺序重新排列所有pool的recovery/backfill优先级：

```
ceph osd pool set {pool-name} recovery_priority {value}
```

例如，如果您有10个pool，则可以将最重要的一个优先级设置为10，下一个9，等等。或者您可以不理会大多数pool，而说3个重要的pool分别设置为优先级1或优先级3、2、1。

# REVERT LOST（永不消失）

如果群集丢失了一个或多个object，并且您决定放弃对丢失数据的搜索，则必须将unfound的object标记为lost。

如果已查询所有可能的位置并且仍然丢失了objects，则可能必须放弃丢失的objects。

当前唯一受支持的选项是“revert”，它可以回滚到该object的先前版本，或者（如果是新object）则完全忘记它。 要将“unfound”的object标记为“lost”，请执行以下操作：

```
ceph pg {pg-id} mark_unfound_lost revert|delete
```

重要说明：请谨慎使用此功能，因为它可能会使期望object存在的应用程序感到困惑。