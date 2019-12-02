---
title: Ceph iSCSI gateway 安装和配置
date: 2019-11-27 21:07:15
tags: ceph
---

# 使用ANSI配置ISCSI TARGET

Ceph iSCSI gateway是iSCSI target node，也是Ceph client node。 Ceph iSCSI gateway可以是独立节点，也可以位于Ceph Object Store Disk (OSD) node上。 完成以下步骤，将安装Ceph iSCSI gateway并将其配置为基本操作。

**要求：**

- 正在运行的Ceph Luminous（12.2.x）集群或更高版本
- Red Hat Enterprise Linux/CentOS 7.5（或更高版本）； Linux内核v4.16（或更高版本）
- 在所有iSCSI gateway节点上安装ceph-iscsi package

**安装：**

在Ansible installer节点（可以是管理节点也可以是专用部署节点）上，执行以下步骤：

1、以root用户身份安装ceph-ansible package：

```
# yum install ceph-ansible
```

2、在/etc/ansible/hosts文件中为gateway group添加一个entry：

```
[iscsigws]
ceph-igw-1
ceph-igw-2
```

注意如果将iSCSI gateway与OSD部署同一节点，则将OSD节点添加到[iscsigws] section。

**配置：**

ceph-ansible package在/usr/share/ceph-ansible/group_vars/目录中放置了一个名为iscsigws.yml.sample的文件。 创建此示例文件的副本iscsigws.yml。 查看以下Ansible变量和说明，并进行相应更新。 有关高级变量的完整列表，请参见iscsigws.yml.sample。

| Variable（变量）        | Meaning/Purpose（含义/目的）                                 |
| :---------------------- | :----------------------------------------------------------- |
| `seed_monitor`          | 每个gateway都需要访问ceph集群以进行rados和rbd calls。 这意味着iSCSI gateway必须已定义适当的`/etc/ceph/`目录。 `seed_monitor` host用于填充iSCSI gateway的`/etc/ceph/`目录。 |
| `cluster_name`          | 自定义存储集群名称（默认为ceph）                             |
| `gateway_keyring`       | Define a custom keyring name.                                |
| `deploy_settings`       | 如果设置为`true`，则在运行playbook时deploy the settings。    |
| `perform_system_checks` | 这是一个布尔值，用于检查每个gateway上的multipath和lvm configuration settings。 必须至少在第一次运行时将其设置为true，以确保正确配置了multipathd和lvm。 |
| `api_user`              | API的用户名。 默认值为admin。                                |
| `api_password`          | 使用API的密码。 默认值为admin。                              |
| `api_port`              | 使用API的TCP端口号。 默认值为5000。                          |
| `api_secure`            | 如果必须使用TLS，则为True。 默认为false。 如果为true，则用户必须创建必要的certificate和key files。 有关详细信息，请参见gwcli man文件。 |
| `trusted_ip_list`       | 有权访问API的IPv4或IPv6地址的列表。 默认情况下，只有iSCSI gateway节点可以访问。 |

**Deploying:**

在Ansible installer节点上，执行以下步骤。

1、以root用户身份执行Ansible playbook：

```
# cd /usr/share/ceph-ansible
# ansible-playbook site.yml --limit iscsigws
```

注意Ansible playbook将处理RPM dependencies，设置daemons并安装gwcli，因此可用于创建iSCSI targets并将RBD images导出为LUN。 在以前的版本中，iscsigws.yml可以定义iSCSI target和其他objects，如clients, images和LUNs, 但现在不再支持该功能。

2、从iSCSI gateway节点验证配置：

```
# gwcli ls
```

请参阅[Configuring the iSCSI Target using the Command Line Interface](https://docs.ceph.com/docs/master/rbd/iscsi-target-cli)，使用gwcli工具创建gateways, LUNs和clients。

```
重要提示尝试使用targetcli工具更改配置将导致以下问题，例如ALUA配置错误和路径故障转移问题。 可能会损坏数据，configuration across iSCSI gateways不匹配，WWN information不匹配，这将导致client multipath问题。
```

**Service Management:**

ceph-iscsi package安装configuration management logic和一个名为rbd-target-api的Systemd service。启用Systemd service后，rbd-target-api将在引导时启动，并将恢复Linux IO状态。Ansible playbook会在部署期间禁用target service。以下是与rbd-target-api Systemd服务交互命令。

```
# systemctl <start|stop|restart|reload> rbd-target-api
```

- reload

reload request将强制rbd-target-api重新读取配置并将其应用于当前正在运行的环境。 通常不需要这样做，因为changes是从Ansible并行部署到所有iSCSI gateway节点的。

- stop

stop request将关闭gateway的portal interfaces，断开与客户端的连接，并从内核中清除当前的LIO配置。这将使iSCSI gateway返回到clean状态。 当客户端断开连接时，客户端multipathing layer会将active I/O（活动的I/O）重新安排到其他iSCSI gateways。

**Removing the Configuration:**

ceph-ansible package提供了Ansible手册，可删除iSCSI gateway配置和相关的RBD images。 Ansible playbook是/usr/share/ceph-ansible/purge_gateways.yml。 运行此Ansible playbook时，系统会提示您执行清除的类型：

*lio* :

在此模式下，将在已定义的所有iSCSI gateways上清除LIO配置。 在Ceph storage集群中，创建的Disks保持不变。

*all* :

选择all后，将删除LIO配置以及在iSCSI gateway环境中定义的所有RBD images，其他不相关的RBD images将不会删除。

```
警告清除操作是对iSCSI gateway环境的破坏性操作。

警告如果RBD images具有snapshots或clones并通过Ceph iSCSI gateway导出，则清除操作将失败。
```

```
[root@rh7-iscsi-client ceph-ansible]# ansible-playbook purge_gateways.yml
Which configuration elements should be purged? (all, lio or abort) [abort]: all


PLAY [Confirm removal of the iSCSI gateway configuration] *********************


GATHERING FACTS ***************************************************************
ok: [localhost]


TASK: [Exit playbook if user aborted the purge] *******************************
skipping: [localhost]


TASK: [set_fact ] *************************************************************
ok: [localhost]


PLAY [Removing the gateway configuration] *************************************


GATHERING FACTS ***************************************************************
ok: [ceph-igw-1]
ok: [ceph-igw-2]


TASK: [igw_purge | purging the gateway configuration] *************************
changed: [ceph-igw-1]
changed: [ceph-igw-2]


TASK: [igw_purge | deleting configured rbd devices] ***************************
changed: [ceph-igw-1]
changed: [ceph-igw-2]


PLAY RECAP ********************************************************************
ceph-igw-1                 : ok=3    changed=2    unreachable=0    failed=0
ceph-igw-2                 : ok=3    changed=2    unreachable=0    failed=0
localhost                  : ok=2    changed=0    unreachable=0    failed=0
```

# 使用COMMAND LINE INTERFACE配置ISCSI TARGET

Ceph iSCSI gateway是iSCSI target节点，也是Ceph client节点。 Ceph iSCSI gateway可以是独立节点，也可以位于Ceph Object Store Disk (OSD)节点上。 完成以下步骤，将安装Ceph iSCSI gateway并将其配置为基本操作。

**Requirements:**

- 正在运行的Ceph Luminous（12.2.x）集群或更高版本
- Red Hat Enterprise Linux/CentOS 7.5（或更高版本）； Linux内核v4.16（或更高版本）
- 必须从Linux发行版的软件repository中安装以下软件包：
  - targetcli-2.1.fb47 or newer package
  - python-rtslib-2.1.fb68 or newer package
  - tcmu-runner-1.4.0 or newer package
  - ceph-iscsi-3.2 or newer package

```
重要说明：如果存在这些packages的先前版本，则必须在安装较新版本之前首先将其删除。
```

在继续Installing section之前，请在Ceph iSCSI gateway节点上执行以下步骤：

1、如果Ceph iSCSI gateway未在OSD节点上，则将/etc/ceph/中的Ceph配置文件复制到iSCSI gateway 节点。 Ceph配置文件必须存在于/etc/ceph/下的iSCSI gateway节点上。

2、安装和配置[Ceph Command-line Interface](http://docs.ceph.com/docs/master/start/quick-rbd/#install-ceph)

3、请在防火墙上打开TCP端口3260和5000。

```
注意对端口5000的访问应仅限于受信任的内部网络或仅使用gwcli或正在运行ceph-mgr daemons的单个主机。
```

4、创建一个新的或使用现有的RADOS Block Device (RBD)。

**Installing:**

如果您使用upstream ceph-iscsi package，请遵循 [manual install instructions](https://docs.ceph.com/docs/master/rbd/iscsi-target-cli-manual-install)。

对于基于rpm的指令，请执行以下命令：

1、以root用户身份，在所有iSCSI gateway节点上，安装ceph-iscsi package：

```
# yum install ceph-iscsi
```

2、以root用户身份，在所有iSCSI gateway节点上，安装tcmu-runner package：

```
# yum install tcmu-runner
```

**Setup:**

1、gwcli需要一个名称为rbd的pool，因此它可以存储iSCSI配置之类的元数据。 要检查是否已创建此pool，请运行：  

```
# ceph osd lspools
```

如果不存在，则可以在[RADOS pool operations page](http://docs.ceph.com/docs/master/rados/operations/pools/)上找到创建pool的说明。

2、在iSCSI gateway节点上，以root身份在/etc/ceph/目录中创建一个名为iscsi-gateway.cfg的文件：

```
# touch /etc/ceph/iscsi-gateway.cfg
```

2.1、编辑iscsi-gateway.cfg文件并添加以下行：

```
[config]
# Name of the Ceph storage cluster. A suitable Ceph configuration file allowing
# access to the Ceph storage cluster from the gateway node is required, if not
# colocated on an OSD node.
cluster_name = ceph

# Place a copy of the ceph cluster's admin keyring in the gateway's /etc/ceph
# drectory and reference the filename here
gateway_keyring = ceph.client.admin.keyring


# API settings.
# The API supports a number of options that allow you to tailor it to your
# local environment. If you want to run the API under https, you will need to
# create cert/key files that are compatible for each iSCSI gateway node, that is
# not locked to a specific node. SSL cert and key files *must* be called
# 'iscsi-gateway.crt' and 'iscsi-gateway.key' and placed in the '/etc/ceph/' directory
# on *each* gateway node. With the SSL files in place, you can use 'api_secure = true'
# to switch to https mode.

# To support the API, the bear minimum settings are:
api_secure = false

# Additional API configuration options are as follows, defaults shown.
# api_user = admin
# api_password = admin
# api_port = 5001
# trusted_ip_list = 192.168.0.10,192.168.0.11



# ------------------------------------------------------
# 翻译如下
[config]
＃Ceph存储集群的名称。如果不位于OSD节点上，则需要一个合适的Ceph配置文件，该文件允许从gateway节点访问Ceph存储群集。
cluster_name = ceph

＃将ceph集群的admin keyring的副本放置在gateway的/etc/ceph文件夹中，并在此处引用filename
gateway_keyring = ceph.client.admin.keyring


＃API设置。
＃API支持许多选项，可让您根据本地环境进行定制。如果要在https下运行API，则需要为每个iSCSI gateway节点创建兼容的cert/key文件，该节点未锁定到特定节点。必须将SSL cert和key文件命名为'iscsi-gateway.crt'和'iscsi-gateway.key'，并放置在每个gateway节点上的'/etc/ceph/'目录中。放置好SSL文件后，您可以使用'api_secure = true'切换到https模式。

＃为了支持API，至少需要配置如下：
api_secure = false

＃其他API配置选项如下，显示的默认值。（trusted翻译为信任）
# api_user = admin
# api_password = admin
# api_port = 5001
# trusted_ip_list = 192.168.0.10,192.168.0.11
```

注意trusted_ip_list是每个iscsi gateway上IP地址的列表，将用于管理操作，如创建target，lun导出等。该IP可以与用于iSCSI data的IP相同，例如与RBD image之间的READ/WRITE命令，但建议使用单独的IP。

```
重要说明：在所有iSCSI gateway节点上，iscsi-gateway.cfg文件必须相同。
```

2.2、以root用户身份将iscsi-gateway.cfg文件复制到所有iSCSI gateway节点。

2.3、以root用户身份，在所有iSCSI gateway节点上，启用并启动API服务：

```
# systemctl daemon-reload
# systemctl enable rbd-target-api
# systemctl start rbd-target-api
```

**Configuring:**

gwcli将创建和配置iSCSI target和RBD images，并在上一部分中的gateways设置之间复制配置。 较低级别的工具（例如targetcli和rbd）可用于查询本地配置，但不应用于对其进行修改。 下一节将演示如何创建iSCSI target并将RBD image导出为LUN 0。

1、以root用户身份，在iSCSI gateway节点上，启动iSCSI gateway command-line interface：

```
# gwcli
```

2、转到iscsi-targets并创建名为iqn.2003-01.com.redhat.iscsi-gw:iscsi-igw的目标：

```
> /> cd /iscsi-target
> /iscsi-target>  create iqn.2003-01.com.redhat.iscsi-gw:iscsi-igw
```

3、创建iSCSI gateways。 下面使用的IP是用于iSCSI data（如READ和WRITE命令）的IP。 它们可以是trusted_ip_list中列出的用于管理操作相同的IP，但是建议使用不同的IP。

```
> /iscsi-target> cd iqn.2003-01.com.redhat.iscsi-gw:iscsi-igw/gateways
> /iscsi-target...-igw/gateways>  create ceph-gw-1 10.172.19.21
> /iscsi-target...-igw/gateways>  create ceph-gw-2 10.172.19.22
```

如果不使用RHEL/CentOS或使用upstream或ceph-iscsi-test kernel，则必须使用skipchecks=true参数。 这将避免Red Hat kernel和rpm checks：

```
> /iscsi-target> cd iqn.2003-01.com.redhat.iscsi-gw:iscsi-igw/gateways
> /iscsi-target...-igw/gateways>  create ceph-gw-1 10.172.19.21 skipchecks=true
> /iscsi-target...-igw/gateways>  create ceph-gw-2 10.172.19.22 skipchecks=true
```

4、在rbd pool中添加名称为disk_1的RBD image：

```
> /iscsi-target...-igw/gateways> cd /disks
> /disks> create pool=rbd image=disk_1 size=90G
```

5、创建一个客户端，使用initiator名称iqn.1994-05.com.redhat:rh7-client：

```
> /disks> cd /iscsi-target/iqn.2003-01.com.redhat.iscsi-gw:iscsi-igw/hosts
> /iscsi-target...eph-igw/hosts>  create iqn.1994-05.com.redhat:rh7-client
```

6、将客户端的CHAP用户名设置为myiscsiusername，将密码设置为myiscsipassword：

```
> /iscsi-target...at:rh7-client>  auth username=myiscsiusername password=myiscsipassword
```

警告必须始终配置CHAP。 如果没有CHAP，target将拒绝任何登录请求。

7、将disk添加到客户端：

```
> /iscsi-target...at:rh7-client> disk add rbd/disk_1
```

下一步是配置iSCSI initiators。

#  手动安装CEPH-ISCSI

**Requirements**

要完成ceph-iscsi的安装，有4个步骤：

1、从Linux发行版的软件repository安装common packages

2、安装Git以直接从其Git repositories中获取其余packages

3、确保使用兼容的kernel

4、安装ceph-iscsi的所有组件并启动相关的daemons：

- tcmu-runner
- rtslib-fb
- configshell-fb
- targetcli-fb
- ceph-iscsi

### 1、安装COMMON PACKAGES

ceph-iscsi和target工具将使用以下packages。 必须从Linux发行版的软件repository中将它们安装在将成为iSCSI gateway的每台计算机上：

- libnl3
- libkmod
- librbd1
- pyparsing
- python kmod
- python pyudev
- python gobject
- python urwid
- python pyparsing
- python rados
- python rbd
- python netifaces
- python crypto
- python requests
- python flask
- pyOpenSSL

### 2、安装GIT

为了安装使用Ceph运行iSCSI所需的所有packages，您需要使用Git直接从其repository中下载它们。 在CentOS/RHEL上执行：

```
> sudo yum install git
```

在Debian/Ubuntu上执行：

```
> sudo apt install git
```

要了解有关Git及其工作方式的更多信息，请访问https://git-scm.com

### 3、确保使用了兼容的KERNEL

确保使用支持Ceph iSCSI patches的kernel：

- 具有内核v4.16或更高版本的所有Linux发行版
- Red Hat Enterprise Linux或CentOS 7.5更高版本

如果您已经在使用兼容的kernel，则可以转到下一步。 但是，如果您不使用兼容的kernel，请查看发行版的文档以获取有关如何构建此kernel的特定说明。 唯一的Ceph iSCSI特定要求是必须启用以下构建选项：

```
CONFIG_TARGET_CORE=m
CONFIG_TCM_USER2=m
CONFIG_ISCSI_TARGET=m
```

### 4、安装CEPH-ISCSI

最后，可以直接从其Git repositories中获取其余工具，并启动其相关服务

**TCMU-RUNNER**

Installation:

```
> git clone https://github.com/open-iscsi/tcmu-runner
> cd tcmu-runner
```

运行以下命令以安装所有必需的依赖项：

  ```
> ./extra/install_dep.sh
  ```

现在，您可以构建tcmu-runner。 为此，请使用以下构建命令：

```
> cmake -Dwith-glfs=false -Dwith-qcow=false -DSUPPORT_SYSTEMD=ON -DCMAKE_INSTALL_PREFIX=/usr
> make install
```

启用并启动守护程序：

```
> systemctl daemon-reload
> systemctl enable tcmu-runner
> systemctl start tcmu-runner
```

**RTSLIB-FB**

Installation:

```
> git clone https://github.com/open-iscsi/rtslib-fb.git
> cd rtslib-fb
> python setup.py install
```

**CONFIGSHELL-FB**

Installation:

```
> git clone https://github.com/open-iscsi/configshell-fb.git
> cd configshell-fb
> python setup.py install
```

**TARGETCLI-FB**

Installation:

```
> git clone https://github.com/open-iscsi/targetcli-fb.git
> cd targetcli-fb
> python setup.py install
> mkdir /etc/target
> mkdir /var/target
```

警告ceph-iscsi工具假定它们正在管理系统上的所有targets。 如果已设置targets并由targetcli管理，则必须禁用targets服务。

**CEPH-ISCSI**
Installation:

```
> git clone https://github.com/ceph/ceph-iscsi.git
> cd ceph-iscsi
> python setup.py install --install-scripts=/usr/bin
> cp usr/lib/systemd/system/rbd-target-gw.service /lib/systemd/system
> cp usr/lib/systemd/system/rbd-target-api.service /lib/systemd/system
```

启用并启动daemon：

```
> systemctl daemon-reload
> systemctl enable rbd-target-gw
> systemctl start rbd-target-gw
> systemctl enable rbd-target-api
> systemctl start rbd-target-api
```

安装完成。 进入[main ceph-iscsi CLI page](https://docs.ceph.com/docs/master/rbd/iscsi-target-cli)上的setup部分。