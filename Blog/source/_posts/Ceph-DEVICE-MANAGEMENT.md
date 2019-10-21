---
title: Ceph DEVICE MANAGEMENT
date: 2019-10-21 14:18:11
tags: ceph
---

### DEVICE MANAGEMENT

Ceph跟踪哪个daemons消耗了哪些hardware storage devices（例如HDD，SSD），并收集有关这些devices的运行状况指标，以提供预测和/或自动响应硬件故障的工具。

# DEVICE TRACKING

您可以查询哪些存储设备正在使用：

```
ceph device ls

DEVICE               HOST:DEV                       DAEMONS LIFE EXPECTANCY 
0d2a946c-413f-43f4-b community-ceph-2.novalocal:vdc osd.0
451e48d6-913e-4f93-a community-ceph-1.novalocal:vdd osd.5
935b6018-1dfe-4cf9-8 community-ceph-1.novalocal:vdc osd.2
abe09d21-d950-47b0-9 community-ceph-2.novalocal:vdd osd.3
bf37729e-9d83-48e9-9 community-ceph-3.novalocal:vdc osd.1
d48dcf29-fe58-4e3e-a community-ceph-3.novalocal:vdd osd.4
```

您还可以按daemon或host列出devices：

```
ceph device ls-by-daemon <daemon>
ceph device ls-by-host <host>

[root@community-ceph-1 ~]# ceph device ls-by-daemon osd.0
DEVICE               HOST:DEV                       EXPECTED FAILURE 
0d2a946c-413f-43f4-b community-ceph-2.novalocal:vdc                  

[root@community-ceph-1 ~]# ceph device ls-by-host community-ceph-2.novalocal
DEVICE               DEV DAEMONS EXPECTED FAILURE 
0d2a946c-413f-43f4-b vdc osd.0                    
abe09d21-d950-47b0-9 vdd osd.3                    
```

对于任何单个设备，您可以通过以下方式查询有关其位置以及如何使用它的信息：

```
ceph device info <devid>

[root@community-ceph-1 ~]# ceph device info 0d2a946c-413f-43f4-b
device 0d2a946c-413f-43f4-b
attachment community-ceph-2.novalocal:vdc
daemons osd.0
```

# ENABLING MONITORING

Ceph还可以监视与您的设备关联的健康指标。 例如，SATA硬盘实现了一个称为SMART的标准，该标准提供了有关设备使用情况和运行状况的内部指标，例如开机小时数，电源循环次数或不可恢复的读取错误。 其他设备类型（例如SAS和NVMe）实现了一组相似的指标（通过略有不同的标准）。 Ceph可以通过smartctl工具收集所有这些信息。

您可以通过以下方式启用或禁用运行状况监视：

```
ceph device monitoring on
```

或

```
ceph device monitoring off
```

# SCRAPING

如果启用了监视，则将定期自动scraped指标。 该间隔可以配置为：

```
ceph config set mgr mgr/devicehealth/scrape_frequency <seconds>
```

默认值为每24小时scrape一次。

您可以使用以下方法手动触发所有设备的scrape：

```
ceph device scrape-health-metrics
```

单个设备可以用以下方式scraped：

```
ceph device scrape-health-metrics <device-id>
```

或单个daemon的设备可以通过以下方式进行scraped：

```
ceph device scrape-daemon-health-metrics <who>
```

可以使用以下命令检索设备的存储健康指标（可选地，用于特定时间戳）：

```
ceph device get-health-metrics <devid> [sample-timestamp]
```

# FAILURE PREDICTION

Ceph可以根据其收集的健康指标预测预期寿命和设备故障。 共有三种模式：

- none: 禁用设备故障预测。
- local: 使用来自ceph-mgr daemon的预训练预测模型
- cloud: 使用ProphetStor运行的外部云服务共享设备运行状况和性能指标，并使用其免费服务或付费服务进行更准确的预测

预测模式可以配置为：

```
ceph config set global device_failure_prediction_mode <mode>
```

预测通常在后台定期进行，因此填充预期寿命值可能需要一些时间。 您可以从以下输出中看到所有设备的预期寿命：

```
ceph device ls
```

您还可以使用以下方法查询特定设备的metadata：

```
ceph device info <devid>
```

您可以使用以下命令显式地强制预测设备的预期寿命：

```
ceph device predict-life-expectancy <devid>
```

如果您未使用Ceph的内部设备故障预测，但是拥有一些有关设备故障的外部信息源，则可以通过以下方式告知Ceph设备的预期寿命：

```
ceph device set-life-expectancy <devid> <from> [<to>]
```

预期寿命以时间间隔表示，因此不确定性可以以宽间隔的形式表示。 间隔结束也可以不指定。

# HEALTH ALERTS

mgr/devicehealth/warn_threshold控制在生成运行状况警告之前，预期的设备故障必须多长时间。

可以使用以下方法检查所有设备的存储预期寿命，并生成任何适当的健康警报：

```
ceph device check-health
```

# AUTOMATIC MITIGATION

如果启用了mgr/devicehealth/self_heal选项（默认情况下），则对于预计将很快发现故障的设备，模块将通过将设备标记为“out”来自动将数据迁移到这些设备之外。

mgr/devicehealth/mark_out_threshold控制在我们将osd自动标记为“out”之前，预期的设备故障必须多长时间。



### 原文：

### https://docs.ceph.com/docs/master/rados/operations/devices/#devices