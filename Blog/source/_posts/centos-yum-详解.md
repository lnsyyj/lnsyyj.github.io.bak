---
title: centos yum 详解
date: 2019-07-10 16:44:54
tags: Linux
---

# NAME

yum - Yellowdog Updater Modified

# SYNOPSIS

yum [options] [command] [package ...]

# DESCRIPTION

 yum是一个基于rpm的交互式package manager。 它可以自动执行系统更新，包括依赖分析和基于`repository`元数据的过时处理。 它还可以安装新软件包，删除旧软件包以及对已安装的或者可用软件包执行查询。yum类似于apt-get和smart等其他高级包管理器。

command是以下之一：

```
* install package1 [package2] [...]
* update [package1] [package2] [...]
* update-to [package1] [package2] [...]
* update-minimal [package1] [package2] [...]
* check-update
* upgrade [package1] [package2] [...]
* upgrade-to [package1] [package2] [...]
* distribution-synchronization [package1] [package2] [...]
* remove | erase package1 [package2] [...]
* autoremove [package1] [...]
* list [...]
* info [...]
* provides | whatprovides feature1 [feature2] [...]
* clean [ packages | metadata | expire-cache | rpmdb | plugins | all ]
* makecache [fast]
* groups [...]
* search string1 [string2] [...]
* shell [filename]
* resolvedep dep1 [dep2] [...]
	(maintained for legacy reasons only - use repoquery or yum provides)
* localinstall rpmfile1 [rpmfile2] [...]
	(maintained for legacy reasons only - use install)
* localupdate rpmfile1 [rpmfile2] [...]
	(maintained for legacy reasons only - use update)
* reinstall package1 [package2] [...]
* downgrade package1 [package2] [...]
* deplist package1 [package2] [...]
* repolist [all|enabled|disabled]
* repoinfo [all|enabled|disabled]
* repository-packages <enabled-repoid> <install|remove|remove-or-reinstall|remove-or-distribution-synchronization> [package2] [...]
* version [ all | installed | available | group-* | nogroups* | grouplist | groupinfo ]
* history [info|list|packages-list|packages-info|summary|addon-info|redo|undo|rollback|new|sync|stats]
* load-transaction [txfile]
* updateinfo [summary | list | info | remove-pkgs-ts | exclude-updates | exclude-all | check-running-kernel]
* fssnapshot [summary | list | have-space | create | delete]
* fs [filters | refilter | refilter-cleanup | du]
* check
* help [command]

除非给出--help或-h选项，否则必须存在上述命令之一。

常用的命令如下：
install
	用于安装最新版本的软件包或软件包组，同时确保满足所有依赖项。

update
	如果在没有指定任何包的情况下，update将更新所有当前安装的包。如果指定了一个或多个包或包，则Yum将仅更新列出的包。在更新包时，yum将确保满足所有依赖关系。

update-to
	此命令的作用类似于“update”，但是要指定包的版本。

check-update
	检查是否有需要更新的包。

remove or erase
	用于从系统中删除指定的包以及依赖包。
```

