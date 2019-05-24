---
title: prometheus ceph_exporter 监控项
date: 2019-05-24 22:15:29
tags: prometheus
---

截止2019-05-24 ceph_exporter监控项整理

| Metrics                              | Chinese   explanation                                        | English   explanation                                        |
| ------------------------------------ | ------------------------------------------------------------ | ------------------------------------------------------------ |
| ceph_active_pgs                      | 处于active状态的PG数                                         | # HELP   ceph_active_pgs No. of active PGs in the cluster    |
| ceph_backfill_wait_pgs               | 处于backfill_wait状态的PG数                                  | # HELP   ceph_backfill_wait_pgs No. of PGs in the cluster with backfill_wait state |
| ceph_backfilling_pgs                 | 处于backfilling状态的PG数                                    | # HELP   ceph_backfilling_pgs No. of backfilling PGs in the cluster |
| ceph_cache_evict_io_bytes            | 每秒从cache   pool（缓存池）中evicted（逐出）的字节数        | # HELP   ceph_cache_evict_io_bytes Rate of bytes being evicted from the cache pool per   second |
| ceph_cache_flush_io_bytes            | 每秒从cache   pool（缓存池）中flushed（刷新）的字节数        | # HELP   ceph_cache_flush_io_bytes Rate of bytes being flushed from the cache pool per   second |
| ceph_cache_promote_io_ops            | 每秒cache promote操作的总数                                  | # HELP   ceph_cache_promote_io_ops Total cache promote operations measured per second |
| ceph_client_io_ops                   | 客户端每秒操作总数                                           | # HELP   ceph_client_io_ops Total client ops on the cluster measured per second |
| ceph_client_io_read_bytes            | 客户端每秒read字节数                                         | # HELP   ceph_client_io_read_bytes Rate of bytes being read by all clients per second |
| ceph_client_io_read_ops              | 客户端每秒read总I/O操作数                                    | # HELP   ceph_client_io_read_ops Total client read I/O ops on the cluster measured per   second |
| ceph_client_io_write_bytes           | 客户端每秒write字节数                                        | # HELP   ceph_client_io_write_bytes Rate of bytes being written by all clients per   second |
| ceph_client_io_write_ops             | 客户端每秒write总I/O操作数                                   | # HELP   ceph_client_io_write_ops Total client write I/O ops on the cluster measured   per second |
| ceph_cluster_available_bytes         | 群集中的可用空间                                             | # HELP   ceph_cluster_available_bytes Available space within the cluster |
| ceph_cluster_capacity_bytes          | 群集的总容量                                                 | # HELP   ceph_cluster_capacity_bytes Total capacity of the cluster |
| ceph_cluster_objects                 | 集群中的rados object数                                       | # HELP   ceph_cluster_objects No. of rados objects within the cluster |
| ceph_cluster_used_bytes              | 集群已使用的容量                                             | # HELP   ceph_cluster_used_bytes Capacity of the cluster currently in use |
| ceph_deep_scrubbing_pgs              | deep scrubbing状态的PG数量                                   | # HELP   ceph_deep_scrubbing_pgs No. of deep scrubbing PGs in the cluster |
| ceph_degraded_objects                | 所有PG中degraded   objects的数量，包括副本                   | # HELP   ceph_degraded_objects No. of degraded objects across all PGs, includes   replicas |
| ceph_degraded_pgs                    | 处于degraded状态的PG数                                       | # HELP   ceph_degraded_pgs No. of PGs in a degraded state    |
| ceph_down_pgs                        | 处于down状态的PG数                                           | # HELP ceph_down_pgs   No. of PGs in the cluster in down state |
| ceph_forced_backfill_pgs             | 处于forced_backfill状态的PG数                                | # HELP   ceph_forced_backfill_pgs No. of PGs in the cluster with forced_backfill state |
| ceph_forced_recovery_pgs             | 处于forced_recovery状态的PG数                                | # HELP   ceph_forced_recovery_pgs No. of PGs in the cluster with forced_recovery state |
| ceph_health_status                   | 群集的health状态，只能在3种状态之间变化(err:2,   warn:1, ok:0) | # HELP   ceph_health_status Health status of Cluster, can vary only between 3 states   (err:2, warn:1, ok:0) |
| ceph_misplaced_objects               | 所有PG中misplaced   object的数量，包括副本                   | # HELP   ceph_misplaced_objects No. of misplaced objects across all PGs, includes   replicas |
| ceph_monitor_quorum_count            | monitor quorum的总数                                         | # HELP   ceph_monitor_quorum_count The total size of the monitor quorum |
| ceph_osd_avail_bytes                 | OSD可用存储字节数                                            | # HELP   ceph_osd_avail_bytes OSD Available Storage in Bytes |
| ceph_osd_average_utilization         | OSD平均利用率                                                | # HELP   ceph_osd_average_utilization OSD Average Utilization |
| ceph_osd_bytes                       | OSD总字节数                                                  | # HELP ceph_osd_bytes   OSD Total Bytes                      |
| ceph_osd_crush_weight                | OSD Crush Weight                                             | # HELP   ceph_osd_crush_weight OSD Crush Weight              |
| ceph_osd_depth                       | OSD 深度                                                     | # HELP ceph_osd_depth   OSD Depth                            |
| ceph_osd_in                          | OSD In状态                                                   | # HELP ceph_osd_in   OSD In Status                           |
| ceph_osd_perf_apply_latency_seconds  | OSD Perf Apply延迟                                           | # HELP   ceph_osd_perf_apply_latency_seconds OSD Perf Apply Latency |
| ceph_osd_perf_commit_latency_seconds | OSD Perf Commit延迟                                          | # HELP   ceph_osd_perf_commit_latency_seconds OSD Perf Commit Latency |
| ceph_osd_pgs                         | OSD Placement Group计数                                      | # HELP ceph_osd_pgs   OSD Placement Group Count              |
| ceph_osd_reweight                    | OSD Reweight                                                 | # HELP   ceph_osd_reweight OSD Reweight                      |
| ceph_osd_total_avail_bytes           | OSD可用存储总字节数                                          | # HELP   ceph_osd_total_avail_bytes OSD Total Available Storage Bytes |
| ceph_osd_total_bytes                 | OSD总存储字节数                                              | # HELP   ceph_osd_total_bytes OSD Total Storage Bytes        |
| ceph_osd_total_used_bytes            | OSD已使用总的存储字节数                                      | # HELP   ceph_osd_total_used_bytes OSD Total Used Storage Bytes |
| ceph_osd_up                          | OSD Up状态                                                   | # HELP ceph_osd_up   OSD Up Status                           |
| ceph_osd_used_bytes                  | OSD已使用的存储空间字节                                      | # HELP   ceph_osd_used_bytes OSD Used Storage in Bytes       |
| ceph_osd_utilization                 | OSD利用率                                                    | # HELP   ceph_osd_utilization OSD Utilization                |
| ceph_osd_variance                    | OSD方差                                                      | # HELP   ceph_osd_variance OSD Variance                      |
| ceph_osdmap_flag_full                | 群集标记为已满，无法提供写入服务                             | # HELP   ceph_osdmap_flag_full The cluster is flagged as full and cannot service   writes |
| ceph_osdmap_flag_nobackfill          | OSD不会被backfill                                            | # HELP   ceph_osdmap_flag_nobackfill OSDs will not be backfilled |
| ceph_osdmap_flag_nodeep_scrub        | 禁用Deep scrubbing                                           | # HELP   ceph_osdmap_flag_nodeep_scrub Deep scrubbing is disabled |
| ceph_osdmap_flag_nodown              | 忽略OSD失败报告，OSD不会被标记为down                         | # HELP   ceph_osdmap_flag_nodown OSD failure reports are ignored, OSDs will not be   marked as down |
| ceph_osdmap_flag_noin                | 不会自动标记out的OSD                                         | # HELP   ceph_osdmap_flag_noin OSDs that are out will not be automatically marked in |
| ceph_osdmap_flag_noout               | 在配置的间隔后，OSD不会自动标记out                           | # HELP   ceph_osdmap_flag_noout OSDs will not be automatically marked out after the   configured interval |
| ceph_osdmap_flag_norebalance         | 数据rebalancing暂停                                          | # HELP   ceph_osdmap_flag_norebalance Data rebalancing is suspended |
| ceph_osdmap_flag_norecover           | Recovery暂停                                                 | # HELP   ceph_osdmap_flag_norecover Recovery is suspended    |
| ceph_osdmap_flag_noscrub             | 禁用Scrubbing                                                | # HELP   ceph_osdmap_flag_noscrub Scrubbing is disabled      |
| ceph_osdmap_flag_notieragent         | Cache tiering   activity已暂停                               | # HELP   ceph_osdmap_flag_notieragent Cache tiering activity is suspended |
| ceph_osdmap_flag_noup                | 不允许OSD start                                              | # HELP   ceph_osdmap_flag_noup OSDs are not allowed to start |
| ceph_osdmap_flag_pauserd             | Reads暂停                                                    | # HELP   ceph_osdmap_flag_pauserd Reads are paused           |
| ceph_osdmap_flag_pausewr             | Writes暂停                                                   | # HELP   ceph_osdmap_flag_pausewr Writes are paused          |
| ceph_osds                            | 群集中总OSD数量                                              | # HELP ceph_osds   Count of total OSDs in the cluster        |
| ceph_osds_down                       | 处于DOWN状态的OSD数量                                        | # HELP ceph_osds_down   Count of OSDs that are in DOWN state |
| ceph_osds_in                         | 处于IN状态并可用于处理请求的OSD数量                          | # HELP ceph_osds_in   Count of OSDs that are in IN state and available to serve requests |
| ceph_osds_up                         | 处于UP状态的OSD数量                                          | # HELP ceph_osds_up   Count of OSDs that are in UP state     |
| ceph_peering_pgs                     | 群集中peering状态的PG数量                                    | # HELP   ceph_peering_pgs No. of peering PGs in the cluster  |
| ceph_pgs_remapped                    | remapped并引起cluster-wide（群集范围）移动的PG数量           | # HELP   ceph_pgs_remapped No. of PGs that are remapped and incurring cluster-wide   movement |
| ceph_recovering_pgs                  | 群集中recovering状态的PG数量                                 | # HELP   ceph_recovering_pgs No. of recovering PGs in the cluster |
| ceph_recovery_io_bytes               | 每秒recovery的字节数                                         | # HELP   ceph_recovery_io_bytes Rate of bytes being recovered in cluster per second |
| ceph_recovery_io_keys                | 每秒恢复的keys数率                                           | # HELP   ceph_recovery_io_keys Rate of keys being recovered in cluster per second |
| ceph_recovery_io_objects             | 每秒恢复的object的速率                                       | # HELP   ceph_recovery_io_objects Rate of objects being recovered in cluster per   second |
| ceph_recovery_wait_pgs               | 处于recovery_wait状态的PG数                                  | # HELP   ceph_recovery_wait_pgs No. of PGs in the cluster with recovery_wait state |
| ceph_scrubbing_pgs                   | 处于scrubbing状态的PG数                                      | # HELP   ceph_scrubbing_pgs No. of scrubbing PGs in the cluster |
| ceph_slow_requests                   | 慢速请求数                                                   | # HELP   ceph_slow_requests No. of slow requests             |
| ceph_stale_pgs                       | 处于stale状态的PG数                                          | # HELP ceph_stale_pgs   No. of stale PGs in the cluster      |
| ceph_stuck_degraded_pgs              | 处于degraded状态的PG数                                       | # HELP   ceph_stuck_degraded_pgs No. of PGs stuck in a degraded state |
| ceph_stuck_requests                  | stuck（卡住）的请求数                                        | # HELP   ceph_stuck_requests No. of stuck requests           |
| ceph_stuck_stale_pgs                 | 处于stale状态的PG数                                          | # HELP   ceph_stuck_stale_pgs No. of stuck stale PGs in the cluster |
| ceph_stuck_unclean_pgs               | 处于unclean状态的PG数                                        | # HELP   ceph_stuck_unclean_pgs No. of PGs stuck in an unclean state |
| ceph_stuck_undersized_pgs            | 处于undersized状态的PG数                                     | # HELP   ceph_stuck_undersized_pgs No. of stuck undersized PGs in the cluster |
| ceph_total_pgs                       | 集群中总的PG数                                               | # HELP ceph_total_pgs   Total no. of PGs in the cluster      |
| ceph_unclean_pgs                     | 处于unclean状态的PG数                                        | # HELP   ceph_unclean_pgs No. of PGs in an unclean state     |
| ceph_undersized_pgs                  | 处于undersized状态的PG数                                     | # HELP   ceph_undersized_pgs No. of undersized PGs in the cluster |