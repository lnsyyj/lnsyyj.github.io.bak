---
title: samba
date: 2019-10-21 19:26:56
tags: samba
---

系统环境

```
cat /etc/redhat-release 
CentOS Linux release 7.4.1708 (Core) 
```

# samba安装

安装samba

```
yum install -y samba

samba Version 4.9.1
```

# Ceph KRBD使用samba

1、首先创建pool

```
ceph osd pool create rbd 8
```

2、在pool中创建rbd

```
rbd create --size 10G rbd/rbd-1

# rbd info rbd/rbd-1
rbd image 'rbd-1':
	size 10GiB in 2560 objects
	order 22 (4MiB objects)
	block_name_prefix: rbd_data.10c06b8b4567
	format: 2
	features: layering, exclusive-lock, object-map, fast-diff, deep-flatten
	flags: 
	create_timestamp: Wed Oct 23 10:04:16 2019
```

3、在linux上创建挂载点

```
mkdir ceph-rbd-1-mountpoint
```

4、map pool中的rbd到linux

```
rbd map rbd/rbd-1

# lsmod | grep rbd
rbd                    83733  2 
libceph               306742  1 rbd

# sudo rbd showmapped
id pool image snap device    
0  rbd  rbd-1 -    /dev/rbd0
```

5、格式化文件系统

```
mkfs.xfs /dev/rbd/rbd/rbd-1
```

6、mount到挂载点

```
mount /dev/rbd/rbd/rbd-1 /root/ceph-rbd-1-mountpoint/

# df -Th
Filesystem     Type      Size  Used Avail Use% Mounted on
/dev/rbd0      xfs        10G   33M   10G   1% /root/ceph-rbd-1-mountpoint
```

7、samba预操作（关闭防火墙）

```
systemctl stop firewalld
setenforce 0

持久化关闭selinux
vim /etc/selinux/config
SELINUX=disabled
```

8、配置samba（samba默认会共享linux home目录，如果自定义需要自己配置），并重启服务

```
vim /etc/samba/smb.conf

[global]
        workgroup = SAMBA
        security = user
        passdb backend = tdbsam
        printing = cups
        printcap name = cups
        load printers = yes
        cups options = raw

[homes]
        comment = Home Directories
        valid users = %S, %D%w%S
        browseable = No
        read only = No
        inherit acls = Yes
        writable = Yes

[ceph]
        comment = ceph
        path = /root/ceph-rbd-1-mountpoint/
        valid users = %S, %D%w%S
        browseable = No
        read only = No
        inherit acls = Yes
        writable = Yes

[printers]
        comment = All Printers
        path = /var/tmp
        printable = Yes
        create mask = 0600
        browseable = No

[print$]
        comment = Printer Drivers
        path = /var/lib/samba/drivers
        write list = @printadmin root
        force group = @printadmin
        create mask = 0664
        directory mask = 0775

systemctl restart smb
```

9、为samba添加ceph用户

```
smbpasswd -a ceph
```

10、客户端访问samba

```
\\10.20.10.23\ceph
```

window清除samba连接，重新登录

```
C:\Users\JiangYu>net use
会记录新的网络连接。
状态       本地        远程                      网络
-------------------------------------------------------------------------------
OK                     \\10.20.10.23\IPC$        Microsoft Windows Network
命令成功完成。


C:\Users\JiangYu>net use \\10.20.10.23\IPC$ /del
\\10.20.10.23\IPC$ 已经删除。


C:\Users\JiangYu>net use
会记录新的网络连接。
列表是空的。
```

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/Blog/source/_posts/samba/linuxcast-smb-登录后查看文件-1.png)

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/Blog/source/_posts/samba/linuxcast-smb-登录后查看文件-2.png)

11、因为是使用root用户创建的ceph-rbd-1-mountpoint目录，所以ceph用户没有权限对这个目录进行写操作，需要修改目录所有者，修改后就可以对这个目录进行创建文件等写操作了

```
chown ceph:ceph ceph-rbd-1-mountpoint/
```

# samba配置

1、查看配置文件帮助手册

```
man smb.conf 5
```

2、smb.conf配置文件语法

```
;或#开头为注释
```

3、检查配置文件语法命令

```
testparm /etc/samba/smb.conf
```

4、Security-Enhanced Linux (SELinux) Notes

```
启用samba_domain_controller Boolean以允许Samba PDC使用useradd和groupadd二进制。以root用户身份运行以下命令以启用此Boolean：
setsebool -P samba_domain_controller on

如果要通过Samba共享home directories，请打开samba_enable_home_dirs Boolean。以root用户身份运行以下命令以启用此Boolean：
setsebool -P samba_enable_home_dirs on

如果创建新directories，例如新的顶级directories，请使用samba_share_t对其进行标记，以便SELinux允许Samba对其进行读写。不要使用samba_share_t标记系统目录，例如/etc/和/home/，因为这样的目录应该已经具有SELinux label了。

运行"ls -ldZ /path/to/directory"命令以查看给定目录的当前SELinux label。

仅在创建的文件和目录上设置SELinux labels。使用chcon命令临时更改label：
chcon -t samba_share_t /path/to/directory

重新标记文件系统或运行诸如restorecon之类的命令时，通过chcon进行的更改将丢失。

使用samba_export_all_ro或samba_export_all_rw Boolean共享系统目录。 
要共享这样的目录并仅允许只读权限：
setsebool -P samba_export_all_ro on
要共享此类目录并允许读写权限：
setsebool -P samba_export_all_rw on

要运行脚本（preexec/root prexec/print command/...），请将它们复制到/var/lib/samba/scripts/目录中，以便SELinux允许smbd运行它们。
注意，如果将脚本移动到/var/lib/samba/scripts/，它们将保留其现有的SELinux labels，这些labels可能是SELinux不允许smbd运行的labels。复制脚本将得到正确的SELinux labels。
以root用户身份运行"restorecon -R -v /var/lib/samba/scripts"命令，以将正确的SELinux labels应用于这些文件。
```

5、配置文件段

```
#======================= Global Settings =====================================

[global]

# ----------------------- Network-Related Options -------------------------
#
# workgroup = Windows NT domain name 或 workgroup name, 例如, MYGROUP.
#
# server string = 等效于Windows NT Description字段（用于描述）.
#
# netbios name = 用于指定不与hostname绑定的server name，最多15个字符。
#
# interfaces = 用于将Samba配置为侦听多个network interfaces。如果您有多个interfaces，则可以使用"interfaces ="选项来配置Samba侦听哪些interface。 切勿忽略localhost interface (lo)。
#
# hosts allow = 允许连接的主机。This option can also be used on a per-share basis.
#
# hosts deny = 不允许主机连接。 This option can also be used on a per-share basis.
#
	workgroup = MYGROUP
	server string = Samba Server Version %v

;	netbios name = MYSERVER

;	interfaces = lo eth0 192.168.12.2/24 192.168.13.2/24
;	hosts allow = 127. 192.168.12. 192.168.13.

# --------------------------- Logging Options -----------------------------
#
# log file = 指定日志文件写入的位置以及它们的拆分方式。
#
# max log size = 指定允许的最大日志文件大小。 日志文件达到"max log size"指定的大小时，将对其进行rotated。
#

	# log files split per-machine:
	log file = /var/log/samba/log.%m
	# maximum size of 50KB per log file, then rotate:
	max log size = 50

# ----------------------- Standalone Server Options ------------------------
#
# security = Samba运行的模式。可以将其设置为user, share (不建议使用), 或 server (不建议使用).
#
# passdb backend = 用于存储用户信息的backend（后端）。新安装应使用tdbsam或ldapsam。tdbsam不需要其他配置。"smbpasswd"可用于向后兼容。
#

	security = user
	passdb backend = tdbsam


# ----------------------- Domain Members Options ------------------------
#
# security = 必须设置为 domain 或 ads.
#
# passdb backend = 用于存储用户信息的backend（后端）。新安装应使用tdbsam或ldapsam。tdbsam不需要其他配置。"smbpasswd"可用于向后兼容。
#
# realm = 当设置了"security = ads"选项时才会用到该选项。
#
# password server = 当设置了"security = server"选项或无法使用DNS定位Domain Controller时，才使用此选项。参数列表可以包括My_PDC_Name, [My_BDC_Name], 和[My_Next_BDC_Name]
#
# password server = My_PDC_Name [My_BDC_Name] [My_Next_BDC_Name]
#
# 使用 "password server = *" 自动定位 Domain Controllers.

;	security = domain
;	passdb backend = tdbsam
;	realm = MY_REALM

;	password server = <NT-Server-Name>

# ----------------------- Domain Controller Options ------------------------
#
# security = 必须将domain controllers设置为user。
#
# passdb backend = 用于存储用户信息的backend（后端）。新安装应使用tdbsam或ldapsam。tdbsam不需要其他配置。"smbpasswd"可用于向后兼容。
#
# domain master = 将Samba指定为Domain Master Browser，从而允许Samba整理subnets之间的browse lists。 如果您已经有Windows NT domain controller执行此任务，请不要使用"domain master"选项。
#
# domain logons = 允许Samba为Windows workstations提供网络登录服务。
#
# logon script = 指定在登录时在客户端上运行的脚本。 必须在名为NETLOGON的共享中提供这些脚本。
#
# logon path = 指定（使用UNC path）用户配置文件的存储位置。
#
#
;	security = user
;	passdb backend = tdbsam

;	domain master = yes
;	domain logons = yes

	# 以下登录脚本名称由machine name确定(%m):
;	logon script = %m.bat
	# 以下登录脚本名称由UNIX user确定:
;	logon script = %u.bat
;	logon path = \\%L\Profiles\%u
	# 使用empty path禁用profile support
;	logon path =

	# 可以在domain controller或独立machine上使用各种脚本来添加或删除相应的UNIX帐户：

;	add user script = /usr/sbin/useradd "%u" -n -g users
;	add group script = /usr/sbin/groupadd "%g"
;	add machine script = /usr/sbin/useradd -n -c "Workstation (%u)" -M -d /nohome -s /bin/false "%u"
;	delete user script = /usr/sbin/userdel "%u"
;	delete user from group script = /usr/sbin/userdel "%u" "%g"
;	delete group script = /usr/sbin/groupdel "%g"


# ----------------------- Browser Control Options ----------------------------
#
# local master = 设置为no时，Samba不会成为网络上的master browser。 设置为yes时，将应用常规election（选举） rules。
#
# os level = 确定服务器在master browser选举中的优先级。 默认值应该合理。
#
# preferred master = 设置为yes时，Samba会在启动时强制进行local browser选举（并使其赢得选举的几率略高）。
#
;	local master = no
;	os level = 33
;	preferred master = yes

#----------------------------- Name Resolution -------------------------------
#
# 本节详细介绍了对 Windows Internet Name Service (WINS) 的支持.
#
# 注意：Samba可以是WINS服务器，也可以是WINS客户端，但不能同时是两者。
#
# wins support = 设置为yes时，Samba的NMBD组件启用其WINS服务器。
#
# wins server = 告诉Samba的NMBD组件是WINS客户端。
#
# wins proxy = 设置为yes时，Samba代表不支持WINS的客户端回答name resolution查询。 为此，网络上至少必须有一个WINS服务器。 默认为no。
#
# dns proxy = 设置为yes时，Samba尝试通过DNS nslookups解析NetBIOS名称。

;	wins support = yes
;	wins server = w.x.y.z
;	wins proxy = yes

;	dns proxy = yes

# --------------------------- Printing Options -----------------------------
#
# 本部分中的选项使您可以配置non-default（非默认） printing system。
#
# load printers = 设置为yes时，将自动加载printers列表，而不是单独进行设置。
#
# cups options = 允许您将选项传递到CUPS库。 例如，将此选项设置为raw，则可以在Windows客户端上使用驱动程序。
#
# printcap name = 用于指定备用的printcap文件。
#

	load printers = yes
	cups options = raw

;	printcap name = /etc/printcap
	# 自动获取UNIX System V系统上的printers列表：
;	printcap name = lpstat
;	printing = cups

# --------------------------- File System Options ---------------------------
#
# 如果文件系统支持扩展属性，并且启用了这些属性（通常通过"user_xattr" mount选项），则可以取消注释本节中的选项。
# 这些选项允许管理员指定DOS属性存储在扩展属性中，并且还确保Samba不会更改permission bits。
#
# 注意：这些选项可以按per-share使用。 全局设置它们（在[global]部分中）使它们成为所有共享的默认值。

;	map archive = no
;	map hidden = no
;	map read only = no
;	map system = no
;	store dos attributes = yes


#============================ Share Definitions ==============================

[homes]
	comment = Home Directories
	browseable = no
	writable = yes
;	valid users = %S
;	valid users = MYDOMAIN\%S

[printers]
	comment = All Printers
	path = /var/spool/samba
	browseable = no
	guest ok = no
	writable = no
	printable = yes

# 取消注释以下内容，并为Domain Logons创建netlogon目录：
;	[netlogon]
;	comment = Network Logon Service
;	path = /var/lib/samba/netlogon
;	guest ok = yes
;	writable = no
;	share modes = no

# 取消注释以下内容以提供特定的roaming profile share。
# 默认为使用用户的home目录：
;	[Profiles]
;	path = /var/lib/samba/profiles
;	browseable = no
;	guest ok = yes

# 一个只读公共目录，但"staff" group中的用户（具有写权限）除外：
;	[public]
;	comment = Public Stuff
;	path = /home/samba
;	public = yes
;	writable = no
;	printable = no
;	write list = +staff
```

6、用户操作

```
smbpasswd -a 增加用户（linux系统用户）
smbpasswd -d 冻结用户
smbpasswd -e 解冻用户
smbpasswd -n 将用户的密码设置成空.
```



# LinuxCast笔记

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/Blog/source/_posts/samba/linuxcast-smb-1.png)

![](https://github.com/lnsyyj/lnsyyj.github.io/blob/hexo/Blog/source/_posts/samba/linuxcast-smb-2.png?raw=true)

![](https://github.com/lnsyyj/lnsyyj.github.io/blob/hexo/Blog/source/_posts/samba/linuxcast-smb-3.png?raw=true)

![](https://github.com/lnsyyj/lnsyyj.github.io/blob/hexo/Blog/source/_posts/samba/linuxcast-smb-4.png?raw=true)

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/Blog/source/_posts/samba/linuxcast-smb-5.png)

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/Blog/source/_posts/samba/linuxcast-smb-6.png)

![](https://github.com/lnsyyj/lnsyyj.github.io/blob/hexo/Blog/source/_posts/samba/linuxcast-smb-7.png?raw=true)

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/Blog/source/_posts/samba/linuxcast-smb-8.png)

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/Blog/source/_posts/samba/linuxcast-smb-9.png)

![](https://github.com/lnsyyj/lnsyyj.github.io/blob/hexo/Blog/source/_posts/samba/linuxcast-smb-10.png?raw=true)

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/Blog/source/_posts/samba/linuxcast-smb-11.png)

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/Blog/source/_posts/samba/linuxcast-smb-windows登陆.png)





# 参考资料

【1】https://blog.csdn.net/skdkjzz/article/details/42101363