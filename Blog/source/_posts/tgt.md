---
title: tgt
date: 2019-09-23 23:53:01
tags: tgt,ceph
---

# 安装编译依赖

```
yum install gcc rpm-build libibverbs-devel librdmacm-devel libaio-devel docbook-style-xsl -y
```

#  

```
[root@dev tgt]# tree scripts/
scripts/
├── build-pkg.sh	#build rpm或deb包
├── checkarch.sh	#
├── checkpatch.pl	#
├── deb
│   ├── changelog
│   ├── compat
│   ├── control
│   ├── copyright
│   ├── init
│   ├── patches
│   │   └── 0001-Use-local-docbook-for-generating-docs.patch
│   ├── rules
│   ├── source
│   │   └── format
│   └── tgt.bash-completion
├── initd.sample
├── Makefile
├── tgt-admin
├── tgt.bashcomp.sh
├── tgt-core-test
├── tgtd.service
├── tgtd.spec
└── tgt-setup-lun

3 directories, 20 files

```

