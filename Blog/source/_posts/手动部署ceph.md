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







