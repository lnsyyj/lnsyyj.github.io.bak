---
title: ceph-ansible
date: 2018-07-27 15:31:57
tags: Ansible
---

原文地址：http://docs.ceph.com/ceph-ansible/master/

# ceph-ansible

Ansible playbooks for Ceph, the distributed filesystem.

分布式文件系统，Ceph Ansible playbooks。

# Installation

您可以按照以下步骤直接从GitHub上的源安装：

Clone the repository:

```
git clone https://github.com/ceph/ceph-ansible.git
```

接下来，您必须决定使用哪个ceph-ansible branch。 有稳定的branch可供选择，或者您可以使用master branch：

```
git checkout $branch
```

接下来，使用pip和提供的requirements.txt来安装ansible和其他所需的python库：

```
pip install -r requirements.txt
```

## Ansible on RHEL and CentOS

您可以从 [Ansible channel](https://access.redhat.com/articles/3174981) 获取并安装RHEL和CentOS上的Ansible。

On RHEL: 

```
subscription-manager repos --enable=rhel-7-server-ansible-2-rpms
```

（CentOS不使用subscription-manager，默认情况下已启用“Extras” 。）

```
sudo yum install ansible
```

## Ansible on Ubuntu

您可以使用 [Ansible PPA](https://launchpad.net/~ansible/+archive/ubuntu/ansible) 在Ubuntu上获取Ansible。

```
sudo add-apt-repository ppa:ansible/ansible
sudo apt update
sudo apt install ansible
```

# Releases

应根据您的需求使用以下branches。stable-* branches已经过QE测试，并且在lifecycle内会得到backport修复。master branch应该被认为是实验性的，谨慎使用。

- `stable-3.0` 支持ceph jewel和luminous版本。该branch需要Ansible 2.4版本。
- `stable-3.1` 支持ceph luminous和mimic版本。该branch需要Ansible 2.4版本。
- `stable-3.2`支持ceph luminous和mimic版本。该branch需要Ansible 2.6版本。
- `stable-4.0`支持ceph nautilus版本。该branch需要Ansible 2.8版本。
- `master` 支持Ceph@master版本。该branch需要Ansible 2.8版本。

# Configuration and Usage

该项目假设您具有ansible的基本知识，并已经准备好主机以供Ansible配置。在clone了ceph-ansible repository后，选择你的branch并且安装了ansible，然后你需要为你的ceph集群创建你的inventory文件，playbook和configuration。

## Inventory

ansible inventory文件定义集群中的主机以及每个主机在ceph集群中扮演的角色（role）。inventory文件的默认位置是`/etc/ansible/hosts`，但此文件可以放在任何位置，并与`ansible-playbook`的`-i` flag一起使用。

示例inventory文件如下所示：

```
[mons]
mon1
mon2
mon3

[osds]
osd1
osd2
osd3
```

```
注意
有关ansible inventories的更多信息，请参阅ansible文档：
http://docs.ansible.com/ansible/latest/intro_inventory.html
```

## Playbook

在部署群集时，必须有一个playbook传递给`ansible-playbook`命令。在`ceph-ansible`项目的根目录中有一个名为`site.yml.sample`的示例playbook。这个playbook应该适用于大多数用法，但它默认包含每个守护进程组（every daemon group），这可能不适合你的群集设置。执行以下步骤准备您的playbook：

- 重命名示例playbook：`mv site.yml.sample site.yml`
- 根据群集需求修改playbook

```
注意
重要的是你使用的playbook应放在ceph-ansible项目的root目录。这就是ansible能够找到ceph-ansible提供的roles的方式。
```

## Configuration Validation

ceph-ansible项目通过ceph-validate 角色（role）提供配置验证（config validation）。如果您使用的是所提供的playbooks，此role将在部署前运行，以确保您的ceph-ansible配置正确。此检查仅确保您为群集提供了正确的配置，而不是生成healthy集群。例如，如果为monitor_address提供了错误的地址，则mon仍然无法加入群集。

验证失败的示例可能如下所示：

```
TASK [ceph-validate : validate provided configuration] *************************
task path: /Users/andrewschoen/dev/ceph-ansible/roles/ceph-validate/tasks/main.yml:3
Wednesday 02 May 2018  13:48:16 -0500 (0:00:06.984)       0:00:18.803 *********
 [ERROR]: [mon0] Validation failed for variable: osd_objectstore

 [ERROR]: [mon0] Given value for osd_objectstore: foo

 [ERROR]: [mon0] Reason: osd_objectstore must be either 'bluestore' or 'filestore'

 fatal: [mon0]: FAILED! => {
     "changed": false
     }
```

### Supported Validation

ceph-validate角色（role）当前支持验证以下osd场景的配置：

- collocated（journal与data一块盘）

- non-collocated
- lvm

ceph-validate角色（role）还验证了以下安装选项：

- ceph_origin 设置为 distro
- ceph_origin 设置为 repository
- ceph_origin 设置为 local
- ceph_repository 设置为 rhcs
- ceph_repository 设置为 dev
- ceph_repository 设置为 community

## Installation methods

Ceph可以通过几种方法安装。

- [Installation methods](http://docs.ceph.com/ceph-ansible/master/installation/methods.html)

## Configuration

ceph集群的配置将通过ceph-ansible提供的ansible变量来设置。所有这些选项及其默认值都在ceph-ansible项目root目录下的group_vars/子目录中定义。Ansible将在group_vars/子目录中使用您的inventory文件或playbook配置。在group_vars/子目录中有许多ansible配置文件示例，它们通过文件名与每个ceph守护进程组（ceph daemon groups）关联。例如，osds.yml.sample包含OSD守护进程的所有默认配置。all.yml.sample文件是一个特殊的group_vars文件，适用于群集中的所有主机。

```
注意
有关设置group或主机特定配置的更多信息，请参阅ansible文档：http://docs.ansible.com/ansible/latest/intro_inventory.html#splitting-out-host-and-group-specific-data
```

最基本的，您必须告诉`ceph-ansible`要安装的ceph版本，安装方法，群集网络设置以及希望如何配置OSD。开始配置，请重命名`group_vars/`目录中的每个文件，让它不包含文件名末尾的`.sample`，取消要更改选项的注释并提供自己的值。

使用lvm批处理方法部署Ceph upstream的octopus版本，在group_vars/all.yml中如下配置：

```
ceph_origin: repository
ceph_repository: community
ceph_stable_release: octopus
public_network: "192.168.3.0/24"
cluster_network: "192.168.4.0/24"
monitor_interface: eth1
devices:
  - '/dev/sda'
  - '/dev/sdb'
```

所有安装都需要更改以下配置选项，但可能还有其他必需选项，具体取决于OSD方案选择或群集的其他方面。

- `ceph_origin`
- `ceph_stable_release`
- `public_network`
- `monitor_interface` or `monitor_address`

部署RGW实例时，需要设置``radosgw_interface` 或`radosgw_address`选项。

## ceph.conf Configuration File

覆盖已定义 ceph.conf 中内容的方法是使用 ceph_conf_overrides 变量。这允许您使用 INI 格式指定配置选项。此变量可用于覆盖已在 ceph.conf 中定义的sections（请参见：roles/ceph-config/templates/ceph.conf.j2）或提供新的配置选项。

支持ceph.conf中的以下部分：

- [global]
- [mon]
- [osd]
- [mds]
- [client.rgw.{instance_name}]

例如：

```
ceph_conf_overrides:
   global:
     foo: 1234
     bar: 5678
   osd:
     osd_mkfs_type: ext4
```

```
注意
我们将不再接受修改ceph.conf模板的pull requests，除非它有助于部署。对于简单的配置调整，请使用ceph_conf_overrides变量。
```

用于配置每个ceph daemon types的完整文档，在以下各sections中。

## OSD Configuration

通过选择OSD场景并提供该场景所需的配置来设置OSD配置。从稳定4.0版的nautilus开始，唯一可用的场景是lvm。

- [OSD Scenarios](http://docs.ceph.com/ceph-ansible/master/osds/scenarios.html)

从`stable-4.0`开始，不再支持下列场景，因为他们与`ceph-disk`相关联：

- collocated
- non-collocated

自Ceph luminous发布以来，首选使用`ceph-volume`工具的lvm场景。任何其他方案都将导致弃用警告。

`ceph-disk`在ceph-ansible 3.2中被弃用，并且已经在Nautilus版本完全从Ceph本身中删除。 目前（从稳定版4.0开始），只有一个场景，默认为`lvm`，参见：

- lvm

因此不再需要配置`osd_scenario`，它默认为`lvm`。

上面提到的`lvm`场景支持containerized和non-containerized集群。提醒一下，可以通过将`containerized_deployment`设置为`True`来完成部署容器化集群。

### lvm

此场景使用`ceph-volume`来创建LVM的OSD，并且仅在Ceph luminous或更新的版本才可用。它会自动启用。

其他（可选）支持的设置：

- `osd_objectstore`：为OSD设置Ceph objectstore。可用选项是`filestore`或`bluestore`。你只能选择luminous或更高Ceph版本的`bluestore`。如果未设置，则默认为`bluestore`。

- `dmcrypt`：使用dmcrypt在OSD上启用Ceph加密。如果未设置，则默认为false。
- `osds_per_device`：为每个设备提供多个OSD（如果未设置，默认为1，也就是说1个设备1个OSD）。

### Simple configuration

使用这种方法，有关设备如何配置为预配 OSD 的决策都由 Ceph 工具完成的（本例中为ceph-volume lvm批处理）。在给定输入设备的情况下，几乎无法修改OSD的组合方式。

要使用此配置，必须配置使用OSD的原始设备路径填充devices选项。

```
注意
原始设备必须“干净”，没有gpt分区表或逻辑卷存在。
```

例如，对于具有`/dev/sda`和`/dev/sdb`的节点，配置将是：

```
devices:
  - /dev/sda
  - /dev/sdb
```

在上述情况下，如果两个设备都是spinning（旋转）设备（HDD），则会创建2个OSD，每个OSD都有自己的collocated journal（journal与data同盘）。

其他方式可能是spinning设备与solid state设备混合部署（HDD+SSD），例如：

```
devices:
  - /dev/sda
  - /dev/sdb
  - /dev/nvme0n1
```

与最初的示例类似，最终会生成2个OSD，但数据将放在较慢的spinning设备（`/dev/sda`, 和`/dev/sdb`）上，而journals将放在速度更快的solid state设备`/dev/nvme0n1`上。 `ceph-volume`工具在[“batch”子命令部分](http://docs.ceph.com/docs/master/ceph-volume/lvm/batch/)详细描述了这一点

此选项也可以与`osd_auto_discovery`一起使用，这意味着您不需要直接填充`devices` ，而是使用ansible找到的任何适当的设备。

```
osd_auto_discovery: true
```

其他（可选）支持的设置：

- `crush_device_class`：为使用此方法创建的所有OSD设置CRUSH device class（使用simple configuration方法不可能每个OSD都有CRUSH device class）。 值必须是字符串，如`crush_device_class："ssd"`

```
未使用crush_device_class
# begin crush map
tunable choose_local_tries 0
tunable choose_local_fallback_tries 0
tunable choose_total_tries 50
tunable chooseleaf_descend_once 1
tunable chooseleaf_vary_r 1
tunable chooseleaf_stable 1
tunable straw_calc_version 1
tunable allowed_bucket_algs 54

# devices
device 0 osd.0 class hdd
device 1 osd.1 class hdd
device 2 osd.2 class hdd
device 3 osd.3 class hdd
device 4 osd.4 class hdd
device 5 osd.5 class hdd
device 6 osd.6 class hdd
device 7 osd.7 class hdd
device 8 osd.8 class hdd

# types
type 0 osd
type 1 host
type 2 chassis
type 3 rack
type 4 row
type 5 pdu
type 6 pod
type 7 room
type 8 datacenter
type 9 zone
type 10 region
type 11 root

# buckets
host community-ceph13-1 {
	id -3		# do not change unnecessarily
	id -4 class hdd		# do not change unnecessarily
	# weight 0.144
	alg straw2
	hash 0	# rjenkins1
	item osd.0 weight 0.048
	item osd.3 weight 0.048
	item osd.6 weight 0.048
}
host community-ceph13-2 {
	id -5		# do not change unnecessarily
	id -6 class hdd		# do not change unnecessarily
	# weight 0.144
	alg straw2
	hash 0	# rjenkins1
	item osd.1 weight 0.048
	item osd.4 weight 0.048
	item osd.8 weight 0.048
}
host community-ceph13-3 {
	id -7		# do not change unnecessarily
	id -8 class hdd		# do not change unnecessarily
	# weight 0.144
	alg straw2
	hash 0	# rjenkins1
	item osd.2 weight 0.048
	item osd.5 weight 0.048
	item osd.7 weight 0.048
}
root default {
	id -1		# do not change unnecessarily
	id -2 class hdd		# do not change unnecessarily
	# weight 0.431
	alg straw2
	hash 0	# rjenkins1
	item community-ceph13-1 weight 0.144
	item community-ceph13-2 weight 0.144
	item community-ceph13-3 weight 0.144
}

# rules
rule replicated_rule {
	id 0
	type replicated
	min_size 1
	max_size 10
	step take default
	step chooseleaf firstn 0 type host
	step emit
}

# end crush map
```



### Advanced configuration

当在设置devices需要更精细的控制OSD时，非常有用。它需要已存在的volume groups和logical volumes设置（`ceph-volume`不会创建这些卷）。

要使用此配置，必须使用logical volumes和volume groups填充`lvm_volumes`选项。此外，分区的绝对路径可用于`journal`，`block.db`和`block.wal`。

```
注意
此配置使用ceph-volume lvm create来设置OSD
```

支持的`lvm_volumes`配置设置：

- `data`：logical volume名称或raw device（原始设备）的绝对路径（使用 100% 的原始设备创建 LV）

- `data_vg`：volume group名称，如果`data`是logical volume，则为必需。
- `crush_device_class`：生成OSD的CRUSH device class名，允许为每个OSD设置device class，而不像全局`crush_device_class`那样为所有OSD设置它们。

```
注意
如果在使用devices时为OSD设置crush_device_class，则必须使用全局crush_device_class选项设置它，如上所示。 当使用像lvm_volumes这样的devices时，无法为每个OSD定义特定的CRUSH device class。
```

`filestore` objectstore变量：

- `journal`：logical volume名称或分区的绝对路径。
- `journal_vg`：volume group名称，如果journal是logical volume，则是必选。

```
警告
每个entry（条目）必须是唯一的，不允许重复的值
```

`bluestore`  objectstore变量：

- `db`： logical volume名称或分区的绝对路径。
- `db_vg`： volume group名称，如果`db`是logical volume，则是必选。
- `wal`： logical volume名称或分区的绝对路径。
- `wal_vg`： volume group名称，如果`wal`是logical volume，则是必选。

```
注意
这些bluestore变量是可选的优化选项。 Bluestore的db和wal只会从更快的设备中受益。可以使用单个raw device（原始设备）创建bluestore OSD。
```

```
警告
每个entry（条目）必须是唯一的，不允许重复的值
```

使用 raw devices（原始设备）的`bluestore`示例：

```
osd_objectstore: bluestore
lvm_volumes:
  - data: /dev/sda
  - data: /dev/sdb
```

```
注意
在这种情况下，将使用100％的devices创建volume groups和logical volumes。
```

具有logical volumes的`bluestore`示例：

```
osd_objectstore: bluestore
lvm_volumes:
  - data: data-lv1
    data_vg: data-vg1
  - data: data-lv2
    data_vg: data-vg2
```

```
注意
必须存在Volume groups和logical volumes。
```

定义`wal`和`db`logical volumes的bluestore示例：

```
osd_objectstore: bluestore
lvm_volumes:
  - data: data-lv1
    data_vg: data-vg1
    db: db-lv1
    db_vg: db-vg1
    wal: wal-lv1
    wal_vg: wal-vg1
  - data: data-lv2
    data_vg: data-vg2
    db: db-lv2
    db_vg: db-vg2
    wal: wal-lv2
    wal_vg: wal-vg2
```

```
注意
必须存在volume groups和logical volumes。
```

具有logical volumes的`filestore`示例：

```
osd_objectstore: filestore
lvm_volumes:
  - data: data-lv1
    data_vg: data-vg1
    journal: journal-lv1
    journal_vg: journal-vg1
  - data: data-lv2
    data_vg: data-vg2
    journal: journal-lv2
    journal_vg: journal-vg2
```

```
注意
必须存在volume groups和logical volumes。
```

# Contribution

有关如何为ceph-ansible做出贡献的指导，请参阅以下部分。

- [Contribution Guidelines](http://docs.ceph.com/ceph-ansible/master/dev/index.html)

# Testing

用于编写 ceph-ansible 功能测试方案的文档。

- [Testing with ceph-ansible](http://docs.ceph.com/ceph-ansible/master/testing/index.html)

- [Glossary](http://docs.ceph.com/ceph-ansible/master/testing/glossary.html)

# Demos

## Vagrant Demo

在裸机上从头开始部署：<https://youtu.be/E8-96NamLDo>

## Bare metal demo

在裸机上从头开始部署：<https://youtu.be/dv_PEp9qAqg>



# ansible不配置ssh免密钥,使用密码登录

```
1、编辑/etc/ansible/ansible.cfg
取消注释：#host_key_checking = False

2、编辑/etc/ansible/hosts
[ceph_exporter]
10.121.136.110	ansible_user=root	ansible_ssh_pass=root
10.121.136.111	ansible_user=root       ansible_ssh_pass=root
10.121.136.112	ansible_user=root       ansible_ssh_pass=root
10.121.136.113	ansible_user=root       ansible_ssh_pass=root
10.121.136.114	ansible_user=root       ansible_ssh_pass=root
10.121.136.115	ansible_user=root       ansible_ssh_pass=root

[node_exporter]
10.121.136.110	ansible_user=root       ansible_ssh_pass=root
10.121.136.111	ansible_user=root       ansible_ssh_pass=root
10.121.136.112	ansible_user=root       ansible_ssh_pass=root
10.121.136.113	ansible_user=root       ansible_ssh_pass=root
10.121.136.114	ansible_user=root       ansible_ssh_pass=root
10.121.136.115	ansible_user=root       ansible_ssh_pass=root

3、测试
ansible all -m ping
```

























