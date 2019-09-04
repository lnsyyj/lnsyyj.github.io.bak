---
title: Centos 7 升级内核(Centos 7 upgrade kernel)
date: 2019-09-04 15:23:20
tags: linux
---

1、查看当前版本kernel (Show current version of kernel)

```
[root@dev ~]# uname -r
3.10.0-957.el7.x86_64
```

2、导入公钥 (Import the public key)

```
[root@dev ~]# rpm --import https://www.elrepo.org/RPM-GPG-KEY-elrepo.org
```

3、为CentOS-7安装ELRepo (install ELRepo for CentOS-7)

```
[root@dev ~]# yum install -y https://www.elrepo.org/elrepo-release-7.0-4.el7.elrepo.noarch.rpm
```

4、安装yum-plugin-fastestmirror (Install yum-plugin-fastestmirror)

```
[root@dev ~]# yum install -y yum-plugin-fastestmirror
```

5、查看可以安装的kernel (Show the kernel that can be installed)

```
[root@dev ~]# yum --enablerepo=elrepo-kernel provides kernel
```

6、安装最新kernel，或安装执行版本的kernel (Install the latest kernel, or install the executable version of the kernel)

```
[root@dev ~]# yum --enablerepo=elrepo-kernel install kernel-ml
or
[root@dev ~]# yum --enablerepo=elrepo-kernel install kernel-lt-4.4.190-1.el7.elrepo.x86_64
```

7、设置默认的启动内核 (Set the default boot kernel)

```
[root@dev ~]# cat /boot/grub2/grub.cfg | grep "CentOS Linux"
menuentry 'CentOS Linux (4.4.190-1.el7.elrepo.x86_64) 7 (Core)' --class centos --class gnu-linux --class gnu --class os --unrestricted $menuentry_id_option 'gnulinux-3.10.0-862.el7.x86_64-advanced-a66de569-ad95-4599-9f1e-37c19744ace0' {
menuentry 'CentOS Linux (3.10.0-957.21.3.el7.x86_64) 7 (Core)' --class centos --class gnu-linux --class gnu --class os --unrestricted $menuentry_id_option 'gnulinux-3.10.0-862.el7.x86_64-advanced-a66de569-ad95-4599-9f1e-37c19744ace0' {
menuentry 'CentOS Linux (3.10.0-862.3.2.el7.x86_64) 7 (Core)' --class centos --class gnu-linux --class gnu --class os --unrestricted $menuentry_id_option 'gnulinux-3.10.0-862.el7.x86_64-advanced-a66de569-ad95-4599-9f1e-37c19744ace0' {
menuentry 'CentOS Linux (3.10.0-862.el7.x86_64) 7 (Core)' --class centos --class gnu-linux --class gnu --class os --unrestricted $menuentry_id_option 'gnulinux-3.10.0-862.el7.x86_64-advanced-a66de569-ad95-4599-9f1e-37c19744ace0' {
menuentry 'CentOS Linux (0-rescue-167a8b301e76475680ccb38e7d691aab) 7 (Core)' --class centos --class gnu-linux --class gnu --class os --unrestricted $menuentry_id_option 'gnulinux-0-rescue-167a8b301e76475680ccb38e7d691aab-advanced-a66de569-ad95-4599-9f1e-37c19744ace0' {

[root@dev ~]# grub2-set-default "CentOS Linux (4.4.190-1.el7.elrepo.x86_64) 7 (Core)"

[root@dev ~]# grub2-editenv list
saved_entry=CentOS Linux (4.4.190-1.el7.elrepo.x86_64) 7 (Core)
```

8、重启机器 (Restart the machine)

```
[root@dev ~]# reboot
```

9、重启后验证是否成功 (Verify successful after reboot)

```
[root@dev ~]# uname -r
4.4.190-1.el7.elrepo.x86_64
```

10、删除老kernel，--count flag用于指定要在系统上保留的内核数 (Delete the old kernel, the --count flag is used to specify the number of cores to keep on the system)

```
[root@dev ~]# yum install -y yum-utils

[root@dev ~]# package-cleanup --oldkernels --count=2
```

