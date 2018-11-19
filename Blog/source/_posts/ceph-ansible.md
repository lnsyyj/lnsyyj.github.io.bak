---
title: ceph-ansible
date: 2018-07-27 15:31:57
tags: Ansible
---

原文地址：http://docs.ceph.com/ceph-ansible/master/

# ceph-ansible

Ansible playbooks for Ceph, the distributed filesystem.

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

（CentOS不使用subscription-manager，默认情况下已启用“Extras” 。

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

应根据您的需求使用以下branches。stable-* branches已经过QE测试，并且在整个生命周期内会得到backport修复。master branch应该被认为是实验性的并且谨慎使用。

- stable-3.0 支持ceph的jewel和luminous版。该branch支持ansible的2.4和2.5版本。
- stable-3.1 支持ceph的luminous和mimic版。该branch支持ansible的2.4和2.5版本。
- master 支持ceph的luminous和mimic版。该branch支持ansible的2.5版本。

# Configuration and Usage

该项目假设您具有ansible的基本知识，并已通过ansible准备好主机以进行配置。在clone了ceph-ansible repository后，选择你的branch并且安装了ansible，然后你需要为你的ceph集群创建你的inventory文件，playbook和configuration。

## Inventory

ansible inventory文件定义集群中的主机以及每个主机在您的ceph集群中扮演的roles。inventory文件的默认位置是/etc/ansible/hosts，但此文件可以放在任何位置，并与ansible-playbook的-i flag一起使用。示例inventory文件如下所示：

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

在部署群集时，必须有一个playbook传递给ansible-playbook命令。在ceph-ansible项目的根目录中有一个名为site.yml.sample的示例playbook。这个playbook应该适用于大多数用法，但它默认包含每个守护程序组（every daemon group），这可能不适合您的群集设置。执行以下步骤准备您的playbook：

- 重命名示例playbook：mv site.yml.sample site.yml
- 根据群集要求修改playbook

```
注意
重要的是你使用的playbook放在ceph-ansible项目的root。这就是ansible能够找到ceph-ansible提供的roles的方式。
```

## Configuration Validation

ceph-ansible项目通过ceph-validate role提供配置验证。如果您使用的是所提供的playbooks，此role将在部署前运行，以确保您的ceph-ansible配置正确。此检查仅确保您为群集提供了正确的配置，而不是生成healthy集群。例如，如果为monitor_address提供了错误的地址，则mon仍然无法加入群集。

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

## ceph-ansible - choose installation method

Ceph可以通过几种方法安装。

- [Installation methods](http://docs.ceph.com/ceph-ansible/master/installation/methods.html)

## ceph-ansible Configuration

ceph集群的配置将通过ceph-ansible提供的ansible变量来设置。所有这些选项及其默认值都在ceph-ansible项目的root下的group_vars/目录中定义。Ansible将在group_vars/目录中使用您的inventory文件或playbook配置。在group_vars/目录中有许多ansible配置文件示例，它们通过文件名与每个ceph守护程序组（ceph daemon groups）关联。例如，osds.yml.sample包含OSD守护进程的所有默认配置。all.yml.sample文件是一个特殊的group_vars文件，适用于群集中的所有主机。

```
注意
有关设置group或主机特定配置的更多信息，请参阅ansible文档：http://docs.ansible.com/ansible/latest/intro_inventory.html#splitting-out-host-and-group-specific-data
```

最基本的，您必须告诉ceph-ansible要安装的ceph版本，安装方法，群集网络设置以及希望如何配置OSD。开始配置，请重命名group_vars/中的每个文件，让它不包含文件名末尾的.sample，取消要更改选项的注释并提供自己的值。

将ceph的upstream jewel版本并且配置collocated journals的OSDs一起部署的示例配置在group_vars/all.yml中看起来是这样的：

```
ceph_origin: repository
ceph_repository: community
ceph_stable_release: jewel
public_network: "192.168.3.0/24"
cluster_network: "192.168.4.0/24"
monitor_interface: eth1
devices:
  - '/dev/sda'
  - '/dev/sdb'
osd_scenario: collocated
```

所有安装都需要更改以下配置选项，但可能还有其他必需选项，具体取决于OSD方案选择或群集的其他方面。

- `ceph_origin`
- `ceph_stable_release`
- `public_network`
- `osd_scenario`
- `monitor_interface` or `monitor_address`
- `radosgw_interface` or `radosgw_address`

## ceph.conf Configuration

覆盖已定义 ceph.conf 中内容的方法是使用 ceph_conf_overrides 变量。这允许您使用 INI 格式指定配置选项。此变量可用于覆盖已在 ceph 中定义的sections（请参见：roles/ceph-config/templates/ceph.conf.j2）或提供新的配置选项。支持ceph.conf中的以下部分：[global]，[mon]，[osd]，[mds]和[rgw]。

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

用于配置每个ceph daemon types的完整文档在以下各sections中。

## OSD Configuration

osd 配置是通过选择 osd 方案并提供该方案所需的配置来设置的。每个场景的要求都不同。通过设置osd_scenario配置选项来选择OSD方案。

- [OSD Scenarios](http://docs.ceph.com/ceph-ansible/master/osds/scenarios.html)

# Contribution

有关如何为ceph-ansible做出贡献的指导，请参阅以下部分。

- [Contribution Guidelines](http://docs.ceph.com/ceph-ansible/master/dev/index.html)

# Testing

用于编写 ceph ansible 功能测试方案的文档。

- [Testing with ceph-ansible](http://docs.ceph.com/ceph-ansible/master/testing/index.html)

- [Glossary](http://docs.ceph.com/ceph-ansible/master/testing/glossary.html)

# Demos

## Vagrant Demo

在裸机上从头开始部署：<https://youtu.be/E8-96NamLDo>

## Bare metal demo

在裸机上从头开始部署：<https://youtu.be/dv_PEp9qAqg>



























