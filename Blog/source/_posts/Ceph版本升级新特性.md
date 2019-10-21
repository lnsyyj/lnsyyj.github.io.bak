---
title: Ceph版本升级新特性
date: 2019-10-16 22:13:29
tags: ceph
---



# V14.2.0 NAUTILUS MAJOR CHANGES FROM MIMIC

参考：

【1】https://docs.ceph.com/docs/master/releases/nautilus/

【2】https://blog.csdn.net/Z_Stand/article/details/89739908

这是Ceph Nautilus的第一个稳定版本。

- Dashboard

  [Ceph Dashboard](https://docs.ceph.com/docs/master/mgr/dashboard/#mgr-dashboard)增加了许多新功能：

  - Support for multiple users / roles
  - SSO (SAMLv2) for user authentication
  - Auditing support（审计支持）
  - New landing page, showing more metrics and health info
  - I18N support（国际化）
  - REST API documentation with Swagger API
  - Swagger 是一个规范和完整的框架，用于生成、描述、调用和可视化 RESTful 风格的 Web 服务。总体目标是使客户端和文件系统作为服务器以同样的速度来更新。文件的方法，参数和模型紧密集成到服务器端的代码，允许API来始终保持同步。作者：天马行空LQ
      链接：https://www.jianshu.com/p/66a14ea07622
    来源：简书
      著作权归作者所有。商业转载请联系作者获得授权，非商业转载请注明出处。
  
  Ceph management新功能：
  
  - OSD management (mark as down/out, change OSD settings, recovery profiles)
  - Cluster config settings editor
  - Ceph Pool management (create/modify/delete)
  - [ECP](https://www.oschina.net/news/2609) management
  - RBD mirroring configuration
  - Embedded Grafana Dashboards (derived from Ceph Metrics)
  - CRUSH map viewer
  - NFS Ganesha management
  - iSCSI target management (via [Ceph iSCSI Gateway](https://docs.ceph.com/docs/master/rbd/iscsi-overview/#ceph-iscsi))
  - RBD QoS configuration
  - Ceph Manager (ceph-mgr) module management
  - Prometheus alert Management

而且，Ceph Dashboard现在被拆分到ceph-mgr-dashboard的package。 如果您的package management software在安装ceph-mgr时失败，则可能需要单独安装ceph-mgr-dashboard。

- RADOS
  - 现在可以随时减少每个pool的placement groups (PGs)数，并且可以根据群集利用率或管理员提示[自动调整PG数](https://docs.ceph.com/docs/master/rados/operations/placement-groups/#pg-autoscaler)。
  - 新的[v2 wire protocol](https://docs.ceph.com/docs/master/rados/configuration/msgr2/#msgr2)支持线路加密
  - 群集可以跟踪OSD和Monitor daemons消耗的物理[存储设备](https://docs.ceph.com/docs/master/rados/operations/devices/#devices)以及运行状况指标（即SMART），并且群集可以通过预先训练的预测模型或者基于云预测服务来[报告预测的HDD或SSD故障](https://docs.ceph.com/docs/master/mgr/diskprediction/#diskprediction)。
  - 可通过ceph osd numa-status命令轻松监视OSD daemons的NUMA节点，并通过osd_numa_node config选项进行配置。
  - 现在，当使用BlueStore OSD时，空间利用率将按object data，omap data和internal metadata，pool以及压缩前和压缩后的大小进行细分。
  - 在执行recovery和backfill时，OSD可以更有效地选择重要的PG和objects优先处理。
  - 在设备出现问题以后，像recovery这种长期运行在后台的进程，可以使用ceph status命令查看进度。
  - 增加了一个实验性的（耦合层） [Coupled-Layer “Clay” erasure code](https://www.usenix.org/conference/fast18/presentation/vajha) plugin，该plugin可减少大多数recovery操作所需的网络带宽和IO。
- RGW
  - S3 lifecycle可以在storage classes与tiering层之间转换
  - Beast取代了civetweb成为默认的web frontend，从而提高了整体性能。
  - 支持新的publish/subscribe基础架构，允许RGW将events推送至serverless frameworks如knative或data pipelies如Kafka。
  - 新增一系列身份验证功能，使用OAuth2和OpenID::connect的STS联合以及OPA（开放策略代理）身份验证委派原型。
  - 新的archive zone federation功能可将所有objects（包括历史记录）完全保留在一个单独的zone中。
- CephFS
  - 对于具有large caches和large RAM并长期运行的客户端，MDS的稳定性已大大提高。Cache trimming（调整）和client capability recall现在受到限制，以防止MDS过载。
  - 现在可以在Rook管理的群集环境中通过NFS-Ganesha导出CephFS。Ceph负责管理集群并确保高可用性和可伸缩性。 [入门演示](https://ceph.com/community/deploying-a-cephnfs-server-cluster-with-rook/)。 预计在未来Nautilus的次要版本中实现此功能的更多自动化。
  - MDS mds_standby_for_*，mon_force_standby_active和mds_standby_replay配置选项已过时。 相反，操作者现在可以在CephFS文件系统上[设置新的](https://docs.ceph.com/docs/master/cephfs/standby/#mds-standby-replay)allow_standby_replay标志。 该设置会使文件系统由standbys变为standby-replay，并且任何可用的rank都会生效。（一个 rank 可看作是一个元数据分片）
  - MDS支持客户端释放缓存的同时释放自己的存储端缓存，这个过程可由MDS admin socket命令 `cache drop`来完成
  - 现在可以检查MDS中正在进行的scrub的进度。 此外，scrub可能会暂停或中止。 有关更多信息，请参见[scrub文档](https://docs.ceph.com/docs/master/cephfs/scrub/#mds-scrub)。
  - 通过`ceph volume` command-line-interface提供了一个用于创建volumes的新interface。
  - 新的[cephfs-shell工具](https://docs.ceph.com/docs/master/cephfs/cephfs-shell/)可用于处理CephFS文件系统而无需mounting。
  - 为了简洁，清楚和有用，已重新格式化了来自ceph status与CephFS相关的输出。
  - Lazy IO已进行了改进。客户端可以使用ceph_open C/C++ API的新CEPH_O_LAZY flag将其打开或通过配置选项client_force_lazyio将其打开。（LazyIO放松了POSIX语义。 即使文件由多个客户端上的多个应用程序打开，也允许缓冲的读/写操作。 应用程序负责自己管理缓存的一致性。自Nautilus发行以来，Libcephfs支持LazyIO。）
  - 现在可以通过ceph fs fail命令快速关闭CephFS文件系统。有关更多信息，请参见 [the administration page](https://docs.ceph.com/docs/master/cephfs/administration/#cephfs-administration)。

- RBD
  - Images可以在尽量短的停机时间内迁移，以帮助在pool之间移动images或移动到新的layouts。
  - 新的rbd perf image iotop和rbd perf image iostat命令为所有RBD images提供了类似于iotop和iostat的IO监视器。
  - 现在，ceph-mgr Prometheus exporter新增一个用于所有RBD images的IO监视器。
  - 支持pool中的单独image namespaces，以便进行租户隔离。
- Misc
  - Ceph 拥有一组新的[orchestrator modules](https://docs.ceph.com/docs/master/mgr/orchestrator_cli/#orchestrator-cli-module)，可直接与外部orchestrators像ceph-ansible, DeepSea, Rook, or simply ssh通过一致的CLI interface(and, eventually, Dashboard) 进行交互。

# V13.2.0 MIMIC MAJOR CHANGES FROM LUMINOUS

- Dashboard
  - Ceph Luminous中引入的（只读）Ceph manager dashboard已由[openATTIC](https://openattic.org/) Ceph management tool新实现取代，提供了具有[许多其他管理功能](https://docs.ceph.com/docs/master/mgr/dashboard/#mgr-dashboard)。
- RADOS
  - 现在，配置选项可以由monitor集中存储和管理。
  - 进行recovery或rebalancing操作时，monitor daemon占用的disk space大大减少。
  - 当OSD从最近的故障中恢复时，异步恢复功能可减少请求的tail latency（少量响应的延迟高于均值，我们把这些响应称为尾延迟）。
  - scrub时OSD冲突请求抢占tail latency减少。
- RGW
  - RGW可以将zone （或subset of buckets）复制到外部云存储服务（例如S3）。
  - RGW在versioned buckets功能上支持S3 multi-factor authentication API。
    - AWS Multi-Factor Authentication（MFA）它在用户名和密码的基础上增加了一层额外的保护。启用MFA后，当用户登录AWS网站时，系统将提示他们输入用户名和密码以及来自其AWS MFA设备的身份验证代码。这些因素综合在一起，为您的AWS账户设置和资源提供了更高的安全性。
  - 版本控制是在相同的存储桶中保留对象的多个变量的方法。对于 Amazon S3 桶中存储的每个对象，您可以使用版本控制功能来保存、检索和还原它们的各个版本。使用版本控制能够轻松从用户意外操作和应用程序故障中恢复数据。
  - Beast frontend不再进行实验，被认为是稳定的并可以使用。
  
- CephFS
  - Snapshots与多个MDS daemons结合使用时，现在稳定。

- RBD
  - Image clones不再需要明确的protect和unprotect步骤。
  - 可以将Images deep-copied（包括与parent image和associated snapshots的任何克隆链接）到新pool或修改data layouts。

- Misc
  - 由于在Stretch中缺少GCC 8，我们已删除了Mimic的Debian构建。我们希望Debian的构建将在2019年初发布Buster后回归，并希望在Buster可用后构建最终的Luminous发行版（以及以后的Mimic point发行版）。