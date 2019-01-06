---
title: DNS bind搭建
date: 2019-01-05 17:22:30
tags: DNS
---

# bind centos7 搭建

【图片预留】

首先安装bind rpm

```
[root@centos ~]# yum install -y bind bind-chroot bind-utils
```

【图片预留】

拷贝配置文件模板

```
[root@centos ~]# cp -rv /usr/share/doc/bind-9.9.4/sample/etc/* /var/named/chroot/etc/
"/usr/share/doc/bind-9.9.4/sample/etc/named.conf" -> "/var/named/chroot/etc/named.conf"
"/usr/share/doc/bind-9.9.4/sample/etc/named.rfc1912.zones" -> "/var/named/chroot/etc/named.rfc1912.zones"
[root@centos ~]# cp -rv /usr/share/doc/bind-9.9.4/sample/var/* /var/named/chroot/var/
"/usr/share/doc/bind-9.9.4/sample/var/named/data" -> "/var/named/chroot/var/named/data"
"/usr/share/doc/bind-9.9.4/sample/var/named/my.external.zone.db" -> "/var/named/chroot/var/named/my.external.zone.db"
"/usr/share/doc/bind-9.9.4/sample/var/named/my.internal.zone.db" -> "/var/named/chroot/var/named/my.internal.zone.db"
"/usr/share/doc/bind-9.9.4/sample/var/named/named.ca" -> "/var/named/chroot/var/named/named.ca"
"/usr/share/doc/bind-9.9.4/sample/var/named/named.empty" -> "/var/named/chroot/var/named/named.empty"
"/usr/share/doc/bind-9.9.4/sample/var/named/named.localhost" -> "/var/named/chroot/var/named/named.localhost"
"/usr/share/doc/bind-9.9.4/sample/var/named/named.loopback" -> "/var/named/chroot/var/named/named.loopback"
"/usr/share/doc/bind-9.9.4/sample/var/named/slaves" -> "/var/named/chroot/var/named/slaves"
"/usr/share/doc/bind-9.9.4/sample/var/named/slaves/my.ddns.internal.zone.db" -> "/var/named/chroot/var/named/slaves/my.ddns.internal.zone.db"
"/usr/share/doc/bind-9.9.4/sample/var/named/slaves/my.slave.internal.zone.db" -> "/var/named/chroot/var/named/slaves/my.slave.internal.zone.db"
```

修改配置文件

```
[root@centos ~]# vim /var/named/chroot/etc/named.conf 
/*
 Sample named.conf BIND DNS server 'named' configuration file
 for the Red Hat BIND distribution.

 See the BIND Administrator's Reference Manual (ARM) for details about the
 configuration located in /usr/share/doc/bind-{version}/Bv9ARM.html
*/

options
{
        // Put files that named is allowed to write in the data/ directory:
        directory               "/var/named";           // "Working" directory

        /*
          Specify listenning interfaces. You can use list of addresses (';' is
          delimiter) or keywords "any"/"none"
        */
        //listen-on port 53     { any; };
        listen-on port 53       { 127.0.0.1; };

        //listen-on-v6 port 53  { any; };
        listen-on-v6 port 53    { ::1; };

};

zone "linuxcast.net" {
        type master;
        file "linuxcast.net.zone"; /* bind会自动到/var/named/chroot/var/named/中找linuxcast.net.zone文件 */
};

```



```
[root@centos ~]# cp -rv /var/named/chroot/var/named/named.localhost /var/named/chroot/var/named/linuxcast.net.zone
"/var/named/chroot/var/named/named.localhost" -> "/var/named/chroot/var/named/linuxcast.net.zone"

[root@centos ~]# vim /var/named/chroot/var/named/linuxcast.net.zone
$TTL 1D
@       IN SOA  @ rname.invalid. (
                                        0       ; serial
                                        1D      ; refresh
                                        1H      ; retry
                                        1W      ; expire
                                        3H )    ; minimum
        NS      @
        A       127.0.0.1
        AAAA    ::1

www     IN      A       192.168.56.101
mail    IN      A       192.168.56.101
        IN      MX      10      mail.linuxcast.net.

```



启动bind服务

```
[root@centos ~]# systemctl start named.service

```

