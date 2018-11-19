---
title: CEPH MANAGER DAEMON - Local pool plugin
date: 2017-11-26 19:13:12
tags: CEPH-MGR
---

localpool插件可以自动创建本地化为整个集群子集的RADOS pool。例如，默认情况下，它将为集群中的每个不同rack创建一个pool。对于某些希望在本地以及整个集群内分布一些数据的部署可能非常有用。
（个人理解这里的本地有可能是不同RULE下的pool，不同RULE下是否同名pool没有验证是否允许）

**ENABLING**

localpool模块启用：

    ceph mgr module enable localpool

**CONFIGURING**

localpool模块可以支持以下选项：



- subtree（default: rack）: 为哪个CRUSH subtree type模块创建一个池


- failure_domain (default: host) : 我们应该在哪个故障域中分隔数据副本


- pg_num (default: 128) : 为每个pool创建PG的数量


- num_rep (default: 3) : 每个pool的副本数。（目前，pool只支持replicated模式）


- min_size (default: none) : 设置min_size值（如果没有设置此选项，则不会改变Ceph的默认值）


- prefix (default: by-$subtreetype-) : pool名的prefix（前缀）

通过config-key接口设置这些选项。例如，设置复制2副本，64PG

    ceph config-key set mgr/localpool/num_rep 2
    ceph config-key set mgr/localpool/pg_num 64
