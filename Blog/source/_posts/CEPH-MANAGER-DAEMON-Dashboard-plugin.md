---
title: CEPH MANAGER DAEMON - Dashboard plugin
date: 2017-11-25 23:58:01
tags: CEPH-MGR
---

CEPH MANAGER DAEMON - Dashboard plugin - DASHBOARD PLUGIN
Dashboard 插件使用ceph-mgr托管的Web server，可以可视化集群的统计信息。

**ENABLING**
dashboard模块启用：

    ceph mgr module enable dashboard

**CONFIGURATION**

像大多数Web应用程序一样，dashboard 绑定到主机名和端口。默认情况下，ceph-mgr daemon托管的dashboard（即当前active manager）将绑定到主机上任何可用的IPv4或IPv6地址的7000端口。
由于每个ceph-mgr都拥有自己的dashboard instance，因此可能需要单独配置它们。hostname 和port 可以通过配置key facility进行更改：

	ceph config-key set mgr/dashboard/$name/server_addr $IP
	ceph config-key set mgr/dashboard/$name/server_port $PORT
其中$name是托管此dashboard Web应用程序的ceph-mgr的ID。
这些设置也可以在集群范围内配置，而不是管理员特定的。 例如：
	ceph config-key set mgr/dashboard/server_addr $IP
	ceph config-key set mgr/dashboard/server_port $PORT
如果未配置端口，则Web应用程序将绑定到7000端口。
如果未配置地址，则Web应用将绑定到::，其对应于所有可用的IPv4和IPv6地址。

您可以为所有URLs配置prefix：

	ceph config-key set mgr/dashboard/url_prefix $PREFIX

所以你可以访问dashboard http://$IP:$PORT/$PREFIX/.

**LOAD BALANCER**
请注意，dashboard 将只在当时处于active状态的管理器上启动。查询Ceph集群状态以查看哪个管理器处于active状态（例如，ceph mgr dump）。为了使仪表板可通过一致的URL访问，无论哪个管理器daemon当前处于active状态，您可能需要在前端设置一个负载平衡器，以将流量引导至任何可用的管理器endpoint。 如果你使用HTTP反向代理转发到dashboard的subpath，则需要配置url_prefix（请参见上文）。














