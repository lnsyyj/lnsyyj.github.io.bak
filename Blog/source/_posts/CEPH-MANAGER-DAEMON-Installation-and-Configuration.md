---
title: CEPH MANAGER DAEMON - Installation and Configuration
date: 2017-11-22 22:41:01
tags: CEPH-MGR
---

CEPH MANAGER DAEMON - Installation and Configuration - CEPH-MGR ADMINISTRATOR’S GUIDE

**MANUAL SETUP**

通常，你可以使用ceph-ansible这样的工具来建立一个ceph-mgr守护进程。 下面这些说明描述了如何手动设置ceph-mgr守护进程。

首先，为守护进程创建一个身份验证密钥：

    ceph auth get-or-create mgr.$name mon 'allow profile mgr' osd 'allow *' mds 'allow *'

该密钥放置到mgr数据路径中，对于集群名“ceph”和mgr $name为“foo”，该路径将是/var/lib/ceph/mgr/ceph-foo。

启动ceph-mgr守护进程：

    ceph-mgr -i $name

通过查看ceph状态的输出来检查mgr是否已经出现，它现在应该包含一个mgr状态行：

    mgr active: $name

**CLIENT AUTHENTICATION**

manager是一个新的守护进程，需要新的CephX功能。 如果您从旧版本的Ceph升级集群，或者使用默认安装/部署工具，则admin client应该自动获得此功能。如果您使用其他地方的工具，则在调用某些ceph集群命令时可能会收到EACCES错误。为了解决这个问题，可以通过修改用户权限来为client的cephx功能添加一个“mgr allow *”。

（http://docs.ceph.com/docs/master/rados/operations/user-management/#modify-user-capabilities）

**HIGH AVAILABILITY**

一般来说，你应该在每台运行ceph-mon守护进程的主机上设置一个ceph-mgr来达到相同的可用性。默认情况下，首先启动的ceph-mgr实例将被monitors设置为active，而其他的则是standbys。ceph-mgr守护进程中不需要仲裁。如果 active daemon在mon_mgr_beacon_grace（default 30s）规定时间内发送beacon到monitors失败，standby会替代当前active。如果要抢先进行故障切换，可以使用命令ceph mgr fail <mgr name>明确标记ceph-mgr daemon失败。

**USING MODULES**

使用命令ceph mgr module ls查看哪些模块可用，哪些是当前启用的。使用命令ceph mgr module enable <module>和ceph mgr module disable <module>分别启用模块和禁用模块。
如果启用了一个模块，则active ceph-mgr daemon将加载并执行它。当模块在加载时发布它的IP地址，如HTTP server。要查看这些模块的地址，请使用命令ceph mgr services。
一些模块也可以实现一个特殊的standby模式，它在standby ceph-mgr daemons以及active daemon。如果client尝试连接到standby，这使得提供服务的模块能够将其client重定向到active daemon。
有关每个模块提供的功能的更多信息，请参阅个别管理器模块的文档页面。

以下是启用dashboard模块的示例：


	$ ceph mgr module ls
	{
	        "enabled_modules": [
	                "restful",
	                "status"
	        ],
	        "disabled_modules": [
	                "dashboard"
	        ]
	}

	$ ceph mgr module enable dashboard
	$ ceph mgr module ls
	{
	        "enabled_modules": [
	                "restful",
	                "status",
	                "dashboard"
	        ],
	        "disabled_modules": [
	        ]
	}

	$ ceph mgr services
	{
	        "dashboard": "http://myserver.com:7789/",
	        "restful": "https://myserver.com:8789/"
	}

**CALLING MODULE COMMANDS**

在模块实现command line hooks的情况下，这些commands将作为普通的Ceph命令来访问：

    ceph <command | help>

如果你想查看manager处理的命令列表（标准ceph help将显示所有的mon和mgr命令），你可以直接发送一个命令到manager daemon：

    ceph tell mgr help

注意，没有必要去访问一个特定的mgr instance，mgr将自动选择当前的active daemon。

**CONFIGURATION**

OPTION(mgr_module_path, OPT_STR, CEPH_PKGLIBDIR “/mgr”) // 从哪里加载python模块

	mgr module path
	Description:    Path to load modules from
	Type:    String
	Default:    "<library dir>/mgr"

	mgr data
	Description:    Path to load daemon data (such as keyring)
	Type:    String
	Default:    "/var/lib/ceph/mgr/$cluster-$id"

	mgr tick period
	Description:    How many seconds between mgr beacons to monitors, and other periodic checks.
	Type:    Integer
	Default:    5

	mon mgr beacon grace
	Description:    How long after last beacon should a mgr be considered failed
	Type:    Integer
	Default:    30
