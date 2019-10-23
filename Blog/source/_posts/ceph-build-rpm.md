---
title: ceph build rpm
date: 2018-09-13 16:23:02
tags: ceph
---

最近做了一个实验，ceph代码如何build出rpm包？

官方文档：http://docs.ceph.com/docs/master/install/build-ceph/

## 步骤

1、首先需要ceph源码，官方的例子是把ceph源码压缩成tar.bz2

2、然后rpmbuild工具根据ceph源码中的ceph.spec规则进行build

## 详细实验步骤

方法一：

```
就是按照官方文档的思路去做
tar --strip-components=1 -C ~/rpmbuild/SPECS/ --no-anchored -xvjf ~/rpmbuild/SOURCES/ceph-10.2.11.tar.bz2 "ceph.spec"
```

方法二：

```
为了实验方便，我们会下载官方发布出来的.src.rpm包来实验。
.src.rpm会随着rpm同时发布，可以用来移植不同系统平台。

安装依赖
[root@yujiang-dev-20180912135521 ~]# git clone https://github.com/ceph/ceph.git
[root@yujiang-dev-20180912135521 ~]# cd ceph
[root@yujiang-dev-20180912135521 ceph]# ./install-deps.sh 
[root@yujiang-dev-20180907154634 ~]# yum install rpm-build rpmdevtools hdparm libatomic_ops-devel fcgi-devel boost-devel cmake gcc-c++ tree selinux-policy-doc -y

下载官方发布出来的.src.rpm
[root@yujiang-dev-20180907154634 ~]# wget http://download.ceph.com/rpm-jewel/el7/SRPMS/ceph-10.2.11-0.el7.src.rpm

生成rpmbuild目录
[root@yujiang-dev-20180907154634 ~]# rpmdev-setuptree

[root@yujiang-dev-20180907154634 ~]# rpm -i ceph-10.2.11-0.el7.src.rpm
[root@yujiang-dev-20180912135521 ~]# tree rpmbuild/SRPMS/
rpmbuild/SRPMS/
└── ceph-10.2.11-0.el7.centos.src.rpm
[root@yujiang-dev-20180912135521 ~]# tree rpmbuild/SPECS/
rpmbuild/SPECS/
└── ceph.spec
开始构建rpm包
[root@yujiang-dev-20180912135521 ~]# rpmbuild -ba ~/rpmbuild/SPECS/ceph.spec

[root@yujiang-dev-20180912135521 ~]# tree  rpmbuild/RPMS/
rpmbuild/RPMS/
└── x86_64
    ├── ceph-10.2.11-0.el7.centos.x86_64.rpm
    ├── ceph-base-10.2.11-0.el7.centos.x86_64.rpm
    ├── ceph-common-10.2.11-0.el7.centos.x86_64.rpm
    ├── ceph-debuginfo-10.2.11-0.el7.centos.x86_64.rpm
    ├── ceph-devel-compat-10.2.11-0.el7.centos.x86_64.rpm
    ├── cephfs-java-10.2.11-0.el7.centos.x86_64.rpm
    ├── ceph-fuse-10.2.11-0.el7.centos.x86_64.rpm
    ├── ceph-libs-compat-10.2.11-0.el7.centos.x86_64.rpm
    ├── ceph-mds-10.2.11-0.el7.centos.x86_64.rpm
    ├── ceph-mon-10.2.11-0.el7.centos.x86_64.rpm
    ├── ceph-osd-10.2.11-0.el7.centos.x86_64.rpm
    ├── ceph-radosgw-10.2.11-0.el7.centos.x86_64.rpm
    ├── ceph-resource-agents-10.2.11-0.el7.centos.x86_64.rpm
    ├── ceph-selinux-10.2.11-0.el7.centos.x86_64.rpm
    ├── ceph-test-10.2.11-0.el7.centos.x86_64.rpm
    ├── libcephfs1-10.2.11-0.el7.centos.x86_64.rpm
    ├── libcephfs1-devel-10.2.11-0.el7.centos.x86_64.rpm
    ├── libcephfs_jni1-10.2.11-0.el7.centos.x86_64.rpm
    ├── libcephfs_jni1-devel-10.2.11-0.el7.centos.x86_64.rpm
    ├── librados2-10.2.11-0.el7.centos.x86_64.rpm
    ├── librados2-devel-10.2.11-0.el7.centos.x86_64.rpm
    ├── libradosstriper1-10.2.11-0.el7.centos.x86_64.rpm
    ├── libradosstriper1-devel-10.2.11-0.el7.centos.x86_64.rpm
    ├── librbd1-10.2.11-0.el7.centos.x86_64.rpm
    ├── librbd1-devel-10.2.11-0.el7.centos.x86_64.rpm
    ├── librgw2-10.2.11-0.el7.centos.x86_64.rpm
    ├── librgw2-devel-10.2.11-0.el7.centos.x86_64.rpm
    ├── python-ceph-compat-10.2.11-0.el7.centos.x86_64.rpm
    ├── python-cephfs-10.2.11-0.el7.centos.x86_64.rpm
    ├── python-rados-10.2.11-0.el7.centos.x86_64.rpm
    ├── python-rbd-10.2.11-0.el7.centos.x86_64.rpm
    ├── rbd-fuse-10.2.11-0.el7.centos.x86_64.rpm
    ├── rbd-mirror-10.2.11-0.el7.centos.x86_64.rpm
    └── rbd-nbd-10.2.11-0.el7.centos.x86_64.rpm

1 directory, 34 files

解压bz2文件
yum install bzip2 -y
bzip2 -d ceph-12.2.8.tar.bz2
tar xvf ceph-12.2.8.tar
tar zcvf ceph-12.2.8.tar.gz ceph-12.2.8

如果是ceph-14.2.4需要安装
yum -y install centos-release-scl
yum -y install devtoolset-7-gcc devtoolset-7-gcc-c++ devtoolset-7-binutils
scl enable devtoolset-7 bash
yum install ceph-2:14.2.4-0.el7.x86_64
rpmbuild -ba ~/rpmbuild/SPECS/ceph.spec

解包：tar xvf FileName.tar
打包：tar cvf FileName.tar DirName

压缩：bzip2 [原文件名].tar
解压：bunzip2 [原文件名].tar.bz2
```



