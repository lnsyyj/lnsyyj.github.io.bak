---
title: ceph osd blacklist
date: 2019-11-13 09:03:48
tags: ceph
---

# 什么是OSD blacklist，如何处理？

### 环境

Red Hat Ceph Storage

### 问题

我正在运行`ceph osd dump`命令，它确实列出了blacklist items：

```
# ceph osd dump
[...]
blacklist 10.37.192.139:0/1308721908 expires 2019-02-27 10:10:52.049084
```

这是什么意思，我该如何解决？

### 决议

尽管有一些控制命令可删除blacklist entries（例如`ceph osd blacklist rm ADDRESS[:source_port]`），但blacklists通常会自动维护，无需手动干预。因此，您无需采取任何措施。如有疑问，请联系Red Hat支持。

### 根本原因

blacklist最常用于CephFS场景中，以防止滞后的元数据服务器对OSD上的数据进行不良更改。

### 诊断步骤

  ```
# ceph osd blacklist ls
listed 1 entries
10.37.192.139:0/1308721908 2019-02-27 10:10:52.049084
  ```

该解决方案是Red Hat快速发布计划的一部分，提供了Red Hat工程师在为客户提供支持时创建的庞大解决方案库。 为了使您立即获得所需的知识，这些文章可能以未经编辑的原始形式出现。



# CEPH FILESYSTEM CLIENT EVICTION（CEPH文件系统客户端驱逐）

当文件系统客户端无响应或行为异常时，可能有必要强制终止其对文件系统的访问。 此过程称为eviction（驱逐）。

驱逐CephFS客户端会阻止其与MDS daemons和OSD daemons进一步通信。 如果客户端正在对文件系统进行buffered IO，则所有未刷新的数据都将丢失。

客户端可以自动退出（如果无法及时与MDS通信），也可以手动退出（由系统管理员）。

客户端驱逐过程适用于各种客户端，包括FUSE mounts，kernel mounts，nfs-ganesha gateways以及任何使用libcephfs的进程。

### AUTOMATIC CLIENT EVICTION（自动客户端逐出）

在三种情况下，可能会自动将客户驱逐：

- 在active MDS daemon上，如果客户端在session_autoclose（文件系统变量）秒（默认为300秒）以上未与MDS通信，则它将自动被驱逐。

- 在active MDS daemon上，如果客户端在mds_cap_revoke_eviction_timeout（配置选项）秒内未响应cap revoke messages。 默认情况下禁用。

- 在MDS启动期间（包括故障转移时），MDS称为reconnect的状态。 在此状态期间，它将等待所有客户端连接到新的MDS daemon。 如果客户端未在时间窗口内这样做（mds_reconnect_timeout，默认为45秒），则将其驱逐。

如果出现以上任何一种情况，warning message将发送到cluster log。

### MANUAL CLIENT EVICTION（手动客户端驱逐）

有时，管理员可能希望手动驱逐客户端。 如果客户端死亡，并且管理员不想等待其session超时；或者，如果客户端行为异常并且管理员无权访问客户端节点来卸载它。

首先检查客户列表：

```
ceph tell mds.0 client ls

[
    {
        "id": 4305,
        "num_leases": 0,
        "num_caps": 3,
        "state": "open",
        "replay_requests": 0,
        "completed_requests": 0,
        "reconnecting": false,
        "inst": "client.4305 172.21.9.34:0/422650892",
        "client_metadata": {
            "ceph_sha1": "ae81e49d369875ac8b569ff3e3c456a31b8f3af5",
            "ceph_version": "ceph version 12.0.0-1934-gae81e49 (ae81e49d369875ac8b569ff3e3c456a31b8f3af5)",
            "entity_id": "0",
            "hostname": "senta04",
            "mount_point": "/tmp/tmpcMpF1b/mnt.0",
            "pid": "29377",
            "root": "/"
        }
    }
]
```

一旦识别出要逐出的客户机，就可以使用其唯一ID或各种其他属性来识别它：

```
# These all work
ceph tell mds.0 client evict id=4305
ceph tell mds.0 client evict client_metadata.=4305
```

### ADVANCED: UN-BLACKLISTING A CLIENT（进阶：取消blacklist客户）

通常，列入blacklist的客户端可能无法重新连接到服务器：必须先将其unmount，然后再重新mount。

但是，在某些情况下，允许被驱逐的客户端尝试重新连接可能会很有用。

由于CephFS使用RADOS OSD blacklist控制客户端驱逐，因此可以通过从blacklist中删除CephFS客户端来重新连接它们：

```
$ ceph osd blacklist ls
listed 1 entries
127.0.0.1:0/3710147553 2018-03-19 11:32:24.716146

$ ceph osd blacklist rm 127.0.0.1:0/3710147553
un-blacklisting 127.0.0.1:0/3710147553
```

如果其他客户端访问了列入blacklist的客户端正在buffered IO的文件，则这样做可能会使数据完整性受到威胁。 也不能保证产生一个功能完备的客户端 — 在驱逐后恢复完全健康的客户端的最佳方法是unmount客户端并重新mount。

如果您尝试以这种方式重新连接客户端，则在FUSE客户端中将client_reconnect_stale设置为true，以提示客户端尝试重新连接。

### ADVANCED: CONFIGURING BLACKLISTING（进阶：配置blacklist）

如果由于客户端主机速度慢或网络不可靠而频繁驱逐客户端，并且您无法解决根本问题，那么您可能希望要求MDS的严格性降低。

可以通过放弃其MDS sessions来响应慢速客户端，但允许他们重新打开sessions并允许他们继续与OSD对话。 要启用此模式，请在MDS节点上将mds_session_blacklist_on_timeout设置为false。

对于手动驱逐的等效行为，请将mds_session_blacklist_on_evict设置为false。

请注意，如果禁用了blacklist，则驱逐客户端只会对您发送命令的MDS产生影响。 在具有multiple active MDS daemons的系统上，您需要向每个active daemon发送驱逐命令。 启用blacklist（默认设置）后，仅将驱逐命令发送到单个MDS就足够了，因为blacklist会将其传播到其他MDS。

### BACKGROUND: BLACKLISTING AND OSD EPOCH BARRIER（背景：blacklist和OSD epoch barrier）

在将客户端列入blacklist之后，有必要确保其他客户端和MDS daemons在尝试访问被列入blacklist的客户端可能已访问的任何数据对象之前，具有最新的OSDMap（包括blacklist entry）。

使用内部的"osdmap epoch barrier"机制可以确保这一点。

barrier的目的是确保当我们分发任何允许touching相同 RADOS objects的功能时，分发的客户端必须具有最新的 OSD map，不与已cancel的操作（来自 ENOSPC）或blacklist客户端（逐出）进行竞争。

更具体地说，设置epoch barrier的情况是：

- Client eviction — 客户端驱逐（客户端被列入blacklist，其他客户端必须等待post-blacklist epoch后才能touch相同的objects）。
- 客户端中的OSD map full flag handling（客户端可以从pre-full epoch取消某些OSD操作，因此其他客户端必须等到full epoch或更晚才能touching相同的objects）。
- MDS启动，因为我们不持续维护barrier epoch，因此，必须假定重新启动后始终需要最新的OSD map。

请注意，这是简单的global value。 我们可以在每个inode的基础上进行维护。 但是我们没有，因为：

- 它将更加复杂。
- 每个inode将使用额外的4个字节的内存。
- 因为几乎每个人都拥有最新的OSD map，所以效率不会更高。 而且，在大多数情况下，每个人都会轻而易举地克服这一barrier，而不是waiting。
- 在极少数情况下遇到barrier，因此很少会看到每个inode粒度带来好处。

epoch barrier与所有capability messages一起发送，并指示message的接收者避免在看到OSD epoch之前向OSD发送更多的RADOS操作。 这主要适用于客户端（将其数据直接写到文件中），但也适用于MDS，因为诸如文件大小probing和文件删除之类的操作是直接从MDS完成的。



# blacklist相关命令

### 1、从blacklist中添加（可选项，直到` <expire> `秒后）或删除`<addr>`，默认3600秒

```
osd blacklist add|rm <EntityAddr> {<float[0.0-]>}                   add (optionally until <expire> seconds from now) or remove <addr> from blacklist
```

实验1，添加删除blacklist测试

```
[root@ceph3 ~]# ceph osd blacklist add 10.20.10.28
blacklisting 10.20.10.28:0/0 until 2019-11-13 12:55:53.700776 (3600 sec)
[root@ceph3 ~]# ceph osd blacklist add 10.20.10.13 6000
blacklisting 10.20.10.13:0/0 until 2019-11-13 13:36:16.575894 (6000 sec)

[root@ceph3 ~]# ceph osd blacklist ls
listed 2 entries
10.20.10.13:0/0 2019-11-13 13:36:16.575894
10.20.10.28:0/0 2019-11-13 12:55:53.700776


[root@ceph3 ~]# ceph osd blacklist ls
listed 1 entries
10.20.10.28:0/0 2019-11-13 10:23:00.029669
```

实验2，当client在blacklist中时，在client端尝试mount cephfs（ceph-client  10.20.10.2）

```
1、将ceph-client加入blacklist
[root@ceph1 ~]# ceph osd blacklist add 10.20.10.2
[root@ceph1 ~]# ceph osd blacklist ls
listed 1 entries
10.20.10.2:0/0 2019-11-13 15:42:54.260358

2、ceph-client尝试mount cephfs
[root@ceph-client ~]# ceph-fuse /root/ceph-fuse/ --verbose
ceph-fuse[1664]: starting ceph client
2019-11-13 14:45:18.902688 7f8f3b0db0c0 -1 init, newargv = 0x55c15933c000 newargc=10
ceph-fuse[1664]: ceph mount failed with (1) Operation not permitted

3、将ceph-client从blacklist中删除
[root@ceph1 ~]# ceph osd blacklist rm 10.20.10.2
un-blacklisting 10.20.10.2:0/0

[root@ceph-client ~]# ceph-fuse /root/ceph-fuse/ 
ceph-fuse[1704]: starting ceph client
2019-11-13 14:49:16.400939 7f9326c7c0c0 -1 init, newargv = 0x557665de4ea0 newargc=9
ceph-fuse[1704]: starting fuse

[root@ceph-client ~]# df -Th
ceph-fuse      fuse.ceph-fuse   93G     0   93G   0% /root/ceph-fuse
```

### 2、清除所有列入blacklist的客户端

```
osd blacklist clear                   clear all blacklisted clients
```

实验1

```
[root@ceph3 ~]# ceph osd blacklist add 10.20.10.28
blacklisting 10.20.10.28:0/0 until 2019-11-13 12:53:48.463948 (3600 sec)
[root@ceph3 ~]# ceph osd blacklist add 10.20.10.13
blacklisting 10.20.10.13:0/0 until 2019-11-13 12:53:56.846733 (3600 sec)

[root@ceph3 ~]# ceph osd blacklist ls
listed 2 entries
10.20.10.13:0/0 2019-11-13 12:53:56.846733
10.20.10.28:0/0 2019-11-13 12:53:48.463948

[root@ceph3 ~]# ceph osd blacklist clear
 removed all blacklist entries

[root@ceph3 ~]# ceph osd blacklist ls
listed 0 entries
```

### 3、显示列入blacklist的客户端

```
osd blacklist ls --format json                   show blacklisted clients
```

实验1

```
[root@ceph1 ~]# ceph osd blacklist ls --format json
listed 1 entries

[{"addr":"10.20.10.2:0/0","until":"2019-11-13 17:10:56.217959"}]

/0表示：AsyncMessenger stuff approximately unique ID set by the Constructor for use in entity_addr_t
```

### 4、如果客户端在`session_autoclose <value>`秒（默认为300秒）以上未与MDS通信，则它将自动被驱逐。

```
fs set <fs_name> max_mds|max_file_size|allow_new_snaps|inline_data|cluster_down|allow_multimds|allow_dirfrags| balancer|standby_count_wanted|session_timeout|session_autoclose <val> {<confirm>}			set fs parameter <var> to <val>
```

实验1

```
[root@ceph1 ~]# ceph fs set cephfs session_autoclose 400
```

### 5、获取有关一个文件系统的信息

```
ceph fs get <fs_name> --format json
```

实验1

```
[root@ceph1 ~]# ceph fs get cephfs --format json
{
	"mdsmap": {
		"epoch": 19,
		"flags": 12,
		"ever_allowed_features": 0,
		"explicitly_allowed_features": 0,
		"created": "2019-11-11 11:16:05.316461",
		"modified": "2019-11-13 15:59:17.551876",
		"tableserver": 0,
		"root": 0,
		"session_timeout": 60,
		"session_autoclose": 400,
		"max_file_size": 1099511627776,
		"last_failure": 0,
		"last_failure_osd_epoch": 104,
		"compat": {
			"compat": {},
			"ro_compat": {},
			"incompat": {
				"feature_1": "base v0.20",
				"feature_2": "client writeable ranges",
				"feature_3": "default file layouts on dirs",
				"feature_4": "dir inode in separate object",
				"feature_5": "mds uses versioned encoding",
				"feature_6": "dirfrag is stored in omap",
				"feature_8": "no anchor table",
				"feature_9": "file layout v2"
			}
		},
		"max_mds": 1,
		"in": [0],
		"up": {
			"mds_0": 4335
		},
		"failed": [],
		"damaged": [],
		"stopped": [],
		"info": {
			"gid_4335": {
				"gid": 4335,
				"name": "ceph2",
				"rank": 0,
				"incarnation": 14,
				"state": "up:active",
				"state_seq": 41535,
				"addr": "10.20.10.13:6804/622620898",
				"standby_for_rank": 0,
				"standby_for_fscid": -1,
				"standby_for_name": "",
				"standby_replay": true,
				"export_targets": [],
				"features": 4611087853746454523
			},
			"gid_4456": {
				"gid": 4456,
				"name": "ceph3",
				"rank": 0,
				"incarnation": 0,
				"state": "up:standby-replay",
				"state_seq": 2,
				"addr": "10.20.10.25:6805/1639008809",
				"standby_for_rank": 0,
				"standby_for_fscid": -1,
				"standby_for_name": "",
				"standby_replay": true,
				"export_targets": [],
				"features": 4611087853746454523
			}
		},
		"data_pools": [6],
		"metadata_pool": 7,
		"enabled": true,
		"fs_name": "cephfs",
		"balancer": "",
		"standby_count_wanted": 1
	},
	"id": 1
}
```



# blacklist相关配置



```
MON相关配置
	客户端blacklist entries保留在OSD map中的持续时间（以秒为单位）
    Option("mon_osd_blacklist_default_expire", Option::TYPE_FLOAT, Option::LEVEL_ADVANCED)
    .set_default(1_hr)
    .add_service("mon")
    .set_description("Duration in seconds that blacklist entries for clients remain in the OSD map"),

	MDS daemons的blacklist entries保留在OSD map中的持续时间（以秒为单位）
    Option("mon_mds_blacklist_interval", Option::TYPE_FLOAT, Option::LEVEL_DEV)
    .set_default(1_day)
    .set_min(1_hr)
    .add_service("mon")
    .set_description("Duration in seconds that blacklist entries for MDS daemons remain in the OSD map"),

RBD相关配置
	是否将损坏锁的客户端列入blacklist
    Option("rbd_blacklist_on_break_lock", Option::TYPE_BOOL, Option::LEVEL_ADVANCED)
    .set_default(true)
    .set_description("whether to blacklist clients whose lock was broken"),

	blacklist的秒数 - OSD 默认值为 0
    Option("rbd_blacklist_expire_seconds", Option::TYPE_UINT, Option::LEVEL_ADVANCED)
    .set_default(0)
    .set_description("number of seconds to blacklist - set to 0 for OSD default"),

MDS相关配置
	是否将sessions已过期的客户端列入blacklist
    Option("mds_session_blacklist_on_timeout", Option::TYPE_BOOL, Option::LEVEL_ADVANCED)
    .set_default(true)
    .set_description("blacklist clients whose sessions have become stale"),

	是否将被逐出的客户端列入blacklist
    Option("mds_session_blacklist_on_evict", Option::TYPE_BOOL, Option::LEVEL_ADVANCED)
    .set_default(true)
    .set_description("blacklist clients that have been evicted"),

	数秒后，没有响应MDS的“cap revoke messages”的客户端将被驱逐。（默认为0，表示关闭该功能）
    Option("mds_cap_revoke_eviction_timeout", Option::TYPE_FLOAT, Option::LEVEL_ADVANCED)
     .set_default(0)
     .set_description("number of seconds after which clients which have not responded to cap revoke messages by the MDS are evicted."),

	MDS重新连接恢复状态期间等待客户端重新连接的超时时间（以秒为单位）
    Option("mds_reconnect_timeout", Option::TYPE_FLOAT, Option::LEVEL_ADVANCED)
    .set_default(45)
    .set_description("timeout in seconds to wait for clients to reconnect during MDS reconnect recovery state"),
```



# 参考资料

【1】https://access.redhat.com/solutions/3944931

【2】https://docs.ceph.com/docs/mimic/cephfs/eviction/