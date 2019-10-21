---
title: Ceph DISKPREDICTION MODULE
date: 2019-10-21 14:41:49
tags: ceph
---

### DISKPREDICTION MODULE

磁盘预测模块支持两种模式：cloud mode和local mode。 在cloud mode下，磁盘和Ceph操作状态信息是从Ceph群集中收集的，并通过Internet发送到基于云的DiskPrediction服务器。 DiskPrediction服务器分析数据，并提供Ceph群集的性能和磁盘运行状况的分析和预测结果。

local mode不需要任何外部服务器即可进行数据分析和输出结果。 在local mode下，磁盘*diskprediction* module将内部predictor module用于磁盘预测服务，然后将磁盘预测结果返回给Ceph系统。

Local predictor: 70% 的准确性

Cloud predictor for free: 95% 的准确性

# ENABLING

运行以下命令以在Ceph环境中启用磁盘预测模块：

```
ceph mgr module enable diskprediction_cloud
ceph mgr module enable diskprediction_local
```

选择预测模式：

```
ceph config set global device_failure_prediction_mode local
```

或

```
ceph config set global device_failure_prediction_mode cloud
```

要禁用预测，请执行以下操作：

```
ceph config set global device_failure_prediction_mode none
```

# CONNECTION SETTINGS

connection settings用于Ceph和DiskPrediction服务器之间的连接。

### LOCAL MODE

diskprediction module利用Ceph设备运行状况检查来收集磁盘运行状况指标，并使用内部predictor module来生成磁盘故障预测并返回Ceph。 因此，在local mode下不需要连接设置。 local predictor module至少需要六个设备健康状况数据集才能实施预测。

运行以下命令以使用本地预测变量预测设备的预期寿命。

```
ceph device predict-life-expectancy <device id>

[root@community-ceph-1 ~]# ceph device predict-life-expectancy 0d2a946c-413f-43f4-b
unknown
```

### CLOUD MODE

在cloud mode下，需要用户注册。 用户必须在https://www.diskprophet.com/#/上注册其帐户，以接收以下DiskPrediction服务器信息以进行连接设置。

Certificate file path: 确认用户注册后，系统将发送一封确认电子邮件，其中包括证书文件下载链接。 下载证书文件并将其保存到Ceph系统。 运行以下命令来验证文件。 如果没有证书文件验证，则无法完成连接设置。

DiskPrediction server: DiskPrediction服务器名称。 如果需要，它可以是IP地址。

Connection account: 用于在Ceph和DiskPrediction服务器之间建立连接的帐户名

Connection password: 用于在Ceph和DiskPrediction服务器之间建立连接的密码

运行以下命令以完成连接设置。

```
ceph device set-cloud-prediction-config <diskprediction_server> <connection_account> <connection_password> <certificate file path>
```

您可以使用以下命令显示连接设置：

```
ceph device show-prediction-config
```

其他可选配置设置如下：

diskprediction_upload_metrics_interval: 指示定期将Ceph性能指标发送到DiskPrediction服务器的频率。 默认值为10分钟。

diskprediction_upload_smart_interval: 指示定期将Ceph物理设备信息发送到DiskPrediction服务器的频率。 默认值为12小时。

diskprediction_retrieve_prediction_interval: 指示Ceph有时会定期从DiskPrediction服务器检索物理设备预测数据。 默认值为12小时。

# DISKPREDICTION DATA

diskprediction module主动向/从DiskPrediction服务器发送/检索以下数据。

### METRICS DATA

- Ceph cluster status

| key                  | Description                                                 |
| :------------------- | :---------------------------------------------------------- |
| cluster_health       | Ceph health check status                                    |
| num_mon              | Number of monitor node                                      |
| num_mon_quorum       | Number of monitors in quorum                                |
| num_osd              | Total number of OSD                                         |
| num_osd_up           | Number of OSDs that are up                                  |
| num_osd_in           | Number of OSDs that are in cluster                          |
| osd_epoch            | Current epoch of OSD map                                    |
| osd_bytes            | Total capacity of cluster in bytes                          |
| osd_bytes_used       | Number of used bytes on cluster                             |
| osd_bytes_avail      | Number of available bytes on cluster                        |
| num_pool             | Number of pools                                             |
| num_pg               | Total number of placement groups                            |
| num_pg_active_clean  | Number of placement groups in active+clean state            |
| num_pg_active        | Number of placement groups in active state                  |
| num_pg_peering       | Number of placement groups in peering state                 |
| num_object           | Total number of objects on cluster                          |
| num_object_degraded  | Number of degraded (missing replicas) objects               |
| num_object_misplaced | Number of misplaced (wrong location in the cluster) objects |
| num_object_unfound   | Number of unfound objects                                   |
| num_bytes            | Total number of bytes of all objects                        |
| num_mds_up           | Number of MDSs that are up                                  |
| num_mds_in           | Number of MDS that are in cluster                           |
| num_mds_failed       | Number of failed MDS                                        |
| mds_epoch            | Current epoch of MDS map                                    |

- Ceph mon/osd performance counts  

Mon:

| key           | Description                               |
| :------------ | :---------------------------------------- |
| num_sessions  | Current number of opened monitor sessions |
| session_add   | Number of created monitor sessions        |
| session_rm    | Number of remove_session calls in monitor |
| session_trim  | Number of trimed monitor sessions         |
| num_elections | Number of elections monitor took part in  |
| election_call | Number of elections started by monitor    |
| election_win  | Number of elections won by monitor        |
| election_lose | Number of elections lost by monitor       |

Osd:

| key                   | Description                                                  |
| :-------------------- | :----------------------------------------------------------- |
| op_wip                | Replication operations currently being processed (primary)   |
| op_in_bytes           | Client operations total write size                           |
| op_r                  | Client read operations                                       |
| op_out_bytes          | Client operations total read size                            |
| op_w                  | Client write operations                                      |
| op_latency            | Latency of client operations (including queue time)          |
| op_process_latency    | Latency of client operations (excluding queue time)          |
| op_r_latency          | Latency of read operation (including queue time)             |
| op_r_process_latency  | Latency of read operation (excluding queue time)             |
| op_w_in_bytes         | Client data written                                          |
| op_w_latency          | Latency of write operation (including queue time)            |
| op_w_process_latency  | Latency of write operation (excluding queue time)            |
| op_rw                 | Client read-modify-write operations                          |
| op_rw_in_bytes        | Client read-modify-write operations write in                 |
| op_rw_out_bytes       | Client read-modify-write operations read out                 |
| op_rw_latency         | Latency of read-modify-write operation (including queue time) |
| op_rw_process_latency | Latency of read-modify-write operation (excluding queue time) |

- Ceph pool statistics

| key        | Description                               |
| :--------- | :---------------------------------------- |
| bytes_used | Per pool bytes used                       |
| max_avail  | Max available number of bytes in the pool |
| objects    | Number of objects in the pool             |
| wr_bytes   | Number of bytes written in the pool       |
| dirty      | Number of bytes dirty in the pool         |
| rd_bytes   | Number of bytes read in the pool          |
| stored_raw | Bytes used in pool including copies made  |

- Ceph physical device metadata

| key            | Description                 |
| :------------- | :-------------------------- |
| disk_domain_id | Physical device identify id |
| disk_name      | Device attachment name      |
| disk_wwn       | Device wwn                  |
| model          | Device model name           |
| serial_number  | Device serial number        |
| size           | Device size                 |
| vendor         | Device vendor name          |

- Ceph each objects correlation information
- The module agent information
- The module agent cluster information
- The module agent host information

### SMART DATA

- Ceph physical device SMART data (provided by Ceph *devicehealth* module)

### PREDICTION DATA

-   Ceph physical device prediction data

# RECEIVING PREDICTED HEALTH STATUS FROM A CEPH OSD DISK DRIVE（从CEPH OSD磁盘驱动器中接收预期的健康状况）

您可以使用以下命令从Ceph OSD磁盘驱动器接收预测的健康状态。

```
ceph device get-predicted-status <device id>
```

get-predicted-status命令返回：

```
{
    "near_failure": "Good",
    "disk_wwn": "5000011111111111",
    "serial_number": "111111111",
    "predicted": "2018-05-30 18:33:12",
    "attachment": "sdb"
}
```

| Attribute     | Description                                                 |
| :------------ | :---------------------------------------------------------- |
| near_failure  | The disk failure prediction state: Good/Warning/Bad/Unknown |
| disk_wwn      | Disk WWN number                                             |
| serial_number | Disk serial number                                          |
| predicted     | Predicted date                                              |
| attachment    | device name on the local system                             |

磁盘故障预测状态的near_failure属性在下表中指示磁盘预期寿命。

| near_failure | Life expectancy (weeks) |
| :----------- | :---------------------- |
| Good         | > 6 weeks               |
| Warning      | 2 weeks ~ 6 weeks       |
| Bad          | < 2 weeks               |

# DEBUGGING

如果要调试DiskPrediction module映射到Ceph日志记录级别，请使用以下命令。

```
[mgr]

    debug mgr = 20
```

将日志记录设置为管理器调试时，模块将打印出带有前缀mgr [diskprediction]的日志记录消息，以便于过滤。