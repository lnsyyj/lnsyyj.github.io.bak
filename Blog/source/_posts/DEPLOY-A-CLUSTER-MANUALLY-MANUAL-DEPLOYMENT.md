---
title: DEPLOY A CLUSTER MANUALLY - MANUAL DEPLOYMENT
date: 2017-12-03 18:20:19
tags: CEPH-Luminous
---

所有Ceph集群都至少需要一个monitor，并且至少需要与群集上存储的对象的副本一样多的OSD。 Bootstrapping the initial monitor(s) 是部署Ceph存储集群的第一步。Monitor部署还为整个群集设置了重要的标准，例如池副本的数量，每个OSD的placement groups数量，心跳间隔，是否需要身份验证等。大多数这些值是默认设置的，因此在生产环境设置集群时了解这些值是有用的。

按照与Installation (Quick)`http://docs.ceph.com/docs/master/start/`相同的配置，我们将建立一个集群，node1为monitor节点，node2和node3为OSD节点。

![](/images/DEPLOY_A_CLUSTER_MANUALLY___MANUAL_DEPLOYMENT.png)

**MONITOR BOOTSTRAPPING**

Bootstrapping a monitor（Ceph存储集群，理论上）需要一些东西：

- Unique Identifier: fsid是集群的唯一标识符，当Ceph存储集群主要用于Ceph Filesystem时，代表从日期生成的File System ID
- Cluster Name: Ceph集群有一个集群名称，这是一个没有空格的简单字符串。默认群集名称是ceph，但是您可以指定不同的群集名称。当您使用多个群集时，您需要清楚地了解您正在使用哪个群集，这时覆盖默认的集群名称特别有用。例如，当您在federated architecture中运行多个群集时，群集名称（例如，us-west，us-east）标识当前CLI会话的群集。注：要在命令行接口上标识集群名称，请指定具有群集名称的Ceph配置文件（例如，ceph.conf，us-west.conf，us-east.conf等）。另请参阅CLI使用（ceph --cluster {cluster-name}）。
- Monitor Name: 群集中的每个monitor实例都有唯一的名称。通常情况下，Ceph Monitor名称是主机名（我们推荐每个单独的主机使用一个Ceph Monitor，Ceph OSD Daemons不能混合部署Ceph Monitors）。您可以通过hostname -s检索short hostname。
- Monitor Map: Bootstrapping the initial monitor(s)需要您生成一个monitor map。monitor map需要fsid，群集名称（或使用默认值）以及至少一个主机名和它的IP地址。
- Monitor Keyring: Monitors通过secret key相互通信。当bootstrapping the initial monitor(s)时，您必须生成一个带有monitor secret的keyring并提供它。
- Administrator Keyring: 要使用ceph CLI工具，你必须有一个client.admin user。因此，您必须生成admin user和keyring，并且还必须将client.admin user添加到monitor keyring中。

上述要求并不意味着一个Ceph配置文件的建立。但是，作为最佳做法，我们建议创建一个Ceph配置文件并使用fsid，mon initial members和mon host配置填充它。

您也可以在运行时获取并设置所有的monitor settings。但是，Ceph配置文件可能只包含那些覆盖默认值的settings。将settings添加到Ceph配置文件时，这些settings会覆盖默认settings。在Ceph配置文件中维护这些settings可以更容易地维护集群。

步骤如下：

1、登录到initial monitor node(s)：

    ssh {hostname}

For example:

    ssh node1

2、确保你有一个Ceph配置文件的目录。默认情况下，Ceph使用/etc/ceph。安装ceph时，安装程序将自动创建/etc/ceph目录。

    ls /etc/ceph

注意：清除群集时，部署工具可能会删除此目录（例如，ceph-deploy purgedata {node-name}, ceph-deploy purge {node-name}）。

3、创建一个Ceph配置文件。默认情况下，Ceph使用ceph.conf，其中ceph反映了集群名称。

    sudo vim /etc/ceph/ceph.conf

4、为您的群集生成一个唯一的ID（即fsid）。

    uuidgen

5、添加唯一的ID到您的Ceph配置文件。

    fsid = {UUID}

For example:

    fsid = a7f64266-0894-4f1e-a635-d0aeaca0e993

6、将initial monitor(s)添加到您的Ceph配置文件中。

    mon initial members = {hostname}[,{hostname}]

For example:

    mon initial members = node1

7、将initial monitor(s)的IP地址添加到您的Ceph配置文件并保存该文件。

    mon host = {ip-address}[,{ip-address}]

For example:

    mon host = 192.168.0.1

注意：您可以使用IPv6地址而不是IPv4地址，但必须将ms bind ipv6设置为true。有关网络配置的详情，请参阅Network Configuration Reference`http://docs.ceph.com/docs/master/rados/configuration/network-config-ref/`。

8、为您的群集创建一个keyring并生成一个monitor secret key。

    ceph-authtool --create-keyring /tmp/ceph.mon.keyring --gen-key -n mon. --cap mon 'allow *'

9、生成一个administrator keyring，生成一个client.admin user并将user添加到该keyring中。

	sudo ceph-authtool --create-keyring /etc/ceph/ceph.client.admin.keyring --gen-key -n client.admin --set-uid=0 --cap mon 'allow *' --cap osd 'allow *' --cap mds 'allow *' --cap mgr 'allow *'

10、生成一个bootstrap-osd keyring，生成一个client.bootstrap-osd user并将user添加到该keyring。

	sudo ceph-authtool --create-keyring /var/lib/ceph/bootstrap-osd/ceph.keyring --gen-key -n client.bootstrap-osd --cap mon 'profile bootstrap-osd'

11、将生成的keys添加到ceph.mon.keyring。

	sudo ceph-authtool /tmp/ceph.mon.keyring --import-keyring /etc/ceph/ceph.client.admin.keyring
	sudo ceph-authtool /tmp/ceph.mon.keyring --import-keyring /var/lib/ceph/bootstrap-osd/ceph.keyring

12、使用hostname(s), host IP address(es)和FSID生成monitor map。 将其另存为/tmp/monmap：

	monmaptool --create --add {hostname} {ip-address} --fsid {uuid} /tmp/monmap

For example:

	monmaptool --create --add node1 192.168.0.1 --fsid a7f64266-0894-4f1e-a635-d0aeaca0e993 /tmp/monmap

13、在monitor host(s)上创建一个默认数据目录（或多个目录）。

	sudo mkdir /var/lib/ceph/mon/{cluster-name}-{hostname}
For example:

	sudo -u ceph mkdir /var/lib/ceph/mon/ceph-node1

有关详细信息，请参阅Monitor Config Reference - Data `http://docs.ceph.com/docs/master/rados/configuration/mon-config-ref/#data`。

14、向monitor daemon(s)填充monitor map 和 keyring。

	sudo -u ceph ceph-mon [--cluster {cluster-name}] --mkfs -i {hostname} --monmap /tmp/monmap --keyring /tmp/ceph.mon.keyring

For example:

	sudo -u ceph ceph-mon --mkfs -i node1 --monmap /tmp/monmap --keyring /tmp/ceph.mon.keyring

15、考虑配置Ceph configuration file。 常用settings包括以下内容：

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

在前面的示例中，配置的[global]部分可能如下所示：

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

16、touch the done 文件。
标记monitor已被创建并准备好启动：

	sudo touch /var/lib/ceph/mon/ceph-node1/done

17、启动monitor(s).
对于大多数发行版，现在通过systemd启动服务：

	sudo systemctl start ceph-mon@node1

对于Ubuntu Trusty，请使用Upstart：

	sudo start ceph-mon id=node1 [cluster={cluster-name}]

在这种情况下，要在每次重新启动时启动守护进程，必须创建两个空文件，如下所示：

	sudo touch /var/lib/ceph/mon/{cluster-name}-{hostname}/upstart

For example:

	sudo touch /var/lib/ceph/mon/ceph-node1/upstart

对于以前的Debian/CentOS/RHEL，使用sysvinit：

	sudo /etc/init.d/ceph start mon.node1

18、确认monitor正在运行。

	ceph -s

您应该看到monitor已启动并正在运行的输出，并且您应该看到一个health error，指出placement groups处于非活动状态（inactive）。它应该看起来像这样：

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

注意：一旦添加OSD并启动它们后，placement group health errors应该消失。 有关详细信息，请参阅Adding OSDs`http://docs.ceph.com/docs/master/install/manual-deployment/#adding-osds`。

**MANAGER DAEMON CONFIGURATION**

在运行ceph-mon daemon的每个node上，也应该设置一个ceph-mgr daemon。
请参阅ceph-mgr administrator's guide`http://docs.ceph.com/docs/master/mgr/administrator/#mgr-administrator-guide`

ADDING OSDS

一旦你的initial monitor(s)运行，你就应该添加OSD。除非有足够的OSD来处理对象的副本数（例如，osd pool default size = 2至少需要两个OSD），否则您的集群无法达到active + clean状态。在bootstrapping你的monitor之后，您的群集有一个默认的CRUSH map; 然而，CRUSH map没有任何Ceph OSD Daemons映射到Ceph节点。

SHORT FORM

Ceph提供了ceph-disk实用程序，它可以用于Ceph的prepare disk,partition或directory。ceph-disk实用程序通过incrementing the index来创建OSD ID。 另外，ceph-disk会将新的OSD添加到主机下的CRUSH map中。执行ceph-disk -h获取CLI详细信息。ceph-disk实用程序可以自动执行下面的Long Form`http://docs.ceph.com/docs/master/install/manual-deployment/#long-form`的步骤。要使用short form的过程创建前两个OSD，请在node2和node3上执行以下操作：

1、Prepare the OSD

	ssh {node-name}
	sudo ceph-disk prepare --cluster {cluster-name} --cluster-uuid {uuid} {data-path} [{journal-path}]

For example:

	ssh node1
	sudo ceph-disk prepare --cluster ceph --cluster-uuid a7f64266-0894-4f1e-a635-d0aeaca0e993 --fs-type ext4 /dev/hdd1

2、Activate the OSD:

	sudo ceph-disk activate {data-path} [--activate-key {path}]

For example:

	sudo ceph-disk activate /dev/hdd1

注意：如果Ceph Node上没有/var/lib/ceph/bootstrap-osd/{cluster}.keyring副本，请使用--activate-key参数。

LONG FORM
没有任何帮助工具的好处，使用以下过程创建一个OSD并将其添加到群集和CRUSH map。要使用long form过程创建前两个OSD，请为每个OSD执行以下步骤。
注意：这个过程没有描述利用dm-crypt 'lockbox'部署dm-crypt。
1、Connect to the OSD host and become root.

	ssh {node-name}
	sudo bash

2、生成OSD的UUID。

	UUID=$(uuidgen)

3、为OSD生成一个cephx key。

	OSD_SECRET=$(ceph-authtool --gen-print-key)

4、创建OSD。请注意，如果您需要重新使用以前销毁的OSD ID，OSD ID可以作为ceph osd的附加参数提供。我们假设client.bootstrap-osd key存在于机器上。您也可以在存在该密钥的其他主机上以client.admin身份执行此命令：

	ID=$(echo "{\"cephx_secret\": \"$OSD_SECRET\"}" | \
	   ceph osd new $UUID -i - \
	   -n client.bootstrap-osd -k /var/lib/ceph/bootstrap-osd/ceph.keyring)

5、在新的OSD上创建默认目录。

	mkdir /var/lib/ceph/osd/ceph-$ID

6、如果OSD用于OS drive以外的drive（驱动器），准备与Ceph一起使用，并将其mount到您刚刚创建的目录中。

	mkfs.xfs /dev/{DEV}
	mount /dev/{DEV} /var/lib/ceph/osd/ceph-$ID

7、将secret（密钥）写入OSD keyring文件。

	ceph-authtool --create-keyring /var/lib/ceph/osd/ceph-$ID/keyring \
	     --name osd.$ID --add-key $OSD_SECRET

8、初始化OSD数据目录。

	ceph-osd -i $ID --mkfs --osd-uuid $UUID

9、Fix ownership.

	chown -R ceph:ceph /var/lib/ceph/osd/ceph-$ID

10、将OSD添加到Ceph后，OSD将处于您的配置中。 但是，它还没有运行。在它接收数据之前您必须先启动新的OSD。

For modern systemd distributions:

	systemctl enable ceph-osd@$ID
	systemctl start ceph-osd@$ID

For example:

	systemctl enable ceph-osd@12
	systemctl start ceph-osd@12

**ADDING MDS**

在下面的说明中，{id}是一个任意的名字，比如机器的主机名。
1、创建mds数据目录：

	mkdir -p /var/lib/ceph/mds/{cluster-name}-{id}

2、创建一个keyring

	ceph-authtool --create-keyring /var/lib/ceph/mds/{cluster-name}-{id}/keyring --gen-key -n mds.{id}

3、导入keyring并设置caps

	ceph auth add mds.{id} osd "allow rwx" mds "allow" mon "allow profile mds" -i /var/lib/ceph/mds/{cluster}-{id}/keyring

4、添加到ceph.conf

	[mds.{id}]
	host = {id}

5、手动启动daemon

	ceph-mds --cluster {cluster-name} -i {id} -m {mon-hostname}:{mon-port} [-f]

6、以正确的方式启动daemon（使用ceph.conf entry）

	service ceph start

7、如果启动daemon失败并出现此错误

	mds.-1.0 ERROR: failed to authenticate: (22) Invalid argument

确保在ceph.conf中的global部分没有设置keyring; 将其移动到client部分; 或添加keyring setting到特定mds daemon。并验证您看到的key与mds数据目录和ceph auth获取的mds.{id}输出相同。

8、现在你已经准备好创建一个Ceph filesystem`http://docs.ceph.com/docs/master/cephfs/createfs/`了。


**SUMMARY**

一旦启动并运行了monitor和两个OSD，您可以通过执行以下操作来观看placement groups peer：

	ceph -w

要查看tree，请执行以下操作：

	ceph osd tree

你应该看到如下所示的输出：

	id    weight  type name       up/down reweight
	-1      2       root default
	-2      2               host node1
	0       1                       osd.0   up      1
	-3      1               host node2
	1       1                       osd.1   up      1

要添加（或删除）其他monitors，请参阅Add/Remove Monitors`http://docs.ceph.com/docs/master/rados/operations/add-or-rm-mons/`。要添加（或删除）其他Ceph OSD Daemons，请参阅Add/Remove OSDs`http://docs.ceph.com/docs/master/rados/operations/add-or-rm-osds/`。