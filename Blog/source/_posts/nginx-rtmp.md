---
title: nginx rtmp
date: 2018-11-13 22:47:28
tags: nginx
---

## 编译安装

```
克隆nginx项目与nginx-rtmp-module

[root@cephJ ~]# yum install gcc pcre-devel openssl-devel epel-release -y

[root@cephJ ~]# rpm --import http://li.nux.ro/download/nux/RPM-GPG-KEY-nux.ro
[root@cephJ ~]# rpm -Uvh http://li.nux.ro/download/nux/dextop/el7/x86_64/nux-dextop-release-0-5.el7.nux.noarch.rpm
[root@cephJ ~]# yum -y install ffmpeg ffmpeg-devel

[root@cephJ ~]# cd github/
[root@cephJ github]# git clone https://github.com/nginx/nginx.git
[root@cephJ github]# cd nginx
[root@cephJ github]# git checkout -b myrelease-1.15.6 release-1.15.6

[root@cephJ github]# git clone https://github.com/arut/nginx-rtmp-module.git
[root@cephJ github]# cd nginx-rtmp-module
[root@cephJ github]# git checkout -b myv1.2.1 v1.2.1

[root@cephJ ~]# tree github/ -L 1
github/
├── nginx
└── nginx-rtmp-module

[root@cephJ ~]# cd github/nginx
[root@cephJ nginx]# ./auto/configure --prefix=/usr/bin/ --add-module=../nginx-rtmp-module/ --with-http_ssl_module --with-debug
[root@cephJ nginx]# make -j 8
[root@cephJ nginx]# make install
```



## 配置nginx



```
[root@cephJ ~]# vim /usr/local/nginx/conf/nginx.conf
# 添加如下配置
rtmp {                          # RTMP服务
   server {
       listen 1935;             # 服务端口
       chunk_size 4096;         # 数据传输块的大小
       application vod {
           play /root/videos/;  # 视频文件存放位置
       }
   }
}


启动nginx
[root@cephJ ~]# /usr/local/nginx/sbin/nginx
[root@cephJ ~]# ps -ef | grep nginx
root     20015     1  0 11:24 ?        00:00:00 nginx: master process /usr/local/nginx/sbin/nginx
nobody   20016 20015  0 11:24 ?        00:00:00 nginx: worker process
root     20019 12686  0 11:24 pts/2    00:00:00 grep --color=auto nginx


```





## 概念

### FFmpeg

FFmpeg 是一个库和工具的集合，用于处理多媒体，如音频、视频、字幕和相关元数据。（FF指的是Fast Forward ——快速前进）

```
相关链接：
【1】https://zh.wikipedia.org/wiki/FFmpeg
【2】https://github.com/FFmpeg/FFmpeg
```

