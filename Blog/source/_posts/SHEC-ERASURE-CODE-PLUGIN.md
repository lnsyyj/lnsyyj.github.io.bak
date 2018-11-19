---
title: SHEC ERASURE CODE PLUGIN
date: 2018-05-18 14:56:32
tags: CEPH-Luminous
---

shec plugin封装了[multiple SHEC](http://tracker.ceph.com/projects/ceph/wiki/Shingled_Erasure_Code_(SHEC))库。它允许ceph比Reed Solomon codes更有效地恢复数据。

## CREATE AN SHEC PROFILE

创建一个新的shec erasure code profile：

````
ceph osd erasure-code-profile set {name} \
     plugin=shec \
     [k={data-chunks}] \
     [m={coding-chunks}] \
     [c={durability-estimator}] \
     [crush-root={root}] \
     [crush-failure-domain={bucket-type}] \
     [crush-device-class={device-class}] \
     [directory={directory}] \
     [--force]
````

其中：

k={data-chunks}

````
Description:	每个object都分为多个data-chunks parts，每个part存储在不同的OSD上。
Type:	Integer
Required:	No.
Default:	4
````

m={coding-chunks}

````
Description:	计算每个object的coding chunks并将它们存储在不同的OSD上。coding chunks的数量也是在不丢失数据的情况下，允许损失OSD的数量。
Type:	Integer
Required:	No.
Default:	3
````

c={durability-estimator}

````
Description:	校验chunk的数量，每个data chunk都包含在计算范围内。该数字被用作耐久性估算。例如，如果c = 2，则可以在不丢失数据的情况下down掉2个OSD。
The number of parity chunks each of which includes each data chunk in its calculation range. The number is used as a durability estimator. For instance, if c=2, 2 OSDs can be down without losing data.
Type:	Integer
Required:	No.
Default:	2
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

## BRIEF DESCRIPTION OF SHEC’S LAYOUTS

### SPACE EFFICIENCY

Space efficiency是data chunks与object中所有chunks的比率，表示为k /（k + m）。为了提高space efficiency，您应该增加k或减少m。

````
space efficiency of SHEC(4,3,2) = 4/(4+3) = 0.57
SHEC(5,3,2) or SHEC(4,2,2) improves SHEC(4,3,2)'s space efficiency
````

### DURABILITY

SHEC（= c）的第三个参数是一个耐久性估算，它近似于在不丢失数据的情况下可以丢失OSD的数量。SHEC（4,3,2）的耐久性估算= 2

### RECOVERY EFFICIENCY

描述recovery efficiency的计算超出了本文件的范围，但至少在不增加c的情况下增加m可以提高恢复效率。（但是，在这种情况下，我们必须牺牲space efficiency。）

SHEC（4,2,2）→SHEC（4,3,2）：提高了recovery efficiency

## ERASURE CODE PROFILE EXAMPLES

````
$ ceph osd erasure-code-profile set SHECprofile \
     plugin=shec \
     k=8 m=4 c=3 \
     crush-failure-domain=host
$ ceph osd pool create shecpool 256 256 erasure SHECprofile
````