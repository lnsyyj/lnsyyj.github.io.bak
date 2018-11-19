---
title: ISA ERASURE CODE PLUGIN
date: 2018-05-16 16:57:03
tags: CEPH-Luminous
---

ISA plugin封装了[ISA](https://01.org/intel%C2%AE-storage-acceleration-library-open-source-version/)库。 它只能在Intel处理器上运行。

## CREATE AN ISA PROFILE

创建新的isa erasure code profile：

````
ceph osd erasure-code-profile set {name} \
     plugin=isa \
     technique={reed_sol_van|cauchy} \
     [k={data-chunks}] \
     [m={coding-chunks}] \
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
Required:	No.
Default:	7
````

m={coding-chunks}

````
Description:	计算每个object的coding chunks并将它们存储在不同的OSD上。coding chunks的数量也是在不丢失数据的情况下，允许损失OSD的数量。
Type:	Integer
Required:	No.
Default:	3
````

technique={reed_sol_van|cauchy}

````
Description:	ISA插件有两种Reed Solomon形式（https://en.wikipedia.org/wiki/Reed%E2%80%93Solomon_error_correction）。
如果设置了reed_sol_van，则为Vandermonde，
如果设置了cauchy（https://en.wikipedia.org/wiki/Vandermonde_matrix），则为Cauchy（https://en.wikipedia.org/wiki/Cauchy_matrix）。
Type:	String
Required:	No.
Default:	reed_sol_van
````

crush-root={root}

````
Description:	crush bucket名字用于CRUSH rule的first step。"take"为step的默认值。
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
Default:	
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
Description:	用相同的名称覆盖已有的profile。
Type:	String
Required:	No.
````