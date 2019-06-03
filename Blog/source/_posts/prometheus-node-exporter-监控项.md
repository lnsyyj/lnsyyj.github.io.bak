---
title: prometheus node_exporter 监控项
date: 2019-05-27 14:01:45
tags: prometheus
---

截止2019-05-24 node_exporter监控项整理。

## Collectors

每个操作系统对collector的支持各不相同。 下表列出了所有现有collector和支持的系统。通过`--collector.<name>`标志来启用collector。默认情况下启用的collector可以通过`--no-collector.<name>`标志来禁用。

### 默认情况下启用

| Name       | Description                                                  | OS                                                           |
| ---------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| arp        | 来自`/proc/net/arp`的ARP统计信息                             | Linux                                                        |
| bcache     | 来自`/sys/fs/bcache/`的bcache统计信息                        | Linux                                                        |
| bonding    | Linux bonding interfaces已配置active slave的数量             | Linux                                                        |
| boottime   | 从`kern.boottime` sysctl派生的系统启动时间                   | Darwin, Dragonfly, FreeBSD, NetBSD, OpenBSD, Solaris         |
| conntrack  | 显示conntrack统计信息（如果没有`/proc/sys/net/netfilter/`，则不执行任何操作） | Linux                                                        |
| cpu        | CPU统计信息                                                  | Darwin, Dragonfly, FreeBSD, Linux, Solaris                   |
| cpufreq    | CPU频率统计信息                                              | Linux, Solaris                                               |
| diskstats  | 磁盘I/O统计信息                                              | Darwin, Linux, OpenBSD                                       |
| edac       | 检错和纠错的统计数据                                         | Linux                                                        |
| entropy    | 可用的熵（entropy）                                          | Linux                                                        |
| exec       | execution统计信息                                            | Dragonfly, FreeBSD                                           |
| filefd     | 来自`/proc/sys/fs/file-nr`的文件描述符统计信息               | Linux                                                        |
| filesystem | 文件系统统计信息，例如使用的磁盘空间                         | Darwin, Dragonfly, FreeBSD, Linux, OpenBSD                   |
| hwmon      | 来自`/sys/class/hwmon/`的硬件监控和传感器数据                | Linux                                                        |
| infiniband | 特定于InfiniBand和Intel OmniPath配置的网络统计信息           | Linux                                                        |
| ipvs       | 来自`/proc/net/ip_vs`的IPVS状态和来自`/proc/net/ip_vs_stats`的统计数据 | Linux                                                        |
| loadavg    | 负载平均值                                                   | Darwin, Dragonfly, FreeBSD, Linux, NetBSD, OpenBSD, Solaris  |
| mdadm      | 有关`/proc/mdstat`中设备的统计信息（如果没有`/proc/mdstat`，则不执行任何操作） | Linux                                                        |
| meminfo    | 内存统计信息                                                 | Darwin, Dragonfly, FreeBSD, Linux, OpenBSD                   |
| netclass   | 来自`/sys/class/net/`的网络接口信息                          | Linux                                                        |
| netdev     | 网络接口统计信息，如字节传输                                 | Darwin, Dragonfly, FreeBSD, Linux, OpenBSD                   |
| netstat    | 来自`/proc/net/netstat`的网络统计信息。 这与`netstat -s`的信息相同 | Linux                                                        |
| nfs        | 从`/proc/net/rpc/nfs`公开NFS客户端统计信息。 这与`nfsstat -c`的信息相同 | Linux                                                        |
| nfsd       | 从`/proc/net/rpc/nfsd`公开NFS内核服务器统计信息。 这与`nfsstat -s`的信息相同 | Linux                                                        |
| pressure   | 来自`/proc/pressure/`的压力失速统计                          | Linux (kernel 4.20+ and/or [CONFIG_PSI](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/Documentation/accounting/psi.txt)) |
| sockstat   | 公开来自`/proc/net/sockstat`的各种统计信息                   | Linux                                                        |
| stat       | 来自`/proc/stat`的各种统计信息。这包括启动时间，forks和interrupts。 | Linux                                                        |
| textfile   | 从本地磁盘读取的统计信息。必须设置`--collector.textfile.directory`标志 | *any*                                                        |
| time       | 当前的系统时间                                               | *any*                                                        |
| timex      | selected adjtimex(2) system call统计信息                     | Linux                                                        |
| uname      | uname系统调用提供的系统信息                                  | FreeBSD, Linux                                               |
| vmstat     | 来自`/proc/vmstat`的统计信息                                 | Linux                                                        |
| xfs        | XFS运行时统计信息                                            | Linux (kernel 4.4+)                                          |
| zfs        | [ZFS](http://open-zfs.org/)性能统计数据                      | [Linux](http://zfsonlinux.org/), Solaris                     |

### 默认情况下禁用

由于内核配置和安全设置，默认情况下，所有Linux系统上的perf收集器可能无法正常工作。 要允许访问，请设置以下sysctl参数：

```
sysctl -w kernel.perf_event_paranoid=X
```

2 仅允许用户空间度量（自Linux 4.6起默认）。

1 允许内核和用户度量（在Linux 4.6之前默认）。

0 允许访问特定CPU的数据，但不允许访问raw tracepoint samples。

-1 没有限制。

根据配置的值，将提供不同的度量标准，对于大多数情况，`0`将提供最完整的设置。 有关更多信息，请参阅`man 2 perf_event_open`。

| Name         | Description                                                  | OS                 |
| ------------ | ------------------------------------------------------------ | ------------------ |
| buddyinfo    | /proc/buddyinfo报告的内存碎片统计信息                        | Linux              |
| devstat      | 设备统计信息                                                 | Dragonfly, FreeBSD |
| drbd         | 分布式副本（Replicated）块设备统计信息（到版本8.4）          | Linux              |
| interrupts   | 详细的中断统计                                               | Linux, OpenBSD     |
| ksmd         | 来自`/sys/kernel/mm/ksm`的内核和系统统计信息                 | Linux              |
| logind       | 会话计数来自[logind](http://www.freedesktop.org/wiki/Software/systemd/logind/) | Linux              |
| meminfo_numa | 来自`/proc/meminfo_numa`的内存统计信息                       | Linux              |
| mountstats   | 来自`/proc/self/mountstats`的文件系统统计信息。详细的NFS客户端统计信息。 | Linux              |
| ntp          | 本地NTP守护程序运行状况检查[时间](https://github.com/prometheus/node_exporter/blob/master/docs/TIME.md) | *any*              |
| processes    | 来自`/proc`的聚合进程统计信息                                | Linux              |
| qdisc        | [queuing discipline](https://en.wikipedia.org/wiki/Network_scheduler#Linux_kernel) 统计 | Linux              |
| runit        | 来自[runit](http://smarden.org/runit/)的服务状态统计         | *any*              |
| supervisord  | 来自[supervisord](http://supervisord.org/)的服务状态统计     | *any*              |
| systemd      | 来自[systemd](http://www.freedesktop.org/wiki/Software/systemd/)的服务和系统状态统计 | Linux              |
| tcpstat      | 来自`/proc/net/tcp`和`/proc/net/tcp6`的TCP连接状态信息。（警告：当前版本在高负载情况下存在潜在的性能问题。） | Linux              |
| wifi         | WiFi设备和station统计                                        | Linux              |
| perf         | 基于perf的指标（警告：指标取决于内核配置和设置）             | Linux              |

### Textfile Collector

textfile collector类似于[Pushgateway](https://github.com/prometheus/pushgateway)因为它允许从批处理job导出统计信息。它还可用于导出静态指标，例如计算机具有的role。Pushgateway应该用于服务级别指标。textfile模式用于绑定计算机的度量标准。

要使用它，请在Node exporter上设置`--collector.textfile.directory`标志。collector将使用文本格式解析该目录中与glob `*.prom`匹配的所有文件。 注意：不支持时间戳。

以原子方式推送cron job的完成时间：

```
echo my_batch_job_completion_time $(date +%s) > /path/to/directory/my_batch_job.prom.$$
mv /path/to/directory/my_batch_job.prom.$$ /path/to/directory/my_batch_job.prom
```

使用标签静态设置计算机的roles：

```
echo 'role{role="application_server"} 1' > /path/to/directory/role.prom.$$
mv /path/to/directory/role.prom.$$ /path/to/directory/role.prom
```

### Filtering enabled collectors

默认情况下，`node_exporter`将从启用的collector中公开所有指标。 这是收集指标以避免在比较不同系列的指标时出错的建议方法。

高级使用`node_exporter`可以传递一个可选的收集器列表来过滤指标。 `collect[]`参数可以多次使用。 在Prometheus配置中，您可以在[scrape config](https://prometheus.io/docs/prometheus/latest/configuration/configuration/#%3Cscrape_config%3E)

```
  params:
    collect[]:
      - foo
      - bar
```

这对于让不同的Prometheus服务器从节点收集特定指标非常有用。

## 部分监控项见下表

| Metrics                                             | Chinese   explanation                                        | English   explanation                                        |
| --------------------------------------------------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| node_arp_entries                                    | device的ARP表项                                              | #   HELP node_arp_entries ARP entries by device              |
| node_boot_time_seconds                              | 节点启动时间，unixtime                                       | #   HELP node_boot_time_seconds Node boot time, in unixtime. |
| node_context_switches_total                         | context   switches（上下文切换）的总数                       | #   HELP node_context_switches_total Total number of context switches. |
| node_cpu_guest_seconds_total                        | 每种模式在guests(VM)上花费CPU的秒数                          | #   HELP node_cpu_guest_seconds_total Seconds the cpus spent in guests (VMs) for   each mode. |
| node_cpu_seconds_total                              | 在每种模式下花费CPU的秒数                                    | #   HELP node_cpu_seconds_total Seconds the cpus spent in each mode. |
| node_disk_io_now                                    | 当前正在进行的I/O数量                                        | #   HELP node_disk_io_now The number of I/Os currently in progress. |
| node_disk_io_time_seconds_total                     | 执行I/O所花费的总时间                                        | #   HELP node_disk_io_time_seconds_total Total seconds spent doing I/Os. |
| node_disk_io_time_weighted_seconds_total            | 进行I/O所花费的加权秒数                                      | #   HELP node_disk_io_time_weighted_seconds_total The weighted # of seconds spent   doing I/Os. |
| node_disk_read_bytes_total                          | 成功读取的总字节数                                           | #   HELP node_disk_read_bytes_total The total number of bytes read successfully. |
| node_disk_read_time_seconds_total                   | 所有读取花费的总秒数                                         | #   HELP node_disk_read_time_seconds_total The total number of seconds spent by   all reads. |
| node_disk_reads_completed_total                     | 成功完成的读取总数                                           | #   HELP node_disk_reads_completed_total The total number of reads completed   successfully. |
| node_disk_reads_merged_total                        | 读合并的次数                                                 | #   HELP node_disk_reads_merged_total The total number of reads merged. |
| node_disk_write_time_seconds_total                  | 这是所有写入花费的总秒数                                     | #   HELP node_disk_write_time_seconds_total This is the total number of seconds   spent by all writes. |
| node_disk_writes_completed_total                    | 成功完成的写入总数                                           | #   HELP node_disk_writes_completed_total The total number of writes completed   successfully. |
| node_disk_writes_merged_total                       | 写合并的次数                                                 | #   HELP node_disk_writes_merged_total The number of writes merged. |
| node_disk_written_bytes_total                       | 成功写入的总字节数                                           | #   HELP node_disk_written_bytes_total The total number of bytes written   successfully. |
| node_entropy_available_bits                         | 可用entropy的Bits（比特）                                    | #   HELP node_entropy_available_bits Bits of available entropy. |
| node_exporter_build_info                            | 构建node_exporter的版本，修订版，分支和goversion标记，具有常量值“1”的metric | #   HELP node_exporter_build_info A metric with a constant '1' value labeled by   version, revision, branch, and goversion from which node_exporter was built. |
| node_filefd_allocated                               | 文件描述符统计：已分配                                       | #   HELP node_filefd_allocated File descriptor statistics: allocated. |
| node_filefd_maximum                                 | 文件描述符统计：最大值                                       | #   HELP node_filefd_maximum File descriptor statistics: maximum. |
| node_filesystem_avail_bytes                         | 非root用户可用的文件系统空间（以字节为单位）                 | #   HELP node_filesystem_avail_bytes Filesystem space available to non-root users   in bytes. |
| node_filesystem_device_error                        | 获取指定设备的统计信息时是否发生错误                         | #   HELP node_filesystem_device_error Whether an error occurred while getting   statistics for the given device. |
| node_filesystem_files                               | 文件系统总的file   nodes数（inode）                          | #   HELP node_filesystem_files Filesystem total file nodes.  |
| node_filesystem_files_free                          | 文件系统空闲file   node数（inode）                           | #   HELP node_filesystem_files_free Filesystem total free file nodes. |
| node_filesystem_free_bytes                          | 文件系统可用空间，以字节为单位                               | #   HELP node_filesystem_free_bytes Filesystem free space in bytes. |
| node_filesystem_readonly                            | 文件系统只读状态                                             | #   HELP node_filesystem_readonly Filesystem read-only status. |
| node_filesystem_size_bytes                          | 文件系统大小（字节）                                         | #   HELP node_filesystem_size_bytes Filesystem size in bytes. |
| node_forks_total                                    | forks总数                                                    | #   HELP node_forks_total Total number of forks.             |
| node_hwmon_chip_names                               | 人类可读（可读性良好）的芯片名称的注释指标                   | #   HELP node_hwmon_chip_names Annotation metric for human-readable chip names |
| node_hwmon_sensor_label                             | 给定芯片和传感器的标签                                       | #   HELP node_hwmon_sensor_label Label for given chip and sensor |
| node_hwmon_temp_celsius                             | 温度硬件监视器（输入）                                       | #   HELP node_hwmon_temp_celsius Hardware monitor for temperature (input) |
| node_hwmon_temp_crit_alarm_celsius                  | 温度硬件监控器（crit_alarm）                                 | #   HELP node_hwmon_temp_crit_alarm_celsius Hardware monitor for temperature   (crit_alarm) |
| node_hwmon_temp_crit_celsius                        | 温度硬件监控器（暴击）                                       | #   HELP node_hwmon_temp_crit_celsius Hardware monitor for temperature (crit) |
| node_hwmon_temp_max_celsius                         | 温度硬件监控器（最大）                                       | #   HELP node_hwmon_temp_max_celsius Hardware monitor for temperature (max) |
| node_intr_total                                     | 服务的中断总数                                               | #   HELP node_intr_total Total number of interrupts serviced. |
| node_load1                                          | 1m负载平均值                                                 | #   HELP node_load1 1m load average.                         |
| node_load15                                         | 15m负载平均值                                                | #   HELP node_load15 15m load average.                       |
| node_load5                                          | 5m负载平均值                                                 | #   HELP node_load5 5m load average.                         |
| node_memory_Active_anon_bytes                       | 内存信息字段Active_anon_bytes                                | #   HELP node_memory_Active_anon_bytes Memory information field   Active_anon_bytes. |
| node_memory_Active_bytes                            | 内存信息字段Active_bytes                                     | #   HELP node_memory_Active_bytes Memory information field Active_bytes. |
| node_memory_Active_file_bytes                       | 内存信息字段Active_file_bytes                                | #   HELP node_memory_Active_file_bytes Memory information field   Active_file_bytes. |
| node_memory_AnonHugePages_bytes                     | 内存信息字段AnonHugePages_bytes                              | #   HELP node_memory_AnonHugePages_bytes Memory information field   AnonHugePages_bytes. |
| node_memory_AnonPages_bytes                         | 内存信息字段AnonPages_bytes                                  | #   HELP node_memory_AnonPages_bytes Memory information field AnonPages_bytes. |
| node_memory_Bounce_bytes                            | 内存信息字段Bounce_bytes                                     | #   HELP node_memory_Bounce_bytes Memory information field Bounce_bytes. |
| node_memory_Buffers_bytes                           | 内存信息字段Buffers_bytes                                    | #   HELP node_memory_Buffers_bytes Memory information field Buffers_bytes. |
| node_memory_Cached_bytes                            | 内存信息字段Cached_bytes                                     | #   HELP node_memory_Cached_bytes Memory information field Cached_bytes. |
| node_memory_CmaFree_bytes                           | 内存信息字段CmaFree_bytes                                    | #   HELP node_memory_CmaFree_bytes Memory information field CmaFree_bytes. |
| node_memory_CmaTotal_bytes                          | 内存信息字段CmaTotal_bytes                                   | #   HELP node_memory_CmaTotal_bytes Memory information field CmaTotal_bytes. |
| node_memory_CommitLimit_bytes                       | 内存信息字段CommitLimit_bytes                                | #   HELP node_memory_CommitLimit_bytes Memory information field   CommitLimit_bytes. |
| node_memory_Committed_AS_bytes                      | 内存信息字段Committed_AS_bytes                               | #   HELP node_memory_Committed_AS_bytes Memory information field   Committed_AS_bytes. |
| node_memory_DirectMap1G_bytes                       | 内存信息字段DirectMap1G_bytes                                | #   HELP node_memory_DirectMap1G_bytes Memory information field   DirectMap1G_bytes. |
| node_memory_DirectMap2M_bytes                       | 内存信息字段DirectMap2M_bytes                                | #   HELP node_memory_DirectMap2M_bytes Memory information field   DirectMap2M_bytes. |
| node_memory_DirectMap4k_bytes                       | 内存信息字段DirectMap4k_bytes                                | #   HELP node_memory_DirectMap4k_bytes Memory information field   DirectMap4k_bytes. |
| node_memory_Dirty_bytes                             | 内存信息字段Dirty_bytes                                      | #   HELP node_memory_Dirty_bytes Memory information field Dirty_bytes. |
| node_memory_HardwareCorrupted_bytes                 | 内存信息字段HardwareCorrupted_bytes                          | #   HELP node_memory_HardwareCorrupted_bytes Memory information field   HardwareCorrupted_bytes. |
| node_memory_HugePages_Free                          | 内存信息字段HugePages_Free                                   | #   HELP node_memory_HugePages_Free Memory information field HugePages_Free. |
| node_memory_HugePages_Rsvd                          | 内存信息字段HugePages_Rsvd                                   | #   HELP node_memory_HugePages_Rsvd Memory information field HugePages_Rsvd. |
| node_memory_HugePages_Surp                          | 内存信息字段HugePages_Surp                                   | #   HELP node_memory_HugePages_Surp Memory information field HugePages_Surp. |
| node_memory_HugePages_Total                         | 内存信息字段HugePages_Total                                  | #   HELP node_memory_HugePages_Total Memory information field HugePages_Total. |
| node_memory_Hugepagesize_bytes                      | 内存信息字段Hugepagesize_bytes                               | #   HELP node_memory_Hugepagesize_bytes Memory information field   Hugepagesize_bytes. |
| node_memory_Inactive_anon_bytes                     | 内存信息字段Inactive_anon_bytes                              | #   HELP node_memory_Inactive_anon_bytes Memory information field   Inactive_anon_bytes. |
| node_memory_Inactive_bytes                          | 内存信息字段Inactive_bytes                                   | #   HELP node_memory_Inactive_bytes Memory information field Inactive_bytes. |
| node_memory_Inactive_file_bytes                     | 内存信息字段Inactive_file_bytes                              | #   HELP node_memory_Inactive_file_bytes Memory information field   Inactive_file_bytes. |
| node_memory_KernelStack_bytes                       | 内存信息字段KernelStack_bytes                                | #   HELP node_memory_KernelStack_bytes Memory information field   KernelStack_bytes. |
| node_memory_Mapped_bytes                            | 内存信息字段Mapped_bytes                                     | #   HELP node_memory_Mapped_bytes Memory information field Mapped_bytes. |
| node_memory_MemAvailable_bytes                      | 内存信息字段MemAvailable_bytes                               | #   HELP node_memory_MemAvailable_bytes Memory information field   MemAvailable_bytes. |
| node_memory_MemFree_bytes                           | 内存信息字段MemFree_bytes                                    | #   HELP node_memory_MemFree_bytes Memory information field MemFree_bytes. |
| node_memory_MemTotal_bytes                          | 内存信息字段MemTotal_bytes                                   | #   HELP node_memory_MemTotal_bytes Memory information field MemTotal_bytes. |
| node_memory_Mlocked_bytes                           | 内存信息字段Mlocked_bytes                                    | #   HELP node_memory_Mlocked_bytes Memory information field Mlocked_bytes. |
| node_memory_NFS_Unstable_bytes                      | 内存信息字段NFS_Unstable_bytes                               | #   HELP node_memory_NFS_Unstable_bytes Memory information field   NFS_Unstable_bytes. |
| node_memory_PageTables_bytes                        | 内存信息字段PageTables_bytes                                 | #   HELP node_memory_PageTables_bytes Memory information field PageTables_bytes. |
| node_memory_SReclaimable_bytes                      | 内存信息字段SReclaimable_bytes                               | #   HELP node_memory_SReclaimable_bytes Memory information field   SReclaimable_bytes. |
| node_memory_SUnreclaim_bytes                        | 内存信息字段SUnreclaim_bytes                                 | #   HELP node_memory_SUnreclaim_bytes Memory information field SUnreclaim_bytes. |
| node_memory_Shmem_bytes                             | 内存信息字段Shmem_bytes                                      | #   HELP node_memory_Shmem_bytes Memory information field Shmem_bytes. |
| node_memory_Slab_bytes                              | 内存信息字段Slab_bytes                                       | #   HELP node_memory_Slab_bytes Memory information field Slab_bytes. |
| node_memory_SwapCached_bytes                        | 内存信息字段SwapCached_bytes                                 | #   HELP node_memory_SwapCached_bytes Memory information field SwapCached_bytes. |
| node_memory_SwapFree_bytes                          | 内存信息字段SwapFree_bytes                                   | #   HELP node_memory_SwapFree_bytes Memory information field SwapFree_bytes. |
| node_memory_SwapTotal_bytes                         | 内存信息字段SwapTotal_bytes                                  | #   HELP node_memory_SwapTotal_bytes Memory information field SwapTotal_bytes. |
| node_memory_Unevictable_bytes                       | 内存信息字段Unevictable_bytes                                | #   HELP node_memory_Unevictable_bytes Memory information field   Unevictable_bytes. |
| node_memory_VmallocChunk_bytes                      | 内存信息字段VmallocChunk_bytes                               | #   HELP node_memory_VmallocChunk_bytes Memory information field   VmallocChunk_bytes. |
| node_memory_VmallocTotal_bytes                      | 内存信息字段VmallocTotal_bytes                               | #   HELP node_memory_VmallocTotal_bytes Memory information field   VmallocTotal_bytes. |
| node_memory_VmallocUsed_bytes                       | 内存信息字段VmallocUsed_bytes                                | #   HELP node_memory_VmallocUsed_bytes Memory information field   VmallocUsed_bytes. |
| node_memory_WritebackTmp_bytes                      | 内存信息字段WritebackTmp_bytes                               | #   HELP node_memory_WritebackTmp_bytes Memory information field   WritebackTmp_bytes. |
| node_memory_Writeback_bytes                         | 内存信息字段Writeback_bytes                                  | #   HELP node_memory_Writeback_bytes Memory information field Writeback_bytes. |
| node_netstat_Icmp6_InErrors                         | 统计Icmp6InErrors                                            | #   HELP node_netstat_Icmp6_InErrors Statistic Icmp6InErrors. |
| node_netstat_Icmp6_InMsgs                           | 统计Icmp6InMsgs                                              | #   HELP node_netstat_Icmp6_InMsgs Statistic Icmp6InMsgs.    |
| node_netstat_Icmp6_OutMsgs                          | 统计Icmp6OutMsgs                                             | #   HELP node_netstat_Icmp6_OutMsgs Statistic Icmp6OutMsgs.  |
| node_netstat_Icmp_InErrors                          | 统计IcmpInErrors                                             | #   HELP node_netstat_Icmp_InErrors Statistic IcmpInErrors.  |
| node_netstat_Icmp_InMsgs                            | 统计IcmpInMsgs                                               | #   HELP node_netstat_Icmp_InMsgs Statistic IcmpInMsgs.      |
| node_netstat_Icmp_OutMsgs                           | 统计IcmpOutMsgs                                              | #   HELP node_netstat_Icmp_OutMsgs Statistic IcmpOutMsgs.    |
| node_netstat_Ip6_InOctets                           | 统计Ip6InOctets                                              | #   HELP node_netstat_Ip6_InOctets Statistic Ip6InOctets.    |
| node_netstat_Ip6_OutOctets                          | 统计Ip6OutOctets                                             | #   HELP node_netstat_Ip6_OutOctets Statistic Ip6OutOctets.  |
| node_netstat_IpExt_InOctets                         | 统计IpExtInOctets                                            | #   HELP node_netstat_IpExt_InOctets Statistic IpExtInOctets. |
| node_netstat_IpExt_OutOctets                        | 统计IpExtOutOctets                                           | #   HELP node_netstat_IpExt_OutOctets Statistic IpExtOutOctets. |
| node_netstat_Ip_Forwarding                          | 统计IpForwarding                                             | #   HELP node_netstat_Ip_Forwarding Statistic IpForwarding.  |
| node_netstat_TcpExt_ListenDrops                     | 统计TcpExtListenDrops                                        | #   HELP node_netstat_TcpExt_ListenDrops Statistic TcpExtListenDrops. |
| node_netstat_TcpExt_ListenOverflows                 | 统计TcpExtListenOverflows                                    | #   HELP node_netstat_TcpExt_ListenOverflows Statistic TcpExtListenOverflows. |
| node_netstat_TcpExt_SyncookiesFailed                | 统计TcpExtSyncookiesFailed                                   | #   HELP node_netstat_TcpExt_SyncookiesFailed Statistic TcpExtSyncookiesFailed. |
| node_netstat_TcpExt_SyncookiesRecv                  | 统计TcpExtSyncookiesRecv                                     | #   HELP node_netstat_TcpExt_SyncookiesRecv Statistic TcpExtSyncookiesRecv. |
| node_netstat_TcpExt_SyncookiesSent                  | 统计TcpExtSyncookiesSent                                     | #   HELP node_netstat_TcpExt_SyncookiesSent Statistic TcpExtSyncookiesSent. |
| node_netstat_TcpExt_TCPSynRetrans                   | 统计TcpExtTCPSynRetrans                                      | #   HELP node_netstat_TcpExt_TCPSynRetrans Statistic TcpExtTCPSynRetrans. |
| node_netstat_Tcp_ActiveOpens                        | 统计TcpActiveOpens                                           | #   HELP node_netstat_Tcp_ActiveOpens Statistic TcpActiveOpens. |
| node_netstat_Tcp_CurrEstab                          | 统计TcpCurrEstab                                             | #   HELP node_netstat_Tcp_CurrEstab Statistic TcpCurrEstab.  |
| node_netstat_Tcp_InErrs                             | 统计TcpInErrs                                                | #   HELP node_netstat_Tcp_InErrs Statistic TcpInErrs.        |
| node_netstat_Tcp_InSegs                             | 统计TcpInSegs                                                | #   HELP node_netstat_Tcp_InSegs Statistic TcpInSegs.        |
| node_netstat_Tcp_OutSegs                            | 统计TcpOutSegs                                               | #   HELP node_netstat_Tcp_OutSegs Statistic TcpOutSegs.      |
| node_netstat_Tcp_PassiveOpens                       | 统计TcpPassiveOpens                                          | #   HELP node_netstat_Tcp_PassiveOpens Statistic TcpPassiveOpens. |
| node_netstat_Tcp_RetransSegs                        | 统计TcpRetransSegs                                           | #   HELP node_netstat_Tcp_RetransSegs Statistic TcpRetransSegs. |
| node_netstat_Udp6_InDatagrams                       | 统计Udp6InDatagrams                                          | #   HELP node_netstat_Udp6_InDatagrams Statistic Udp6InDatagrams. |
| node_netstat_Udp6_InErrors                          | 统计Udp6InErrors                                             | #   HELP node_netstat_Udp6_InErrors Statistic Udp6InErrors.  |
| node_netstat_Udp6_NoPorts                           | 统计Udp6NoPorts                                              | #   HELP node_netstat_Udp6_NoPorts Statistic Udp6NoPorts.    |
| node_netstat_Udp6_OutDatagrams                      | 统计Udp6OutDatagrams                                         | #   HELP node_netstat_Udp6_OutDatagrams Statistic Udp6OutDatagrams. |
| node_netstat_UdpLite6_InErrors                      | 统计UdpLite6InErrors                                         | #   HELP node_netstat_UdpLite6_InErrors Statistic UdpLite6InErrors. |
| node_netstat_UdpLite_InErrors                       | 统计UdpLiteInErrors                                          | #   HELP node_netstat_UdpLite_InErrors Statistic UdpLiteInErrors. |
| node_netstat_Udp_InDatagrams                        | 统计UdpInDatagrams                                           | #   HELP node_netstat_Udp_InDatagrams Statistic UdpInDatagrams. |
| node_netstat_Udp_InErrors                           | 统计UdpInErrors                                              | #   HELP node_netstat_Udp_InErrors Statistic UdpInErrors.    |
| node_netstat_Udp_NoPorts                            | 统计UdpNoPorts                                               | #   HELP node_netstat_Udp_NoPorts Statistic UdpNoPorts.      |
| node_netstat_Udp_OutDatagrams                       | 统计UdpOutDatagrams                                          | #   HELP node_netstat_Udp_OutDatagrams Statistic UdpOutDatagrams. |
| node_network_address_assign_type                    | /sys/class/net/<iface>   address_assign_type值               | #   HELP node_network_address_assign_type address_assign_type value of   /sys/class/net/<iface>. |
| node_network_carrier                                | /sys/class/net/<iface>   carrier值                           | #   HELP node_network_carrier carrier value of /sys/class/net/<iface>. |
| node_network_carrier_changes_total                  | /sys/class/net/<iface>   carrier_changes_total值             | #   HELP node_network_carrier_changes_total carrier_changes_total value of   /sys/class/net/<iface>. |
| node_network_device_id                              | /sys/class/net/<iface>   device_id值                         | #   HELP node_network_device_id device_id value of /sys/class/net/<iface>. |
| node_network_dormant                                | /sys/class/net/<iface>   dormant（休眠）值                   | #   HELP node_network_dormant dormant value of /sys/class/net/<iface>. |
| node_network_flags                                  | /sys/class/net/<iface>   flags值                             | #   HELP node_network_flags flags value of /sys/class/net/<iface>. |
| node_network_iface_id                               | /sys/class/net/<iface>   iface_id值                          | #   HELP node_network_iface_id iface_id value of /sys/class/net/<iface>. |
| node_network_iface_link                             | /sys/class/net/<iface>   iface_link值                        | #   HELP node_network_iface_link iface_link value of   /sys/class/net/<iface>. |
| node_network_iface_link_mode                        | /sys/class/net/<iface>   iface_link_mode值                   | #   HELP node_network_iface_link_mode iface_link_mode value of   /sys/class/net/<iface>. |
| node_network_info                                   | 来自/sys/class/net/<iface>的非数字数据，值始终为1            | #   HELP node_network_info Non-numeric data from /sys/class/net/<iface>,   value is always 1. |
| node_network_mtu_bytes                              | /sys/class/net/<iface>   mtu_bytes值                         | #   HELP node_network_mtu_bytes mtu_bytes value of /sys/class/net/<iface>. |
| node_network_net_dev_group                          | /sys/class/net/<iface>   net_dev_group值                     | #   HELP node_network_net_dev_group net_dev_group value of   /sys/class/net/<iface>. |
| node_network_protocol_type                          | /sys/class/net/<iface>   protocol_type值                     | #   HELP node_network_protocol_type protocol_type value of   /sys/class/net/<iface>. |
| node_network_receive_bytes_total                    | 网络设备统计信息receive_bytes                                | #   HELP node_network_receive_bytes_total Network device statistic receive_bytes. |
| node_network_receive_compressed_total               | 网络设备统计信息receive_compressed                           | #   HELP node_network_receive_compressed_total Network device statistic   receive_compressed. |
| node_network_receive_drop_total                     | 网络设备统计信息receive_drop                                 | #   HELP node_network_receive_drop_total Network device statistic receive_drop. |
| node_network_receive_errs_total                     | 网络设备统计信息receive_errs                                 | #   HELP node_network_receive_errs_total Network device statistic receive_errs. |
| node_network_receive_fifo_total                     | 网络设备统计信息receive_fifo                                 | #   HELP node_network_receive_fifo_total Network device statistic receive_fifo. |
| node_network_receive_frame_total                    | 网络设备统计信息receive_frame                                | #   HELP node_network_receive_frame_total Network device statistic receive_frame. |
| node_network_receive_multicast_total                | 网络设备统计信息receive_multicast                            | #   HELP node_network_receive_multicast_total Network device statistic   receive_multicast. |
| node_network_receive_packets_total                  | 网络设备统计信息receive_packets                              | #   HELP node_network_receive_packets_total Network device statistic   receive_packets. |
| node_network_speed_bytes                            | /sys/class/net/<iface>   speed_bytes值                       | #   HELP node_network_speed_bytes speed_bytes value of   /sys/class/net/<iface>. |
| node_network_transmit_bytes_total                   | 网络设备统计信息transmit_bytes                               | #   HELP node_network_transmit_bytes_total Network device statistic   transmit_bytes. |
| node_network_transmit_carrier_total                 | 网络设备统计信息transmit_carrier                             | #   HELP node_network_transmit_carrier_total Network device statistic   transmit_carrier. |
| node_network_transmit_colls_total                   | 网络设备统计信息transmit_colls                               | #   HELP node_network_transmit_colls_total Network device statistic   transmit_colls. |
| node_network_transmit_compressed_total              | 网络设备统计信息transmit_compressed                          | #   HELP node_network_transmit_compressed_total Network device statistic   transmit_compressed. |
| node_network_transmit_drop_total                    | 网络设备统计信息transmit_drop                                | #   HELP node_network_transmit_drop_total Network device statistic transmit_drop. |
| node_network_transmit_errs_total                    | 网络设备统计信息transmit_errs                                | #   HELP node_network_transmit_errs_total Network device statistic transmit_errs. |
| node_network_transmit_fifo_total                    | 网络设备统计信息transmit_fifo                                | #   HELP node_network_transmit_fifo_total Network device statistic transmit_fifo. |
| node_network_transmit_packets_total                 | 网络设备统计信息transmit_packets                             | #   HELP node_network_transmit_packets_total Network device statistic   transmit_packets. |
| node_network_transmit_queue_length                  | /sys/class/net/<iface>   transmit_queue_length值             | #   HELP node_network_transmit_queue_length transmit_queue_length value of   /sys/class/net/<iface>. |
| node_network_up                                     | 如果operstate为'up'，则值为1，否则为0                        | #   HELP node_network_up Value is 1 if operstate is 'up', 0 otherwise. |
| node_procs_blocked                                  | 阻塞等待I/O完成的进程数                                      | #   HELP node_procs_blocked Number of processes blocked waiting for I/O to   complete. |
| node_procs_running                                  | 处于可运行状态的进程数                                       | #   HELP node_procs_running Number of processes in runnable state. |
| node_scrape_collector_duration_seconds              | node_exporter：collector scrape持续时间                      | #   HELP node_scrape_collector_duration_seconds node_exporter: Duration of a   collector scrape. |
| node_scrape_collector_success                       | node_exporter：collector 是否成功                            | #   HELP node_scrape_collector_success node_exporter: Whether a collector   succeeded. |
| node_sockstat_FRAG_inuse                            | state   inuse中的FRAG sockets数量                            | #   HELP node_sockstat_FRAG_inuse Number of FRAG sockets in state inuse. |
| node_sockstat_FRAG_memory                           | state   memory（状态存储器）中的FRAG sockets数量             | #   HELP node_sockstat_FRAG_memory Number of FRAG sockets in state memory. |
| node_sockstat_RAW_inuse                             | state   inuse中的RAW sockets数                               | #   HELP node_sockstat_RAW_inuse Number of RAW sockets in state inuse. |
| node_sockstat_TCP_alloc                             | state   alloc中的TCP sockets数                               | #   HELP node_sockstat_TCP_alloc Number of TCP sockets in state alloc. |
| node_sockstat_TCP_inuse                             | state   inuse中的TCP sockets数                               | #   HELP node_sockstat_TCP_inuse Number of TCP sockets in state inuse. |
| node_sockstat_TCP_mem                               | state   mem中的TCP sockets数                                 | #   HELP node_sockstat_TCP_mem Number of TCP sockets in state mem. |
| node_sockstat_TCP_mem_bytes                         | state   mem_bytes中的TCP sockets数                           | #   HELP node_sockstat_TCP_mem_bytes Number of TCP sockets in state mem_bytes. |
| node_sockstat_TCP_orphan                            | state   orphan中的TCP sockets数                              | #   HELP node_sockstat_TCP_orphan Number of TCP sockets in state orphan. |
| node_sockstat_TCP_tw                                | state   tw中的TCP sockets数                                  | #   HELP node_sockstat_TCP_tw Number of TCP sockets in state tw. |
| node_sockstat_UDPLITE_inuse                         | state   inuse中的UDPLITE UDP sockets数                       | #   HELP node_sockstat_UDPLITE_inuse Number of UDPLITE sockets in state inuse. |
| node_sockstat_UDP_inuse                             | state   inuse中的UDP sockets数                               | #   HELP node_sockstat_UDP_inuse Number of UDP sockets in state inuse. |
| node_sockstat_UDP_mem                               | state   mem中的UDP sockets数                                 | #   HELP node_sockstat_UDP_mem Number of UDP sockets in state mem. |
| node_sockstat_UDP_mem_bytes                         | state   mem_bytes中的UDP sockets数                           | #   HELP node_sockstat_UDP_mem_bytes Number of UDP sockets in state mem_bytes. |
| node_sockstat_sockets_used                          | state   used中的sockets sockets数                            | #   HELP node_sockstat_sockets_used Number of sockets sockets in state used. |
| node_textfile_scrape_error                          | 如果打开或读取文件时出错时为1，否则为0                       | #   HELP node_textfile_scrape_error 1 if there was an error opening or reading a   file, 0 otherwise |
| node_time_seconds                                   | 以纪元（1970年）开始的系统时间                               | #   HELP node_time_seconds System time in seconds since epoch (1970). |
| node_timex_estimated_error_seconds                  | 估计误差（秒）                                               | #   HELP node_timex_estimated_error_seconds Estimated error in seconds. |
| node_timex_frequency_adjustment_ratio               | 本地时钟频率调整                                             | #   HELP node_timex_frequency_adjustment_ratio Local clock frequency adjustment. |
| node_timex_loop_time_constant                       | 锁相回路时间常数                                             | #   HELP node_timex_loop_time_constant Phase-locked loop time constant. |
| node_timex_maxerror_seconds                         | 最大误差（秒）                                               | #   HELP node_timex_maxerror_seconds Maximum error in seconds. |
| node_timex_offset_seconds                           | 本地系统和参考时钟之间的时间偏移                             | #   HELP node_timex_offset_seconds Time offset in between local system and   reference clock. |
| node_timex_pps_calibration_total                    | 每秒脉冲校准间隔计数                                         | #   HELP node_timex_pps_calibration_total Pulse per second count of calibration   intervals. |
| node_timex_pps_error_total                          | 每秒脉冲校准错误计数                                         | #   HELP node_timex_pps_error_total Pulse per second count of calibration errors. |
| node_timex_pps_frequency_hertz                      | 脉冲每秒频率                                                 | #   HELP node_timex_pps_frequency_hertz Pulse per second frequency. |
| node_timex_pps_jitter_seconds                       | 脉冲每秒抖动                                                 | #   HELP node_timex_pps_jitter_seconds Pulse per second jitter. |
| node_timex_pps_jitter_total                         | 每秒脉冲数超出抖动限制的事件数                               | #   HELP node_timex_pps_jitter_total Pulse per second count of jitter limit   exceeded events. |
| node_timex_pps_shift_seconds                        | 每秒脉冲数超出稳定极限的事件数                               | #   HELP node_timex_pps_shift_seconds Pulse per second interval duration. |
| node_timex_pps_stability_exceeded_total             | 每秒脉冲数超出稳定极限事件                                   | #   HELP node_timex_pps_stability_exceeded_total Pulse per second count of   stability limit exceeded events. |
| node_timex_pps_stability_hertz                      | 脉冲每秒稳定性，最近频率变化的平均值                         | #   HELP node_timex_pps_stability_hertz Pulse per second stability, average of   recent frequency changes. |
| node_timex_status                                   | status   array bits的值                                      | #   HELP node_timex_status Value of the status array bits.   |
| node_timex_sync_status                              | 时钟与可靠服务器同步(1   = yes, 0 = no)                      | #   HELP node_timex_sync_status Is clock synchronized to a reliable server (1 =   yes, 0 = no). |
| node_timex_tai_offset_seconds                       | 国际原子时间（TAI）偏移量                                    | #   HELP node_timex_tai_offset_seconds International Atomic Time (TAI) offset. |
| node_timex_tick_seconds                             | 时钟周期之间的秒数                                           | #   HELP node_timex_tick_seconds Seconds between clock ticks. |
| node_uname_info                                     | 由uname系统调用提供的标记系统信息                            | #   HELP node_uname_info Labeled system information as provided by the uname   system call. |
| node_vmstat_pgfault                                 | /proc/vmstat信息字段pgfault                                  | #   HELP node_vmstat_pgfault /proc/vmstat information field pgfault. |
| node_vmstat_pgmajfault                              | /proc/vmstat信息字段pgmajfault                               | #   HELP node_vmstat_pgmajfault /proc/vmstat information field pgmajfault. |
| node_vmstat_pgpgin                                  | /proc/vmstat信息字段pgpgin                                   | #   HELP node_vmstat_pgpgin /proc/vmstat information field pgpgin. |
| node_vmstat_pgpgout                                 | /proc/vmstat信息字段pgpgout                                  | #   HELP node_vmstat_pgpgout /proc/vmstat information field pgpgout. |
| node_vmstat_pswpin                                  | /proc/vmstat信息字段pswpin                                   | #   HELP node_vmstat_pswpin /proc/vmstat information field pswpin. |
| node_vmstat_pswpout                                 | /proc/vmstat信息字段pswpout                                  | #   HELP node_vmstat_pswpout /proc/vmstat information field pswpout. |
| node_xfs_allocation_btree_compares_total            | 文件系统的分配B-tree比较数                                   | #   HELP node_xfs_allocation_btree_compares_total Number of allocation B-tree   compares for a filesystem. |
| node_xfs_allocation_btree_lookups_total             | 文件系统的分配B-tree查找数                                   | #   HELP node_xfs_allocation_btree_lookups_total Number of allocation B-tree   lookups for a filesystem. |
| node_xfs_allocation_btree_records_deleted_total     | 为文件系统删除的分配B-tree记录数                             | #   HELP node_xfs_allocation_btree_records_deleted_total Number of allocation   B-tree records deleted for a filesystem. |
| node_xfs_allocation_btree_records_inserted_total    | 为文件系统插入的分配B-tree记录数                             | #   HELP node_xfs_allocation_btree_records_inserted_total Number of allocation   B-tree records inserted for a filesystem. |
| node_xfs_block_map_btree_compares_total             | 对于文件系统block映射B-tree比较的数量                        | #   HELP node_xfs_block_map_btree_compares_total Number of block map B-tree   compares for a filesystem. |
| node_xfs_block_map_btree_lookups_total              | 对于文件系统block映射B-tree查找的数量                        | #   HELP node_xfs_block_map_btree_lookups_total Number of block map B-tree   lookups for a filesystem. |
| node_xfs_block_map_btree_records_deleted_total      | 文件系统删除block映射B-tree记录的数量                        | #   HELP node_xfs_block_map_btree_records_deleted_total Number of block map   B-tree records deleted for a filesystem. |
| node_xfs_block_map_btree_records_inserted_total     | 文件系统插入block映射B-tree记录的数量                        | #   HELP node_xfs_block_map_btree_records_inserted_total Number of block map   B-tree records inserted for a filesystem. |
| node_xfs_block_mapping_extent_list_compares_total   | 文件系统的extent列表比较数                                   | #   HELP node_xfs_block_mapping_extent_list_compares_total Number of extent list   compares for a filesystem. |
| node_xfs_block_mapping_extent_list_deletions_total  | 文件系统的extent列表删除数                                   | #   HELP node_xfs_block_mapping_extent_list_deletions_total Number of extent list   deletions for a filesystem. |
| node_xfs_block_mapping_extent_list_insertions_total | 文件系统的extent列表插入数                                   | #   HELP node_xfs_block_mapping_extent_list_insertions_total Number of extent   list insertions for a filesystem. |
| node_xfs_block_mapping_extent_list_lookups_total    | 文件系统的extent列表查找次数                                 | #   HELP node_xfs_block_mapping_extent_list_lookups_total Number of extent list   lookups for a filesystem. |
| node_xfs_block_mapping_reads_total                  | 文件系统读取操作的block映射数                                | #   HELP node_xfs_block_mapping_reads_total Number of block map for read   operations for a filesystem. |
| node_xfs_block_mapping_unmaps_total                 | 文件系统的block取消映射（删除）的数量                        | #   HELP node_xfs_block_mapping_unmaps_total Number of block unmaps (deletes) for   a filesystem. |
| node_xfs_block_mapping_writes_total                 | 文件系统写操作的block映射数                                  | #   HELP node_xfs_block_mapping_writes_total Number of block map for write   operations for a filesystem. |
| node_xfs_extent_allocation_blocks_allocated_total   | 文件系统分配的block数                                        | #   HELP node_xfs_extent_allocation_blocks_allocated_total Number of blocks   allocated for a filesystem. |
| node_xfs_extent_allocation_blocks_freed_total       | 文件系统释放的block数                                        | #   HELP node_xfs_extent_allocation_blocks_freed_total Number of blocks freed for   a filesystem. |
| node_xfs_extent_allocation_extents_allocated_total  | 为文件系统分配的extent数      在电脑文件系统中，一个 Extent (在中国大陆某些文献中翻译为“区块”[1])，是指一段连续的存储空间。一般来说，一个文件的物理大小一定是一个   extent 容量的整数倍。当一个进程创建一个文件的时候，文件系统管理程序会将整个 extent 分配给这个文件。当再次向该文件写入数据时   (有可能是在其他写入操作之后)，数据会从上次写入的数据末尾处追加数据。这样可以减少甚至消除文件碎片。      参考：http://www.wikiwand.com/zh-sg/Extent_(%E6%AA%94%E6%A1%88%E7%B3%BB%E7%B5%B1) | #   HELP node_xfs_extent_allocation_extents_allocated_total Number of extents   allocated for a filesystem. |
| node_xfs_extent_allocation_extents_freed_total      | 文件系统释放的extent数                                       | #   HELP node_xfs_extent_allocation_extents_freed_total Number of extents freed   for a filesystem. |