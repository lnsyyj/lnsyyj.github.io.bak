---
title: JERASURE ERASURE CODE PLUGIN
date: 2018-05-16 11:17:03
tags: CEPH-Luminous
---

jerasure plugin是最通用和灵活的插件，它也是Ceph erasure coded pools的默认设置。

jerasure plugin封装了[Jerasure](http://jerasure.org/)库。建议阅读Jelasure文档以更好地理解参数。

## CREATE A JERASURE PROFILE

创建新的jerasure erasure code profile：

````
ceph osd erasure-code-profile set {name} \
     plugin=jerasure \
     k={data-chunks} \
     m={coding-chunks} \
     technique={reed_sol_van|reed_sol_r6_op|cauchy_orig|cauchy_good|liberation|blaum_roth|liber8tion} \
     [crush-root={root}] \
     [crush-failure-domain={bucket-type}] \
     [crush-device-class={device-class}] \
     [directory={directory}] \
     [--force]
````

其中：

k={data chunks}

````
Description:	每个object都分为多个data-chunks parts，每个part存储在不同的OSD上。
Type:	Integer
Required:	Yes.
Example:	4
````

m={coding-chunks}

````
Description:	计算每个object的coding chunks并将它们存储在不同的OSD上。coding chunks的数量也是在不丢失数据的情况下，允许损失OSD的数量。
Type:	Integer
Required:	Yes.
Example:	2
````

technique={reed_sol_van|reed_sol_r6_op|cauchy_orig|cauchy_good|liberation|blaum_roth|liber8tion}

````
Description:	更灵活的technique是reed_sol_van：只需要设置k和m。cauchy_good technique可以更快但您需要调优packetsize。reed_sol_r6_op，liberation，blaum_roth，liber8tion都与RAID6等价，因为它们只能配置m=2。
Type:	String
Required:	No.
Default:	reed_sol_van
````

packetsize={bytes}

````
Description:	encoding将在一次bytes大小的packets上完成。选择合理的packet大小是困难的。Jeasure文档包含了关于这个topic的大量信息。
Type:	Integer
Required:	No.
Default:	2048
````

crush-root={root}

````
Description:	crush bucket名字用于CRUSH rule的first step。"take"为step的默认值。
The name of the crush bucket used for the first step of the CRUSH rule. For intance step take default.
Type:	String
Required:	No.
Default:	default
````

crush-failure-domain={bucket-type}

````
Description:	确保不要有两个chunks位于相同故障域的bucket中。例如，如果故障域是host，则不会在同一主机上存储两个chunks。它用于创建CRUSH rule step，比如step chooseleaf host。
Type:	String
Required:	No.
Default:	host
````

crush-device-class={device-class}

````
Description:	使用CRUSH map中的crush device class名称，限定存储到特定class devices（例如SSD或HDD）。
Type:	String
Required:	No.
````

directory={directory}

````
Description:	设置加载erasure code plugin的目录名。
Type:	String
Required:	No.
Default:	/usr/lib/ceph/erasure-code
````

--force

````
Description:	以相同的名称重写已经存在的profile。
Type:	String
Required:	No.
````