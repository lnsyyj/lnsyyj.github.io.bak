---
title: centos创建内部mirror
date: 2019-08-20 11:10:09
tags: centos
---

公司内部搭建mirror的好处：可以节省大量时间、控制版本、节省公司外网带宽（尤其是国内很多公司基于开源软件搞定制化二次开发），同时也可以纳入发布流程当中。

yum可以通过FTP或HTTP传输文件，这里只实验HTTP方式。

# server端（搭建的mirror端）

1、首先安装Nginx

```
sudo yum install -y epel-release && sudo yum install -y nginx 
```

2、安装createrepo工具（负责将.rpm放到repomd repository）

```
sudo yum install -y createrepo yum-utils
```

3、在本地创建目录，存储repositories

```
sudo mkdir -p /usr/share/nginx/repos/ceph/rpm-nautilus/el7/{SRPMS,aarch64,noarch,x86_64}
```

4、修改centos源

```
cat /etc/yum.repos.d/ceph_163.repo 

# $basearch is x86_64, can be modified
[ceph]
baseurl = http://mirrors.163.com/ceph/rpm-nautilus/el7/$basearch
gpgcheck = 0
gpgkey = http://mirrors.163.com/ceph/keys/release.asc
name = Ceph Stable $basearch repo
priority = 2

[noarch]
baseurl = http://mirrors.163.com/ceph/rpm-nautilus/el7/noarch
gpgcheck = 0
gpgkey = http://mirrors.163.com/ceph/keys/release.asc
name = Ceph Stable noarch repo
priority = 2
```

5、导入GPG Key

```
curl https://mirrors.163.com/ceph/keys/release.asc | gpg --import -
```

6、下载官方repositories到本地服务器

```
sudo reposync -g -l -d -m --repoid=ceph --newest-only --download-metadata --download_path=/usr/share/nginx/repos/ceph/rpm-nautilus/el7/x86_64/
sudo reposync -g -l -d -m --repoid=noarch --newest-only --download-metadata --download_path=/usr/share/nginx/repos/ceph/rpm-nautilus/el7/noarch/

同步后，目录层级会不同，在download_path后面会自动加一层repoid指定的名字，需要自行调整。
```

7、创建new repository

```
sudo createrepo /usr/share/nginx/repos/ceph/rpm-nautilus/el7/x86_64/
sudo createrepo /usr/share/nginx/repos/ceph/rpm-nautilus/el7/noarch/

这时会在相应的目录中生成repodata
```

8、修改Nginx配置

```
server { 
	# 修改
	autoindex on;
	root         /usr/share/nginx/repos/;
}

启动nginx
systemctl start nginx && systemctl enable nginx
```

# client端（使用mirror端）

在/etc/yum.repos.d/中增加新的源

```
[root@dev ~]# cat /etc/yum.repos.d/ceph_stable.repo 
[ceph_stable]
baseurl = http://10.121.9.103/ceph/rpm-nautilus/el7/$basearch
gpgcheck = 1
gpgkey = https://download.ceph.com/keys/release.asc
name = Ceph Stable $basearch repo
priority = 2

[ceph_stable_noarch]
baseurl = http://10.121.9.103/ceph/rpm-nautilus/el7/noarch
gpgcheck = 1
gpgkey = https://download.ceph.com/keys/release.asc
name = Ceph Stable noarch repo
priority = 2
```

