---
title: Ubuntu 常用操作
date: 2019-11-22 11:19:55
tags: linux
---

Ubuntu版本

```
NAME="Ubuntu"
VERSION="18.04.3 LTS (Bionic Beaver)"
ID=ubuntu
ID_LIKE=debian
PRETTY_NAME="Ubuntu 18.04.3 LTS"
VERSION_ID="18.04"
HOME_URL="https://www.ubuntu.com/"
SUPPORT_URL="https://help.ubuntu.com/"
BUG_REPORT_URL="https://bugs.launchpad.net/ubuntu/"
PRIVACY_POLICY_URL="https://www.ubuntu.com/legal/terms-and-policies/privacy-policy"
VERSION_CODENAME=bionic
UBUNTU_CODENAME=bionic
```

# 常用命令

1、add-apt-repository

add-apt-repository是用于添加apt source.list条目的脚本。它可用于添加任何repository，还提供用于添加Launchpad PPA repository的简写语法（Personal Package Archive，个人软件包存档）。

```
sudo add-apt-repository ppa:g2p/storage
sudo apt-get update
sudo apt-get install bcache-tools
```









