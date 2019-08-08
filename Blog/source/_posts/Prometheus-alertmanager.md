---
title: Prometheus alertmanager
date: 2019-08-08 16:38:21
tags: prometheus
---

最近在研究Ceph的监控系统，Ceph 14.2.1的MGR中已经有Prometheus Plugin，可以替代原来的[ceph_exporter](https://github.com/digitalocean/ceph_exporter)项目，作为新的exporter。MGR中的Prometheus Plugin可以对Ceph的一些状态进行监控（大概390+个监控指标），基本涵盖了能想到的还有想不到的监控指标，打开了认识Ceph的另一个窗口。enable起这个Plugin并不是难点，难点在怎样从中捞取有价值的数据，这是需要长期积累持续挖掘的问题，你懂得。。。。。。说了一堆XXX，这里只做一个prometheus与alertmanager集成的入门实验。

prometheus与alertmanager是相互独立的组件，我们先来说一下它们的职责：

- prometheus端负责产生告警，并发送到alertmanager端

  需要配置alertmanager服务在哪、告警产生的条件（alert rule）

- alertmanager端负责接收prometheus发来的告警，然后做一些后续处理（比如把告警信息发出到email、微信、钉钉......）

  需要配置route、receivers

![](https://github.com/prometheus/alertmanager/raw/master/doc/arch.svg?sanitize=true)

假设有这样一个场景：Prometheus已经收集到了Ceph的监控指标，如果集群中有OSD down超过1小时，那么我们需要发送邮件给相关人员，告知需要处理。





















