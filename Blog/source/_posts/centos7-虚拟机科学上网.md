---
title: centos7 虚拟机科学上网
date: 2018-08-21 21:49:05
tags: 科学上网
---

参考：https://cloud.tencent.com/developer/article/1122336

```
[root@k8s-master ~]# yum groupinstall "Development Tools" -y && yum install -y m2crypto python-setuptools && easy_install pip && python -m pip install --upgrade pip && python -m pip install --upgrade setuptools && pip install shadowsocks

[root@k8s-master ~]# vi /etc/shadowsocks.json
{
  "server": "38.*.*.*",
  "server_port": 7777,
  "password": "pwd",
  "method": "aes-256-cfb",
  "local_address":"127.0.0.1",
  "local_port":1080
}

[root@k8s-master ~]# sslocal  -c /etc/shadowsocks.json

[root@k8s-master ~]# yum install epel-release -y && yum install -y privoxy
[root@k8s-master ~]# vi /etc/privoxy/config
forward-socks5t   /               127.0.0.1:1080
listen-address  127.0.0.1:8118

[root@k8s-master ~]# vi  ~/.bashrc
export http_proxy=http://127.0.0.1:8118
export https_proxy=http://127.0.0.1:8118
export ftp_proxy=http://127.0.0.1:8118
[root@k8s-master ~]# source ~/.bashrc 

[root@k8s-master ~]# systemctl restart privoxy.service && systemctl status privoxy.service
```



