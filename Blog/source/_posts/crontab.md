---
title: crontab
date: 2018-08-04 16:45:34
tags: crontab
---

课程地址：https://www.imooc.com/learn/1009

### 安装

```
yum install cronie crontabs -y
```

### 验证CROND服务

验证crond服务和crontab工具（centos7)

检查crond服务是否安装及启动：

```
yum list cronie && systemctl status crond
```

检查crontab工具是否安装：

```
yum list crontabs && which crontab && crontab -l
```

### CRONTAB架构

```
   文件                              解析                              守护进程
                                 crontab工具
* * * * *                        ==========>                            CROND
my command
按照格式编写定时任务                                            定时检查是否有任务需要执行
```

例如：

```
(1)编辑任务列表
crontab -e
(2)查看任务列表
crontab -l
(3)重启crond服务，查看crond服务状态
systemctl restart crond
systemctl status crond
```

### CRONTAB文件格式

```
*  *  *  *  *     my command
分 时 日 月 周     要运行的命令

分：范围0-59
时：范围0-23
日：范围1-31
月：范围1-12
周：范围0-6
my command：范围命令或脚本

*：取值范围内的数字，通常代表对应时间区间内所涵盖的所有数字
/：代表每，通常与*组合。例如2/*在"分"这个占位符中代表每两分钟，5/*在"时"这个占位符代表每5个小时
-：代表某个数字到某个数字之间的区间，2-10在"分"占位符中代表第2分钟到第10分钟
,：分开几个离散的数字。2,10 代表第2分钟和第10分钟

例子：
	crontab -e
	* * * * * echo -e "Hello" > /root/crontabtest.output
```

### CRONTAB配置文件

系统配置文件

```
############### /etc/crontab ###############
SHELL=/bin/bash
PATH=/sbin:/bin:/usr/sbin:/usr/bin
MAILTO=root

# For details see man 4 crontabs

# Example of job definition:
# .---------------- minute (0 - 59)
# |  .------------- hour (0 - 23)
# |  |  .---------- day of month (1 - 31)
# |  |  |  .------- month (1 - 12) OR jan,feb,mar,apr ...
# |  |  |  |  .---- day of week (0 - 6) (Sunday=0 or 7) OR sun,mon,tue,wed,thu,fri,sat
# |  |  |  |  |
# *  *  *  *  * user-name  command to be executed
```

系统用户crontab配置文件保存目录（crontab -e）

```
############### /var/spool/cron/ ###############
文件以linux用户区分
root：/var/spool/cron/root
user01：/var/spool/cron/user01
```

### CRONTAB环境变量

添加PATH到/etc/crontab

```
############### /etc/crontab ###############
PATH=/sbin:/bin:/usr/sbin:/usr/bin:/usr/local/jdk1.8.0_111/bin

* * * * * root java -version 2> /root/temp.out
```

在执行具体任务前引入系统/用户环境变量（推荐）

```
30 2 * * * source /etc/profile;sh /root/test.sh
30 2 * * * source ~/.bash_profile;sh /root/test.sh
systemctl restart crond
```

### CRONTAB日志

Cron日志保存在系统目录/var/log/cron

```
tail -n 2 /var/log/cron
```

### 实战

CRONTAB清理系统日志

```
查看当前目录所有文件大小
# du -sh *
* 1 * * * cat /dev/null > /var/log/messages
```

