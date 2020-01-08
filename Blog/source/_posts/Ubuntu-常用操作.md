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

sudo apt-get install -y software-properties-common
sudo add-apt-repository ppa:ansible/ansible-2.8
sudo apt-get update
sudo apt-get -y install ansible
```

2、vim 粘贴串行问题

```
在粘贴前设置（粘贴前不会自动缩进）
set paste

在粘贴后恢复
set nopaste
```

3、安装package常用操作

````
sudo apt clean   #清空缓存
sudo apt install -d software_name   #只下载不安装，缓存位置 /var/cache/apt/archives
sudo dpkg -i *.deb

apt-get install -f   修复损坏的软件包，尝试卸载出错的包，重新安装正确版本
````



### 设置网络

```
root@yujiang-ceph-1:~# cat /etc/netplan/50-cloud-init.yaml 
# This file is generated from information provided by
# the datasource.  Changes to it will not persist across an instance.
# To disable cloud-init's network configuration capabilities, write a file
# /etc/cloud/cloud.cfg.d/99-disable-network-config.cfg with the following:
# network: {config: disabled}
network:
    ethernets:
        ens160:
            dhcp4: false
            addresses: [192.168.1.127/24]
            gateway4: 192.168.1.1
            nameservers:
                    addresses: [192.168.1.1, 114.114.114.114]
    version: 2
```



### 编译deb

```
解压xz文件
xz -d prometheus_2.1.0+ds-1.debian.tar.xz
tar -xvf prometheus_2.1.0+ds-1.debian.tar

压缩xz文件
tar cvf prometheus_2.1.0+ds-1.debian.tar debian/
xz -z prometheus_2.1.0+ds-1.debian.tar

apt-get install debhelper dh-golang golang-github-aws-aws-sdk-go-dev golang-github-azure-azure-sdk-for-go-dev golang-github-azure-go-autorest-dev golang-github-cespare-xxhash-dev golang-github-cockroachdb-cmux-dev golang-github-fsnotify-fsnotify-dev golang-github-go-kit-kit-dev golang-github-gogo-protobuf-dev golang-github-golang-snappy-dev golang-github-gophercloud-gophercloud-dev golang-github-grpc-ecosystem-grpc-gateway-dev golang-github-hashicorp-go-cleanhttp-dev golang-github-hashicorp-serf-dev golang-github-miekg-dns-dev golang-github-mwitkow-go-conntrack-dev golang-github-opentracing-contrib-go-stdlib-dev golang-github-opentracing-opentracing-go-dev golang-github-pkg-errors-dev golang-github-prometheus-client-golang-dev golang-github-prometheus-client-model-dev golang-github-prometheus-common-dev golang-github-prometheus-tsdb-dev golang-github-samuel-go-zookeeper-dev golang-go golang-golang-x-net-dev golang-golang-x-oauth2-google-dev golang-golang-x-time-dev golang-google-api-dev golang-google-genproto-dev golang-google-grpc-dev golang-gopkg-alecthomas-kingpin.v2-dev golang-gopkg-yaml.v2-dev

    dpkg-buildpackage -uc -us

dpkg-buildpackage -rfakeroot -Tclean
```







