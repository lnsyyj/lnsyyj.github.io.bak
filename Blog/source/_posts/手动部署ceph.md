---
title: 手动部署ceph
date: 2018-11-26 18:47:23
tags: ceph
---

手动部署ceph是看懂官方ansible或ceph-deploy的关键，了解部署步骤和相关组件才能更好的理解代码，理解OSD的挂载流程，所以个人认为需要做此实验。

官方文档：http://docs.ceph.com/docs/master/install/manual-deployment/

# MANUAL DEPLOYMENT

所有Ceph集群都需要至少一个monitor，并且至少需要与集群中存储对象副本一样多的OSD。引导初始monitor(s)是部署Ceph存储集群的第一步。monitor部署还为整个集群设置了重要准则，例如pool的副本数，每个OSD的placement groups数（PG数），心跳间隔，是否需要身份验证等。这些值中的大多数都是默认设置的，因此在设置生产环境集群时，了解它们很有用。
遵循与[安装（快速）](http://docs.ceph.com/docs/master/start/)相同的配置，我们将设置一个集群，其中node1作为监控节点，node2和node3作为OSD节点

![](http://docs.ceph.com/docs/master/_images/ditaa-b67d58275cae03a5573d36907b437e36df685600.png)

## MONITOR BOOTSTRAPPING

引导monitor需要许多东西：

- Unique Identifier（唯一标识符）: fsid是集群的唯一标识符，它是 Ceph 作为文件系统时的文件系统标识符。现在Ceph也支持原生接口，块设备和对象存储网关接口，因此fsid有点不恰当的。

- Cluster Name（集群名称）: Ceph集群有一个集群名称，这是一个没有空格的字符串。默认集群名称为ceph，但您可以指定其他集群名称。当您使用多个集群并且需要清楚地了解正在使用哪个集群时，覆盖默认集群名称尤其有用。

  例如，在[multisite configuration](http://docs.ceph.com/docs/master/radosgw/multisite/#multisite)中运行多个集群时，集群名称（例如，us-west，us-east）标识当前CLI会话的集群。注意：要在CLI标识集群名称，请使用集群名称指定Ceph配置文件（例如，ceph.conf，us-west.conf，us-east.conf等）。另请参阅CLI用法（ceph --cluster {cluster-name}）。

- Monitor Name: 集群中的每个monitor实例都具有唯一的名称。通常，Ceph Monitor名称是主机名（我们建议独立一台主机运行Ceph Monitor，而不要将Ceph OSD Daemons与Ceph Monitors混合部署在一台机器上）。您可以使用hostname -s检索短主机名。

- Monitor Map: 引导初始monitor(s)需要您生成monitor map。monitor map需要fsid，cluster name，以及至少一个host name及其IP地址。

- Monitor Keyring: Monitors通过secret key相互通信。您必须生成带有monitor secret的keyring，并在引导初始monitor(s)时提供它。

- Administrator Keyring: 要使用ceph CLI tools，您必须具有client.admin用户。因此，您必须生成admin用户和相应keyring，并且还必须将client.admin用户添加到monitor keyring。

上述要求并不意味着创建Ceph配置文件。但是，作为最佳实践，我们建议创建一个Ceph配置文件，并使用fsid，mon initial members和mon host填充它。

您也可以在运行时获取并设置所有monitor settings。但是，Ceph配置文件可能只包含覆盖默认值的那些settings。将settings添加到Ceph配置文件时，这些settings会覆盖默认settings。在Ceph配置文件中维护这些settings可以使您更轻松地维护集群。

过程如下：

1、登录monitor节点：

```
ssh {hostname}
```

例如：

```
ssh node1
```

2、确保您有Ceph配置文件的目录。 默认情况下，Ceph使用/etc/ceph。 安装ceph时，安装程序将自动创建/etc/ceph目录。

```
ls /etc/ceph
```

注意：部署工具可能在清除集群时删除此目录（例如，ceph-deploy purgedata {node-name}，ceph-deploy purge {node-name}）。

3、创建Ceph配置文件。 默认情况下，Ceph使用ceph.conf，其中ceph反映了集群名称。

```
sudo vim /etc/ceph/ceph.conf
```

4、为您的集群生成唯一ID（即fsid）。

```
uuidgen
```

5、将唯一ID添加到Ceph配置文件中。

```
fsid = {UUID}
```

例如：

```
fsid = a7f64266-0894-4f1e-a635-d0aeaca0e993
```

6、将初始monitor(s)添加到Ceph配置文件中。

```
mon initial members = {hostname}[,{hostname}]
```

例如：

```
mon initial members = node1
```

7、将初始monitor(s)的IP地址添加到Ceph配置文件并保存文件。

```
mon host = {ip-address}[,{ip-address}]
```

例如：

```
mon host = 192.168.0.1
```

注意：您可以使用IPv6地址而不是IPv4地址，但必须将ms bind ipv6设置为true。有关网络配置的详细信息，请参阅[网络配置参考](http://docs.ceph.com/docs/master/rados/configuration/network-config-ref)。

8、为集群创建keyring并生成monitor secret key。

```
ceph-authtool --create-keyring /tmp/ceph.mon.keyring --gen-key -n mon. --cap mon 'allow *'
```

9、生成administrator keyring，生成client.admin用户并将用户添加到keyring。

```
sudo ceph-authtool --create-keyring /etc/ceph/ceph.client.admin.keyring --gen-key -n client.admin --cap mon 'allow *' --cap osd 'allow *' --cap mds 'allow *' --cap mgr 'allow *'
```

10、生成bootstrap-osd keyring，生成client.bootstrap-osd用户并将用户添加到keyring。

```
sudo ceph-authtool --create-keyring /var/lib/ceph/bootstrap-osd/ceph.keyring --gen-key -n client.bootstrap-osd --cap mon 'profile bootstrap-osd'
```

11、将生成的keys添加到ceph.mon.keyring。

```
sudo ceph-authtool /tmp/ceph.mon.keyring --import-keyring /etc/ceph/ceph.client.admin.keyring
sudo ceph-authtool /tmp/ceph.mon.keyring --import-keyring /var/lib/ceph/bootstrap-osd/ceph.keyring
```

12、使用主机名，主机IP地址和FSID生成monitor map。将其另存为/tmp/monmap。

```
monmaptool --create --add {hostname} {ip-address} --fsid {uuid} /tmp/monmap
```

例如：

```
monmaptool --create --add node1 192.168.0.1 --fsid a7f64266-0894-4f1e-a635-d0aeaca0e993 /tmp/monmap
```

13、在monitor主机上创建默认数据目录（或多个目录）。

```
sudo mkdir /var/lib/ceph/mon/{cluster-name}-{hostname}
```

例如：

```
sudo -u ceph mkdir /var/lib/ceph/mon/ceph-node1
```

有关详细信息，请参阅[Monitor Config Reference - Data](http://docs.ceph.com/docs/master/rados/configuration/mon-config-ref#data)。

14、使用monitor map和keyring填充monitor daemon(s)。

```
sudo -u ceph ceph-mon [--cluster {cluster-name}] --mkfs -i {hostname} --monmap /tmp/monmap --keyring /tmp/ceph.mon.keyring
```

例如：

```
sudo -u ceph ceph-mon --mkfs -i node1 --monmap /tmp/monmap --keyring /tmp/ceph.mon.keyring
```

15、考虑Ceph配置文件的settings，常见settings包括以下内容：

```
[global]
fsid = {cluster-id}
mon initial members = {hostname}[, {hostname}]
mon host = {ip-address}[, {ip-address}]
public network = {network}[, {network}]
cluster network = {network}[, {network}]
auth cluster required = cephx
auth service required = cephx
auth client required = cephx
osd journal size = {n}
osd pool default size = {n}  # Write an object n times.
osd pool default min size = {n} # Allow writing n copies in a degraded state.
osd pool default pg num = {n}
osd pool default pgp num = {n}
osd crush chooseleaf type = {n}
```

在上面的示例中，配置的[global]部分可能如下所示：

```
[global]
fsid = a7f64266-0894-4f1e-a635-d0aeaca0e993
mon initial members = node1
mon host = 192.168.0.1
public network = 192.168.0.0/24
auth cluster required = cephx
auth service required = cephx
auth client required = cephx
osd journal size = 1024
osd pool default size = 3
osd pool default min size = 2
osd pool default pg num = 333
osd pool default pgp num = 333
osd crush chooseleaf type = 1
```

16、启动monitor(s)。

对于大多数linux发行版，services通过systemd启动：

```
sudo systemctl start ceph-mon@node1
```

对于较旧的Debian/CentOS/RHEL，请使用sysvinit：

```
sudo /etc/init.d/ceph start mon.node1
```

17、验证monitor是否正在运行。

```
ceph -s
```

您应该看到monitor已启动并打印运行的输出，并且您应该看到一个运行状况错误，提示placement groups（PG）处于inactive状态。看起来像这样：

```
cluster:
  id:     a7f64266-0894-4f1e-a635-d0aeaca0e993
  health: HEALTH_OK

services:
  mon: 1 daemons, quorum node1
  mgr: node1(active)
  osd: 0 osds: 0 up, 0 in

data:
  pools:   0 pools, 0 pgs
  objects: 0 objects, 0 bytes
  usage:   0 kB used, 0 kB / 0 kB avail
  pgs:
```

注意：添加OSD并启动它们后，placement group（PG）错误应该消失。有关详细信息，请参阅[添加OSD](http://docs.ceph.com/docs/master/install/manual-deployment/#adding-osds)。

## MANAGER DAEMON CONFIGURATION

在运行ceph-mon daemon的每个节点上，还应该设置ceph-mgr daemon。

请参阅[ceph-mgr管理员指南](http://docs.ceph.com/docs/master/mgr/administrator/#mgr-administrator-guide)。

## ADDING OSDS

初始monitor(s)运行后，应添加OSD。在有足够的OSD来处理对象的副本数量之前，您的集群无法达到active + clean状态（例如，osd pool default size = 2需要至少两个OSD）。

引导monitor后，您的集群具有默认的CRUSH map，但是，CRUSH map没有映射到Ceph节点的任何Ceph OSD Daemons。

### SHORT FORM（简写）

Ceph提供了ceph-volume实用程序，它可以prepare logical volume，disk或partition，以便与ceph一起使用。 ceph-volume实用程序通过递增索引来创建OSD ID。 此外，ceph-volume会将新OSD添加到CRUSH map中的host下。执行ceph-volume -h以获取CLI详细信息。ceph-volume实用程序自动执行下面的[Long Form](http://docs.ceph.com/docs/master/install/manual-deployment/#long-form)的步骤。要使用short form过程创建前两个OSD，请在node2和node3上执行以下操作：

### BLUESTORE

1、创建OSD。

```
ssh {node-name}
sudo ceph-volume lvm create --data {data-path}
```

例如：

```
ssh node1
sudo ceph-volume lvm create --data /dev/hdd1
```

或者，创建过程可以分为两个阶段（prepare和activate）：

1、Prepare the OSD。

```
ssh {node-name}
sudo ceph-volume lvm prepare --data {data-path} {data-path}
```

例如：

```
ssh node1
sudo ceph-volume lvm prepare --data /dev/hdd1
```

prepare后，激活prepare后的OSD的ID和FSID。这些可以通过列出当前服务器中的OSD来获得：

```
sudo ceph-volume lvm list
```

2、Activate the OSD。

```
sudo ceph-volume lvm activate {ID} {FSID}
```

例如：

```
sudo ceph-volume lvm activate 0 a7f64266-0894-4f1e-a635-d0aeaca0e993
```

### FILESTORE

1、Create the OSD。

```
ssh {node-name}
sudo ceph-volume lvm create --filestore --data {data-path} --journal {journal-path}
```

例如：

```
ssh node1
sudo ceph-volume lvm create --filestore --data /dev/hdd1 --journal /dev/hdd2
```

或者，创建过程可以分为两个阶段（prepare和activate）：

1、Prepare the OSD。

```
ssh {node-name}
sudo ceph-volume lvm prepare --filestore --data {data-path} --journal {journal-path}
```

例如：

```
ssh node1
sudo ceph-volume lvm prepare --filestore --data /dev/hdd1 --journal /dev/hdd2
```

prepare后，激活prepare后的OSD的ID和FSID。这些可以通过列出当前服务器中的OSD来获得：

```
sudo ceph-volume lvm list
```

2、Activate the OSD。

```
sudo ceph-volume lvm activate --filestore {ID} {FSID}
```

例如：

```
sudo ceph-volume lvm activate --filestore 0 a7f64266-0894-4f1e-a635-d0aeaca0e993
```

### LONG FORM

如果没有任何helper实用程序，请创建一个OSD并使用以下步骤将其添加到集群和CRUSH map中。要使用long form方式创建前两个OSD，请对每个OSD执行以下步骤。

```
注意：此过程没有描述使用dm-crypt“lockbox”在dm-crypt之上的部署。
```

1、连接到OSD主机并变为root用户。

```
ssh {node-name}
sudo bash
```

2、为OSD生成UUID。

```
UUID=$(uuidgen)
```

3、为OSD生成cephx key。

```
OSD_SECRET=$(ceph-authtool --gen-print-key)
```

4、创建OSD。 请注意，如果您需要重用之前销毁的OSD ID，可以提供OSD ID作为ceph osd new的附加参数。我们假设机器上存在client.bootstrap-osd key。您也可以在拥有该key的其他主机上以client.admin身份执行此命令：

```
ID=$(echo "{\"cephx_secret\": \"$OSD_SECRET\"}" | \
   ceph osd new $UUID -i - \
   -n client.bootstrap-osd -k /var/lib/ceph/bootstrap-osd/ceph.keyring)
```

也可以在JSON中包含crush_device_class属性，以设置默认值以外的initial class（基于自动检测到的设备类型，ssd或hdd）。

5、在新OSD上创建默认目录。

```
mkdir /var/lib/ceph/osd/ceph-$ID
```

6、如果OSD使用操作系统以外的drive，先创建文件系统，并将其mount到刚刚创建的目录。

```
mkfs.xfs /dev/{DEV}
mount /dev/{DEV} /var/lib/ceph/osd/ceph-$ID
```

7、将secret写入OSD keyring文件中。

```
ceph-authtool --create-keyring /var/lib/ceph/osd/ceph-$ID/keyring \
     --name osd.$ID --add-key $OSD_SECRET
```

8、初始化OSD数据目录。

```
ceph-osd -i $ID --mkfs --osd-uuid $UUID
```

9、修复所有权。

```
chown -R ceph:ceph /var/lib/ceph/osd/ceph-$ID
```

10、将OSD添加到Ceph后，OSD就在您的配置中。但是，它还没有运行。必须先启动新的OSD才能开始接收数据。

systemd方式：

```
systemctl enable ceph-osd@$ID
systemctl start ceph-osd@$ID
```

例如：

```
systemctl enable ceph-osd@12
systemctl start ceph-osd@12
```

## ADDING MDS

在以下说明中，{id}是任意名称，例如计算机的hostname。

1、创建mds数据目录：

```
mkdir -p /var/lib/ceph/mds/{cluster-name}-{id}
```

2、创建一个keyring：

```
ceph-authtool --create-keyring /var/lib/ceph/mds/{cluster-name}-{id}/keyring --gen-key -n mds.{id}
```

3、导入keyring并设置caps：

```
ceph auth add mds.{id} osd "allow rwx" mds "allow" mon "allow profile mds" -i /var/lib/ceph/mds/{cluster}-{id}/keyring
```

4、添加到ceph.conf：

```
[mds.{id}]
host = {id}
```

5、以手动方式启动daemon：

```
ceph-mds --cluster {cluster-name} -i {id} -m {mon-hostname}:{mon-port} [-f]
```

6、以正确的方式启动daemon（使用ceph.conf entry）：

```
service ceph start
```

7、如果启动daemon失败并显示以下错误：

```
mds.-1.0 ERROR: failed to authenticate: (22) Invalid argument
```

那么，需要确认你没有在ceph.conf的global section中设置keyring；并将其移至client section。或添加特定于此mds daemon的keyring设置。并验证您是否在mds data目录中看到相同的key，并且与ceph auth get mds.{id}命令的输出相同。

8、现在您已准备好[创建Ceph filesystem](http://docs.ceph.com/docs/master/cephfs/createfs/)。

## SUMMARY

一旦您的monitor和两个OSD启动并运行，可以通过执行以下操作来观察placement groups peer：

```
ceph -w
```

要查看tree，请执行以下操作：

```
ceph osd tree
```

你应该看到像这样的输出：

```
# id    weight  type name       up/down reweight
-1      2       root default
-2      2               host node1
0       1                       osd.0   up      1
-3      1               host node2
1       1                       osd.1   up      1
```

要添加（或删除）其他monitors，请参阅[Add/Remove Monitors](http://docs.ceph.com/docs/master/rados/operations/add-or-rm-mons/)。要添加（或删除）其他Ceph OSD Daemons，请参阅[Add/Remove OSDs](http://docs.ceph.com/docs/master/rados/operations/add-or-rm-osds/)。



# 部署实验

## monitor部署实验

1、添加ceph源

```
[root@cephlm ~]# vi /etc/yum.repos.d/ceph.repo 
[ceph]
name=Ceph
baseurl=http://mirrors.163.com/ceph/rpm-luminous/el7/x86_64/
# baseurl=https://download.ceph.com/rpm-luminous/el7/x86_64/
enabled=1
gpgcheck=1
type=rpm-md
gpgkey=http://mirrors.163.com/ceph/keys/release.asc
# gpgkey=https://download.ceph.com/keys/release.asc
```

2、安装epel与ceph

```
[1]epel参考：https://blog.csdn.net/yasi_xi/article/details/11746255
EPEL的全称叫 Extra Packages for Enterprise Linux 。EPEL是由 Fedora 社区打造，为 RHEL 及衍生发行版如 CentOS、Scientific Linux 等提供高质量软件包的项目。装上了 EPEL之后，就相当于添加了一个第三方源。

[root@cephlm ~]# yum install epel-release -y && yum install ceph -y
```

3、为集群创建keyring并生成monitor secret key。

```
[root@cephlm ~]# ceph-authtool --create-keyring /tmp/ceph.mon.keyring --gen-key -n mon. --cap mon 'allow *'
creating /tmp/ceph.mon.keyring
[root@cephlm ~]# cat /tmp/ceph.mon.keyring
[mon.]
	key = AQAgbv1bc62FBBAAvuCz2a5EDtbAmy9ep1Dxxw==
	caps mon = "allow *"
```

4、生成administrator keyring，生成client.admin用户并将用户添加到keyring。

```
[root@cephlm ~]# sudo ceph-authtool --create-keyring /etc/ceph/ceph.client.admin.keyring --gen-key -n client.admin --cap mon 'allow *' --cap osd 'allow *' --cap mds 'allow *' --cap mgr 'allow *'
creating /etc/ceph/ceph.client.admin.keyring
[root@cephlm ~]# cat /etc/ceph/ceph.client.admin.keyring
[client.admin]
	key = AQBFbv1bImaJCxAAYUiUCuia//zZSMIPyOHJuA==
	caps mds = "allow *"
	caps mgr = "allow *"
	caps mon = "allow *"
	caps osd = "allow *"
```

5、生成bootstrap-osd keyring，生成client.bootstrap-osd用户并将用户添加到keyring。

```
[root@cephlm ~]# sudo ceph-authtool --create-keyring /var/lib/ceph/bootstrap-osd/ceph.keyring --gen-key -n client.bootstrap-osd --cap mon 'profile bootstrap-osd'
creating /var/lib/ceph/bootstrap-osd/ceph.keyring
[root@cephlm ~]# cat /var/lib/ceph/bootstrap-osd/ceph.keyring
[client.bootstrap-osd]
	key = AQBQbv1bXv0WAhAAo/hv7OOaftMHOovHNeyOFg==
	caps mon = "profile bootstrap-osd"
```

6、将生成的keys添加到ceph.mon.keyring。

```
[root@cephlm ~]# sudo ceph-authtool /tmp/ceph.mon.keyring --import-keyring /etc/ceph/ceph.client.admin.keyring
[root@cephlm ~]# sudo ceph-authtool /tmp/ceph.mon.keyring --import-keyring /var/lib/ceph/bootstrap-osd/ceph.keyring

[root@cephlm ~]# cat /tmp/ceph.mon.keyring
[mon.]
	key = AQAgbv1bc62FBBAAvuCz2a5EDtbAmy9ep1Dxxw==
	caps mon = "allow *"
[client.admin]
	key = AQBFbv1bImaJCxAAYUiUCuia//zZSMIPyOHJuA==
	caps mds = "allow *"
	caps mgr = "allow *"
	caps mon = "allow *"
	caps osd = "allow *"
[client.bootstrap-osd]
	key = AQBQbv1bXv0WAhAAo/hv7OOaftMHOovHNeyOFg==
	caps mon = "profile bootstrap-osd"

```

7、使用主机名，主机IP地址和FSID生成monitor map。将其另存为/tmp/monmap。

```
[root@cephlm ~]# monmaptool --create --add cephlm 192.168.0.10 --fsid c8b0b137-1ba7-4c1f-a514-281139c35233 /tmp/monmap
monmaptool: monmap file /tmp/monmap
monmaptool: set fsid to c8b0b137-1ba7-4c1f-a514-281139c35233
monmaptool: writing epoch 0 to /tmp/monmap (1 monitors)

更改权限，可是使ceph用户有读该文件的权限
[root@cephlm ~]# chmod +r /tmp/monmap && chmod +r /tmp/ceph.mon.keyring

[root@cephlm ~]# monmaptool --print /tmp/monmap
monmaptool: monmap file /tmp/monmap
epoch 0
fsid c8b0b137-1ba7-4c1f-a514-281139c35233
last_changed 2018-11-28 10:55:11.677288
created 2018-11-28 10:55:11.677288
0: 192.168.56.205:6789/0 mon.cephlm
```

8、创建配置文件文件`<集群名>.conf`

```
[root@cephlm ~]# vi /etc/ceph/ceph.conf 
[global]
fsid = c8b0b137-1ba7-4c1f-a514-281139c35233
mon initial members = cephlm
mon host = 192.168.0.10
public network = 192.168.0.0/24
cluster network = 192.168.0.0/24
auth cluster required = cephx
auth service required = cephx
auth client required = cephx
osd pool default size = 1
osd pool default min size = 1
osd pool default pg num = 16
osd pool default pgp num = 16
```

9、使用monitor map和keyring填充monitor daemon(s)。

```
[root@cephlm ~]# sudo -u ceph ceph-mon --cluster ceph --mkfs -i cephlm --monmap /tmp/monmap --keyring /tmp/ceph.mon.keyring
```

10、启动ceph-mon服务

```
[root@cephlm ~]# systemctl enable ceph-mon@cephlm
[root@cephlm ~]# systemctl start ceph-mon@cephlm

[root@cephlm ~]# ceph -s
  cluster:
    id:     c8b0b137-1ba7-4c1f-a514-281139c35233
    health: HEALTH_OK
 
  services:
    mon: 1 daemons, quorum cephlm
    mgr: no daemons active
    osd: 0 osds: 0 up, 0 in
 
  data:
    pools:   0 pools, 0 pgs
    objects: 0 objects, 0B
    usage:   0B used, 0B / 0B avail
    pgs:     
```

## osd部署实验



```
1、为OSD生成UUID。
[root@cephlm ~]# UUID=$(uuidgen)
[root@cephlm ~]# echo ${UUID}
265b6807-fa12-46f7-8e25-ce8b03dc2a2d

2、为OSD生成cephx key。
[root@cephlm ~]# OSD_SECRET=$(ceph-authtool --gen-print-key)
[root@cephlm ~]# echo ${OSD_SECRET}
AQDljwhcB8MxBxAATx/pqYOv0uRhQI9Tey5UeQ==

3、为OSD生成cephx key（秘钥）。
[root@cephlm ~]# ID=$(echo "{\"cephx_secret\": \"$OSD_SECRET\"}" | \
     ceph osd new $UUID -i - \/*
     -n client.bootstrap-osd -k /var/lib/ceph/bootstrap-osd/ceph.keyring)
[root@cephlm ~]# echo ${ID}
0

4、
[root@cephlm ~]# mkdir /var/lib/ceph/osd/ceph-$ID

5、
[root@cephlm ~]# lsblk
NAME   MAJ:MIN RM SIZE RO TYPE MOUNTPOINT
vda    253:0    0  20G  0 disk 
├─vda1 253:1    0   1G  0 part /boot
├─vda2 253:2    0   4G  0 part [SWAP]
└─vda3 253:3    0  15G  0 part /
vdb    253:16   0  50G  0 disk 
vdc    253:32   0  50G  0 disk 
vdd    253:48   0  50G  0 disk 

[root@cephlm ~]# ll /var/lib/ceph/osd/
总用量 0
drwxr-xr-x 2 root root 6 12月  6 11:03 ceph-0

[root@cephlm ~]# mkfs.xfs /dev/vdb -f
meta-data=/dev/vdb               isize=512    agcount=4, agsize=3276800 blks
         =                       sectsz=512   attr=2, projid32bit=1
         =                       crc=1        finobt=0, sparse=0
data     =                       bsize=4096   blocks=13107200, imaxpct=25
         =                       sunit=0      swidth=0 blks
naming   =version 2              bsize=4096   ascii-ci=0 ftype=1
log      =internal log           bsize=4096   blocks=6400, version=2
         =                       sectsz=512   sunit=0 blks, lazy-count=1
realtime =none                   extsz=4096   blocks=0, rtextents=0

[root@cephlm ~]# mount /dev/vdb /var/lib/ceph/osd/ceph-$ID/
[root@cephlm ~]# df -Th
文件系统       类型      容量  已用  可用 已用% 挂载点
/dev/vda3      xfs        15G  1.6G   14G   11% /
devtmpfs       devtmpfs  1.9G     0  1.9G    0% /dev
tmpfs          tmpfs     1.9G     0  1.9G    0% /dev/shm
tmpfs          tmpfs     1.9G  8.6M  1.9G    1% /run
tmpfs          tmpfs     1.9G     0  1.9G    0% /sys/fs/cgroup
/dev/vda1      xfs      1014M  172M  843M   17% /boot
tmpfs          tmpfs     379M     0  379M    0% /run/user/0
/dev/vdb       xfs        50G   33M   50G    1% /var/lib/ceph/osd/ceph-0

6、将secret写入OSD keyring文件。
[root@cephlm ~]# ceph-authtool --create-keyring /var/lib/ceph/osd/ceph-$ID/keyring --name osd.$ID --add-key $OSD_SECRET
creating /var/lib/ceph/osd/ceph-0/keyring
added entity osd.0 auth auth(auid = 18446744073709551615 key=AQDljwhcB8MxBxAATx/pqYOv0uRhQI9Tey5UeQ== with 0 caps)

7、初始化OSD数据目录
[root@cephlm ~]# ceph-osd -i $ID --mkfs --osd-uuid $UUID
2019-08-02 18:07:34.990087 7f41b6680d80 -1 journal FileJournal::_open: disabling aio for non-block journal.  Use journal_force_aio to force use of aio anyway
2019-08-02 18:07:35.061501 7f41b6680d80 -1 journal FileJournal::_open: disabling aio for non-block journal.  Use journal_force_aio to force use of aio anyway
2019-08-02 18:07:35.062969 7f41b6680d80 -1 journal do_read_entry(4096): bad header magic
2019-08-02 18:07:35.062993 7f41b6680d80 -1 journal do_read_entry(4096): bad header magic
2019-08-02 18:07:35.063581 7f41b6680d80 -1 read_settings error reading settings: (2) No such file or directory
2019-08-02 18:07:35.146735 7f41b6680d80 -1 created object store /var/lib/ceph/osd/ceph-0 for osd.0 fsid c8b0b137-1ba7-4c1f-a514-281139c35233

8、
[root@cephlm ~]# chown -R ceph:ceph /var/lib/ceph/osd/ceph-$ID

9、
[root@cephlm ~]# systemctl enable ceph-osd@$ID
[root@cephlm ~]# systemctl start ceph-osd@$ID
10、
[root@sds-ceph-1 ~]# ceph osd pool create rbd 16
[root@sds-ceph-1 ~]# rados bench -p rbd 10 write --no-cleanup

[root@sds-ceph-1 ~]# tree -h  /var/lib/ceph/osd/ceph-0/current/
/var/lib/ceph/osd/ceph-0/current/
├── [4.0K]  1.0_head
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject126__head_314B9110__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject138__head_F0F776D0__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject149__head_9DC23D60__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject154__head_03D0DB50__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject179__head_E33CA580__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject209__head_3BFDF2F0__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject211__head_DECF96B0__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject212__head_5CED1CF0__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject217__head_1319A430__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject220__head_524D4D00__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject229__head_F40C7DE0__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject22__head_1686F940__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject235__head_516A6B20__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject236__head_12CA1390__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject73__head_397FC990__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject77__head_ADAEA100__1
│   └── [   0]  __head_00000000__1
├── [   6]  1.0_TEMP
├── [4.0K]  1.1_head
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject102__head_862AA4F1__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject129__head_F5965891__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject151__head_98E52B51__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject184__head_E52D8D41__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject1__head_F5F320A1__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject234__head_00E8BB81__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject244__head_2363F6E1__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject25__head_581DDE31__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject32__head_324147A1__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject35__head_34FABC91__1
│   └── [   0]  __head_00000001__1
├── [   6]  1.1_TEMP
├── [4.0K]  1.2_head
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject104__head_413DA2B2__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject117__head_C0124C02__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject125__head_9DBB0642__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject153__head_37933452__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject167__head_0AE2CE12__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject173__head_821A3292__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject175__head_734C3FB2__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject180__head_74A8D2D2__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject195__head_7A482B82__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject214__head_C62A3262__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject227__head_FBA5E8D2__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject230__head_F81DABF2__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject240__head_96D571D2__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject251__head_8C210072__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject28__head_8D1859C2__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject43__head_9D4243F2__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject4__head_6D94E022__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject53__head_F31A17D2__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject74__head_BA8C88D2__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject93__head_26EE91D2__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject97__head_1025B4F2__1
│   └── [   0]  __head_00000002__1
├── [   6]  1.2_TEMP
├── [4.0K]  1.3_head
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject114__head_C05EC3E3__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject145__head_BB9B5383__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject187__head_7F2D3B93__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject20__head_FB09C543__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject222__head_CA878AF3__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject223__head_45D47B63__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject249__head_EB2C03F3__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject2__head_E1BEFAD3__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject48__head_C2ABBF03__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject58__head_73A784F3__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject66__head_7D1C7D23__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject68__head_64431003__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject75__head_EFA44023__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject7__head_1EDC2B13__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject83__head_56DE9F23__1
│   └── [   0]  __head_00000003__1
├── [   6]  1.3_TEMP
├── [4.0K]  1.4_head
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject0__head_F8BC28D4__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject160__head_866C8F94__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject162__head_02475BD4__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject164__head_26707F24__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject16__head_3B6FF704__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject174__head_ECF6CC74__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject18__head_7CB94CE4__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject202__head_503B6AE4__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject237__head_1AF8F074__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject247__head_A70993B4__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject31__head_09DBA9F4__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject36__head_ABBB7824__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject39__head_E1727904__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject3__head_E0FE6334__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject46__head_9E9B4204__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject51__head_8C1E0DC4__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject56__head_7065C2F4__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject62__head_DB32DD24__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject87__head_BDBE9FC4__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject90__head_A1654F74__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject96__head_FC6F33F4__1
│   └── [   0]  __head_00000004__1
├── [   6]  1.4_TEMP
├── [4.0K]  1.5_head
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject118__head_7B842745__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject123__head_5CEFA465__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject183__head_10F85FE5__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject186__head_3000C235__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject191__head_BC532EE5__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject224__head_92A1E3C5__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject29__head_027ADDC5__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject67__head_5BDDDAA5__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject82__head_A64A01D5__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject8__head_F68FCDE5__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject94__head_ECE61315__1
│   └── [   0]  __head_00000005__1
├── [   6]  1.5_TEMP
├── [4.0K]  1.6_head
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject121__head_3F39BC76__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject12__head_88A43B86__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject13__head_FD5483D6__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject141__head_CDACF3C6__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject157__head_E56D1786__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject197__head_804A2DC6__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject207__head_A3266F06__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject216__head_DE5D3A76__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject226__head_CAFCCF66__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject44__head_0544C586__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject45__head_65E3FAB6__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject61__head_A2704176__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject72__head_6E3BA9D6__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject80__head_1B1E6F16__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject85__head_A0756156__1
│   └── [   0]  __head_00000006__1
├── [   6]  1.6_TEMP
├── [4.0K]  1.7_head
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject100__head_EF2E6867__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject139__head_5E961CE7__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject142__head_01BCFCA7__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject143__head_A3CDD177__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject144__head_74883667__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject158__head_F5DD1EA7__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject165__head_031366F7__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject168__head_9BE47967__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject169__head_E96B49F7__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject182__head_7119D257__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject194__head_D3C65A17__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject250__head_E9F80417__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject41__head_9295C277__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject55__head_70B967B7__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject78__head_367C2607__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject89__head_7CDE0FC7__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject98__head_96668A47__1
│   └── [   0]  __head_00000007__1
├── [   6]  1.7_TEMP
├── [4.0K]  1.8_head
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject116__head_F4318178__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject128__head_32ED4D38__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject134__head_486DF128__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject156__head_F2E7C068__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject190__head_5AAF63A8__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject219__head_50799E88__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject225__head_9EB9D0C8__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject242__head_F2EC42B8__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject33__head_64C2C558__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject34__head_65F25F38__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject38__head_D38CB608__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject49__head_FF4A4208__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject50__head_0F3F77A8__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject52__head_0F2A0548__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject69__head_BDEA5548__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject71__head_EA5383B8__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject76__head_02238F98__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject88__head_45AB4318__1
│   └── [   0]  __head_00000008__1
├── [   6]  1.8_TEMP
├── [4.0K]  1.9_head
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject110__head_CE2B10A9__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject136__head_04EDA809__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject163__head_37F8AE49__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject177__head_5F771509__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject193__head_23935A99__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject215__head_15DEBEA9__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject21__head_11518C29__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject248__head_FAFE5689__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject27__head_5B7D7369__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject60__head_64B13949__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject70__head_F3A64E09__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject91__head_31511109__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject99__head_8A68E509__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject9__head_FC52C009__1
│   └── [   0]  __head_00000009__1
├── [   6]  1.9_TEMP
├── [4.0K]  1.a_head
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject112__head_FF4D01DA__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject113__head_CADF065A__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject119__head_A72F161A__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject122__head_BF4A0A6A__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject124__head_5BC12E2A__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject140__head_37A19CDA__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject14__head_64FC3EDA__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject152__head_14C2CA0A__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject166__head_C47A6B0A__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject171__head_79A2CFEA__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject17__head_2A4A393A__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject185__head_1D2DDDDA__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject188__head_7FBA73AA__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject204__head_AB1FBD8A__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject205__head_B24EE76A__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject206__head_9891B01A__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject208__head_C07981EA__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject210__head_1227C4DA__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject228__head_0B9B39AA__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject238__head_C56F1D5A__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject23__head_4F279EBA__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject26__head_CCB3C59A__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject86__head_5ED418EA__1
│   └── [   0]  __head_0000000A__1
├── [   6]  1.a_TEMP
├── [4.0K]  1.b_head
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject103__head_F8EF0F1B__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject107__head_F3693ABB__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject108__head_13D4D0EB__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject109__head_11C8A99B__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject132__head_227AA5FB__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject137__head_5D82744B__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject148__head_989FE31B__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject159__head_422DA2AB__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject178__head_01C4C3CB__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject181__head_0DC3436B__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject213__head_05B8BA6B__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject246__head_2140121B__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject24__head_5B8169AB__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject64__head_CC62D71B__1
│   └── [   0]  __head_0000000B__1
├── [   6]  1.b_TEMP
├── [4.0K]  1.c_head
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject101__head_7711D38C__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject11__head_0218B38C__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject127__head_9A51BCDC__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject133__head_7E6B67EC__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject135__head_C67A3E8C__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject150__head_8635CA6C__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject161__head_3A42EA3C__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject170__head_612CFAFC__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject172__head_EB33BB8C__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject196__head_475FC6BC__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject19__head_D21864EC__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject200__head_52F1139C__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject201__head_14AC036C__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject243__head_3E9C237C__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject245__head_DABE713C__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject5__head_7CE3E36C__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject65__head_86389E8C__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject6__head_1FBF833C__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject81__head_05B34FAC__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject92__head_3988A13C__1
│   └── [   0]  __head_0000000C__1
├── [   6]  1.c_TEMP
├── [4.0K]  1.d_head
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject111__head_66CE6C0D__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject115__head_71F498AD__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject120__head_1BFD990D__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject130__head_301FC3CD__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject232__head_320DC13D__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject37__head_AC68B41D__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject40__head_423D9AAD__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject47__head_0951C33D__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject84__head_5EB447DD__1
│   ├── [  24]  benchmark\\ulast\\umetadata__head_8E7A861D__1
│   └── [   0]  __head_0000000D__1
├── [   6]  1.d_TEMP
├── [4.0K]  1.e_head
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject105__head_C439984E__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject10__head_1BF35FCE__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject131__head_DBAC810E__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject146__head_6153061E__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject176__head_80C2B02E__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject189__head_4F8A59BE__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject199__head_5ACCE8EE__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject203__head_D77F59BE__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject221__head_C3ADCA4E__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject231__head_0C48675E__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject233__head_F01E13EE__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject30__head_9464F9BE__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject54__head_B1E1B44E__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject59__head_CE84DF4E__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject79__head_CAFFBC0E__1
│   └── [   0]  __head_0000000E__1
├── [   6]  1.e_TEMP
├── [4.0K]  1.f_head
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject106__head_48E9311F__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject147__head_7211B38F__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject155__head_551133AF__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject15__head_58EF207F__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject192__head_9E2A248F__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject198__head_D7D45B1F__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject218__head_F2A93B5F__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject239__head_2FC0444F__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject241__head_286B4E0F__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject42__head_6FF5EA1F__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject57__head_2A0A792F__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject63__head_2685437F__1
│   ├── [4.0M]  benchmark\\udata\\usds-ceph-1.novalocal\\u12560\\uobject95__head_1786F82F__1
│   └── [   0]  __head_0000000F__1
├── [   6]  1.f_TEMP
├── [   4]  commit_op_seq
├── [4.0K]  meta
│   ├── [ 802]  inc\\uosdmap.1__0_B65F4306__none
│   ├── [ 232]  inc\\uosdmap.2__0_B65F40D6__none
│   ├── [ 635]  inc\\uosdmap.3__0_B65F4066__none
│   ├── [ 763]  inc\\uosdmap.4__0_B65F4136__none
│   ├── [ 406]  inc\\uosdmap.5__0_B65F46C6__none
│   ├── [ 454]  inc\\uosdmap.6__0_B65F4796__none
│   ├── [ 212]  inc\\uosdmap.7__0_B65F4726__none
│   ├── [ 204]  inc\\uosdmap.8__0_B65F44F6__none
│   ├── [ 598]  osdmap.1__0_FD6E49B1__none
│   ├── [ 869]  osdmap.2__0_FD6E4941__none
│   ├── [ 960]  osdmap.3__0_FD6E4E11__none
│   ├── [1.1K]  osdmap.4__0_FD6E4FA1__none
│   ├── [1.0K]  osdmap.5__0_FD6E4F71__none
│   ├── [1.3K]  osdmap.6__0_FD6E4C01__none
│   ├── [1.3K]  osdmap.7__0_FD6E4DD1__none
│   ├── [1.3K]  osdmap.8__0_FD6E4D61__none
│   ├── [ 511]  osd\\usuperblock__0_23C2FCDE__none
│   └── [   0]  snapmapper__0_A468EC03__none
├── [   0]  nosnap
└── [ 179]  omap
    ├── [1002]  000007.sst
    ├── [208K]  000009.log
    ├── [  16]  CURRENT
    ├── [  37]  IDENTITY
    ├── [   0]  LOCK
    ├── [ 15K]  LOG
    ├── [ 108]  MANIFEST-000008
    ├── [4.1K]  OPTIONS-000008
    ├── [4.1K]  OPTIONS-000011
    └── [  37]  osd_uuid

34 directories, 299 files

[root@sds-ceph-1 ~]# rados ls -p rbd
benchmark_data_sds-ceph-1.novalocal_12560_object77
benchmark_data_sds-ceph-1.novalocal_12560_object220
benchmark_data_sds-ceph-1.novalocal_12560_object179
benchmark_data_sds-ceph-1.novalocal_12560_object22
benchmark_data_sds-ceph-1.novalocal_12560_object235
benchmark_data_sds-ceph-1.novalocal_12560_object149
benchmark_data_sds-ceph-1.novalocal_12560_object229
benchmark_data_sds-ceph-1.novalocal_12560_object126
benchmark_data_sds-ceph-1.novalocal_12560_object73
benchmark_data_sds-ceph-1.novalocal_12560_object236
benchmark_data_sds-ceph-1.novalocal_12560_object154
benchmark_data_sds-ceph-1.novalocal_12560_object138
benchmark_data_sds-ceph-1.novalocal_12560_object217
benchmark_data_sds-ceph-1.novalocal_12560_object211
benchmark_data_sds-ceph-1.novalocal_12560_object212
benchmark_data_sds-ceph-1.novalocal_12560_object209
benchmark_data_sds-ceph-1.novalocal_12560_object49
benchmark_data_sds-ceph-1.novalocal_12560_object38
benchmark_data_sds-ceph-1.novalocal_12560_object219
benchmark_data_sds-ceph-1.novalocal_12560_object52
benchmark_data_sds-ceph-1.novalocal_12560_object69
benchmark_data_sds-ceph-1.novalocal_12560_object225
benchmark_data_sds-ceph-1.novalocal_12560_object134
benchmark_data_sds-ceph-1.novalocal_12560_object190
benchmark_data_sds-ceph-1.novalocal_12560_object50
benchmark_data_sds-ceph-1.novalocal_12560_object156
benchmark_data_sds-ceph-1.novalocal_12560_object88
benchmark_data_sds-ceph-1.novalocal_12560_object76
benchmark_data_sds-ceph-1.novalocal_12560_object33
benchmark_data_sds-ceph-1.novalocal_12560_object128
benchmark_data_sds-ceph-1.novalocal_12560_object34
benchmark_data_sds-ceph-1.novalocal_12560_object242
benchmark_data_sds-ceph-1.novalocal_12560_object71
benchmark_data_sds-ceph-1.novalocal_12560_object116
benchmark_data_sds-ceph-1.novalocal_12560_object46
benchmark_data_sds-ceph-1.novalocal_12560_object39
benchmark_data_sds-ceph-1.novalocal_12560_object16
benchmark_data_sds-ceph-1.novalocal_12560_object51
benchmark_data_sds-ceph-1.novalocal_12560_object87
benchmark_data_sds-ceph-1.novalocal_12560_object36
benchmark_data_sds-ceph-1.novalocal_12560_object62
benchmark_data_sds-ceph-1.novalocal_12560_object164
benchmark_data_sds-ceph-1.novalocal_12560_object18
benchmark_data_sds-ceph-1.novalocal_12560_object202
benchmark_data_sds-ceph-1.novalocal_12560_object160
benchmark_data_sds-ceph-1.novalocal_12560_object0
benchmark_data_sds-ceph-1.novalocal_12560_object162
benchmark_data_sds-ceph-1.novalocal_12560_object3
benchmark_data_sds-ceph-1.novalocal_12560_object247
benchmark_data_sds-ceph-1.novalocal_12560_object237
benchmark_data_sds-ceph-1.novalocal_12560_object174
benchmark_data_sds-ceph-1.novalocal_12560_object90
benchmark_data_sds-ceph-1.novalocal_12560_object56
benchmark_data_sds-ceph-1.novalocal_12560_object31
benchmark_data_sds-ceph-1.novalocal_12560_object96
benchmark_data_sds-ceph-1.novalocal_12560_object65
benchmark_data_sds-ceph-1.novalocal_12560_object135
benchmark_data_sds-ceph-1.novalocal_12560_object101
benchmark_data_sds-ceph-1.novalocal_12560_object11
benchmark_data_sds-ceph-1.novalocal_12560_object172
benchmark_data_sds-ceph-1.novalocal_12560_object81
benchmark_data_sds-ceph-1.novalocal_12560_object150
benchmark_data_sds-ceph-1.novalocal_12560_object201
benchmark_data_sds-ceph-1.novalocal_12560_object5
benchmark_data_sds-ceph-1.novalocal_12560_object19
benchmark_data_sds-ceph-1.novalocal_12560_object133
benchmark_data_sds-ceph-1.novalocal_12560_object200
benchmark_data_sds-ceph-1.novalocal_12560_object127
benchmark_data_sds-ceph-1.novalocal_12560_object161
benchmark_data_sds-ceph-1.novalocal_12560_object92
benchmark_data_sds-ceph-1.novalocal_12560_object245
benchmark_data_sds-ceph-1.novalocal_12560_object6
benchmark_data_sds-ceph-1.novalocal_12560_object196
benchmark_data_sds-ceph-1.novalocal_12560_object243
benchmark_data_sds-ceph-1.novalocal_12560_object170
benchmark_data_sds-ceph-1.novalocal_12560_object117
benchmark_data_sds-ceph-1.novalocal_12560_object195
benchmark_data_sds-ceph-1.novalocal_12560_object125
benchmark_data_sds-ceph-1.novalocal_12560_object28
benchmark_data_sds-ceph-1.novalocal_12560_object4
benchmark_data_sds-ceph-1.novalocal_12560_object214
benchmark_data_sds-ceph-1.novalocal_12560_object167
benchmark_data_sds-ceph-1.novalocal_12560_object173
benchmark_data_sds-ceph-1.novalocal_12560_object153
benchmark_data_sds-ceph-1.novalocal_12560_object74
benchmark_data_sds-ceph-1.novalocal_12560_object227
benchmark_data_sds-ceph-1.novalocal_12560_object180
benchmark_data_sds-ceph-1.novalocal_12560_object93
benchmark_data_sds-ceph-1.novalocal_12560_object240
benchmark_data_sds-ceph-1.novalocal_12560_object53
benchmark_data_sds-ceph-1.novalocal_12560_object104
benchmark_data_sds-ceph-1.novalocal_12560_object175
benchmark_data_sds-ceph-1.novalocal_12560_object251
benchmark_data_sds-ceph-1.novalocal_12560_object97
benchmark_data_sds-ceph-1.novalocal_12560_object43
benchmark_data_sds-ceph-1.novalocal_12560_object230
benchmark_data_sds-ceph-1.novalocal_12560_object152
benchmark_data_sds-ceph-1.novalocal_12560_object166
benchmark_data_sds-ceph-1.novalocal_12560_object204
benchmark_data_sds-ceph-1.novalocal_12560_object124
benchmark_data_sds-ceph-1.novalocal_12560_object228
benchmark_data_sds-ceph-1.novalocal_12560_object188
benchmark_data_sds-ceph-1.novalocal_12560_object122
benchmark_data_sds-ceph-1.novalocal_12560_object205
benchmark_data_sds-ceph-1.novalocal_12560_object86
benchmark_data_sds-ceph-1.novalocal_12560_object208
benchmark_data_sds-ceph-1.novalocal_12560_object171
benchmark_data_sds-ceph-1.novalocal_12560_object206
benchmark_data_sds-ceph-1.novalocal_12560_object119
benchmark_data_sds-ceph-1.novalocal_12560_object26
benchmark_data_sds-ceph-1.novalocal_12560_object113
benchmark_data_sds-ceph-1.novalocal_12560_object238
benchmark_data_sds-ceph-1.novalocal_12560_object210
benchmark_data_sds-ceph-1.novalocal_12560_object140
benchmark_data_sds-ceph-1.novalocal_12560_object14
benchmark_data_sds-ceph-1.novalocal_12560_object112
benchmark_data_sds-ceph-1.novalocal_12560_object185
benchmark_data_sds-ceph-1.novalocal_12560_object17
benchmark_data_sds-ceph-1.novalocal_12560_object23
benchmark_data_sds-ceph-1.novalocal_12560_object207
benchmark_data_sds-ceph-1.novalocal_12560_object44
benchmark_data_sds-ceph-1.novalocal_12560_object12
benchmark_data_sds-ceph-1.novalocal_12560_object157
benchmark_data_sds-ceph-1.novalocal_12560_object197
benchmark_data_sds-ceph-1.novalocal_12560_object141
benchmark_data_sds-ceph-1.novalocal_12560_object226
benchmark_data_sds-ceph-1.novalocal_12560_object80
benchmark_data_sds-ceph-1.novalocal_12560_object85
benchmark_data_sds-ceph-1.novalocal_12560_object72
benchmark_data_sds-ceph-1.novalocal_12560_object13
benchmark_data_sds-ceph-1.novalocal_12560_object45
benchmark_data_sds-ceph-1.novalocal_12560_object121
benchmark_data_sds-ceph-1.novalocal_12560_object216
benchmark_data_sds-ceph-1.novalocal_12560_object61
benchmark_data_sds-ceph-1.novalocal_12560_object79
benchmark_data_sds-ceph-1.novalocal_12560_object131
benchmark_data_sds-ceph-1.novalocal_12560_object105
benchmark_data_sds-ceph-1.novalocal_12560_object54
benchmark_data_sds-ceph-1.novalocal_12560_object221
benchmark_data_sds-ceph-1.novalocal_12560_object59
benchmark_data_sds-ceph-1.novalocal_12560_object10
benchmark_data_sds-ceph-1.novalocal_12560_object176
benchmark_data_sds-ceph-1.novalocal_12560_object199
benchmark_data_sds-ceph-1.novalocal_12560_object233
benchmark_data_sds-ceph-1.novalocal_12560_object146
benchmark_data_sds-ceph-1.novalocal_12560_object231
benchmark_data_sds-ceph-1.novalocal_12560_object189
benchmark_data_sds-ceph-1.novalocal_12560_object203
benchmark_data_sds-ceph-1.novalocal_12560_object30
benchmark_data_sds-ceph-1.novalocal_12560_object234
benchmark_data_sds-ceph-1.novalocal_12560_object184
benchmark_data_sds-ceph-1.novalocal_12560_object1
benchmark_data_sds-ceph-1.novalocal_12560_object32
benchmark_data_sds-ceph-1.novalocal_12560_object244
benchmark_data_sds-ceph-1.novalocal_12560_object129
benchmark_data_sds-ceph-1.novalocal_12560_object35
benchmark_data_sds-ceph-1.novalocal_12560_object151
benchmark_data_sds-ceph-1.novalocal_12560_object25
benchmark_data_sds-ceph-1.novalocal_12560_object102
benchmark_data_sds-ceph-1.novalocal_12560_object9
benchmark_data_sds-ceph-1.novalocal_12560_object136
benchmark_data_sds-ceph-1.novalocal_12560_object70
benchmark_data_sds-ceph-1.novalocal_12560_object91
benchmark_data_sds-ceph-1.novalocal_12560_object99
benchmark_data_sds-ceph-1.novalocal_12560_object177
benchmark_data_sds-ceph-1.novalocal_12560_object248
benchmark_data_sds-ceph-1.novalocal_12560_object163
benchmark_data_sds-ceph-1.novalocal_12560_object60
benchmark_data_sds-ceph-1.novalocal_12560_object21
benchmark_data_sds-ceph-1.novalocal_12560_object110
benchmark_data_sds-ceph-1.novalocal_12560_object215
benchmark_data_sds-ceph-1.novalocal_12560_object27
benchmark_data_sds-ceph-1.novalocal_12560_object193
benchmark_data_sds-ceph-1.novalocal_12560_object118
benchmark_data_sds-ceph-1.novalocal_12560_object29
benchmark_data_sds-ceph-1.novalocal_12560_object224
benchmark_data_sds-ceph-1.novalocal_12560_object67
benchmark_data_sds-ceph-1.novalocal_12560_object123
benchmark_data_sds-ceph-1.novalocal_12560_object191
benchmark_data_sds-ceph-1.novalocal_12560_object8
benchmark_data_sds-ceph-1.novalocal_12560_object183
benchmark_data_sds-ceph-1.novalocal_12560_object94
benchmark_data_sds-ceph-1.novalocal_12560_object82
benchmark_data_sds-ceph-1.novalocal_12560_object186
benchmark_data_sds-ceph-1.novalocal_12560_object111
benchmark_data_sds-ceph-1.novalocal_12560_object120
benchmark_data_sds-ceph-1.novalocal_12560_object130
benchmark_data_sds-ceph-1.novalocal_12560_object115
benchmark_data_sds-ceph-1.novalocal_12560_object40
benchmark_data_sds-ceph-1.novalocal_12560_object37
benchmark_last_metadata
benchmark_data_sds-ceph-1.novalocal_12560_object84
benchmark_data_sds-ceph-1.novalocal_12560_object232
benchmark_data_sds-ceph-1.novalocal_12560_object47
benchmark_data_sds-ceph-1.novalocal_12560_object68
benchmark_data_sds-ceph-1.novalocal_12560_object48
benchmark_data_sds-ceph-1.novalocal_12560_object145
benchmark_data_sds-ceph-1.novalocal_12560_object20
benchmark_data_sds-ceph-1.novalocal_12560_object75
benchmark_data_sds-ceph-1.novalocal_12560_object66
benchmark_data_sds-ceph-1.novalocal_12560_object83
benchmark_data_sds-ceph-1.novalocal_12560_object223
benchmark_data_sds-ceph-1.novalocal_12560_object114
benchmark_data_sds-ceph-1.novalocal_12560_object7
benchmark_data_sds-ceph-1.novalocal_12560_object187
benchmark_data_sds-ceph-1.novalocal_12560_object2
benchmark_data_sds-ceph-1.novalocal_12560_object58
benchmark_data_sds-ceph-1.novalocal_12560_object222
benchmark_data_sds-ceph-1.novalocal_12560_object249
benchmark_data_sds-ceph-1.novalocal_12560_object137
benchmark_data_sds-ceph-1.novalocal_12560_object178
benchmark_data_sds-ceph-1.novalocal_12560_object159
benchmark_data_sds-ceph-1.novalocal_12560_object24
benchmark_data_sds-ceph-1.novalocal_12560_object213
benchmark_data_sds-ceph-1.novalocal_12560_object181
benchmark_data_sds-ceph-1.novalocal_12560_object108
benchmark_data_sds-ceph-1.novalocal_12560_object246
benchmark_data_sds-ceph-1.novalocal_12560_object148
benchmark_data_sds-ceph-1.novalocal_12560_object64
benchmark_data_sds-ceph-1.novalocal_12560_object103
benchmark_data_sds-ceph-1.novalocal_12560_object109
benchmark_data_sds-ceph-1.novalocal_12560_object107
benchmark_data_sds-ceph-1.novalocal_12560_object132
benchmark_data_sds-ceph-1.novalocal_12560_object78
benchmark_data_sds-ceph-1.novalocal_12560_object98
benchmark_data_sds-ceph-1.novalocal_12560_object89
benchmark_data_sds-ceph-1.novalocal_12560_object142
benchmark_data_sds-ceph-1.novalocal_12560_object158
benchmark_data_sds-ceph-1.novalocal_12560_object100
benchmark_data_sds-ceph-1.novalocal_12560_object144
benchmark_data_sds-ceph-1.novalocal_12560_object168
benchmark_data_sds-ceph-1.novalocal_12560_object139
benchmark_data_sds-ceph-1.novalocal_12560_object250
benchmark_data_sds-ceph-1.novalocal_12560_object194
benchmark_data_sds-ceph-1.novalocal_12560_object182
benchmark_data_sds-ceph-1.novalocal_12560_object55
benchmark_data_sds-ceph-1.novalocal_12560_object41
benchmark_data_sds-ceph-1.novalocal_12560_object143
benchmark_data_sds-ceph-1.novalocal_12560_object165
benchmark_data_sds-ceph-1.novalocal_12560_object169
benchmark_data_sds-ceph-1.novalocal_12560_object241
benchmark_data_sds-ceph-1.novalocal_12560_object192
benchmark_data_sds-ceph-1.novalocal_12560_object147
benchmark_data_sds-ceph-1.novalocal_12560_object239
benchmark_data_sds-ceph-1.novalocal_12560_object95
benchmark_data_sds-ceph-1.novalocal_12560_object57
benchmark_data_sds-ceph-1.novalocal_12560_object155
benchmark_data_sds-ceph-1.novalocal_12560_object42
benchmark_data_sds-ceph-1.novalocal_12560_object106
benchmark_data_sds-ceph-1.novalocal_12560_object198
benchmark_data_sds-ceph-1.novalocal_12560_object218
benchmark_data_sds-ceph-1.novalocal_12560_object15
benchmark_data_sds-ceph-1.novalocal_12560_object63

```





