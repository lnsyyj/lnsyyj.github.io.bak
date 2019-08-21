---
title: git 常用操作
date: 2019-08-20 21:12:49
tags: git
---

视频地址：<https://www.youtube.com/channel/UCPhn2rCqhu0HdktsFjixahA>

原文地址：<https://github.com/twtrubiks/Git-Tutorials>

本文档只作为学习笔记，方便自己速查，如果想系统学习请看沈老师的视频和github。

# 如何加速大型repo clone速度

- --depth参数：只下载最新1次commits log，默认会加--single-branch（只clone单分支，无法checkout）。如果想clone所有分支需要加--no-single-branch（git clone https://github.com/ceph/ceph.git --depth 1 --no-single-branch），可以checkout。

```
yujiangdeMacBook-Pro-13:test yujiang$ git clone https://github.com/ceph/ceph.git --depth 1
Cloning into 'ceph'...
remote: Enumerating objects: 8395, done.
remote: Counting objects: 100% (8395/8395), done.
remote: Compressing objects: 100% (7508/7508), done.
remote: Total 8395 (delta 1133), reused 2722 (delta 467), pack-reused 0
Receiving objects: 100% (8395/8395), 21.01 MiB | 1.52 MiB/s, done.
Resolving deltas: 100% (1133/1133), done.
Checking out files: 100% (8847/8847), done.
yujiangdeMacBook-Pro-13:test yujiang$ cd ceph/
yujiangdeMacBook-Pro-13:ceph yujiang$ git log
commit 6b0ef5dc3c550cd8d17c830156541dd491e9a57a (grafted, HEAD -> master, origin/master, origin/HEAD)
Author: Alfredo Deza <adeza@redhat.com>
Date:   Tue Aug 20 09:32:05 2019 -0400

    Merge pull request #29762 from alfredodeza/bz-1738379
    
    ceph-volume: use the OSD identifier when reporting success
    
    Reviewed-by: Jan Fajerski <jfajerski@suse.com>

```

