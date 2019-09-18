---
title: elastic stack - filebeat
date: 2019-09-17 15:28:40
tags: elk
---

ELK是Elasticsearch、Logstash、Kibana的简称，这三者是核心套件，并不是全部。一般用于集中搜集并展示日志。

# filebeat

filebeat的主要作用是采集日志文件，是轻量级的日志采集器。部署filebeat非常简单，这里就不做介绍了。我们来看一下filebeat的配置与使用。

## 简单配置及演示：

​	这里我们先看一下filebeat.inputs类型为stdin，output.console方式。启动filebeat后，它会从标准输入接收数据，并以JSON格式打印在控制台中。

```
[root@dev filebeat]# cat /etc/filebeat/test_stdin.yml 
filebeat.inputs:       # filebeat输入配置
  - type: stdin        # stdin表示标准输入
    enabled: true      # 用于启用或禁用模块
output.console:        # Console output
  enabled: true        # 用于启用或禁用模块
  codec.json:          # 配置JSON编码
    pretty: true       # Pretty-print JSON event
    escape_html: true  # 在字符串中配置转义HTML symbols



[root@dev filebeat]# filebeat -e -c test_stdin.yml
hello yujiang # 在控制台输入
{
  "@timestamp": "2019-09-17T09:01:00.251Z",
  "@metadata": {				# 元数据
    "beat": "filebeat",
    "type": "doc",
    "version": "6.8.3"
  },
  "offset": 0,
  "log": {
    "file": {
      "path": ""
    }
  },
  "prospector": {				# 标准输入勘探器
    "type": "stdin"
  },
  "input": {					# 控制台标准输入
    "type": "stdin"
  },
  "beat": {						# beat版本以及hostname
    "name": "dev",
    "hostname": "dev",
    "version": "6.8.3"
  },
  "host": {
    "name": "dev"
  },
  "message": "hello yujiang",	# 输入的内容
  "source": ""
}
```

## 日志文件配置及演示

```
[root@dev filebeat]# cat test_log.yml 
filebeat.inputs:       # filebeat输入配置
  - type: log          # log表示收集日志文件
    enabled: true      # 用于启用或禁用模块
    paths:             # 日志文件位置
      - /var/log/yujiang.log
output.console:        # Console output
  enabled: true        # 用于启用或禁用模块
  codec.json:          # 配置JSON编码
    pretty: true       # Pretty-print JSON event
    escape_html: true  # 在字符串中配置转义HTML symbols



# 启动filebeat，这时开启另一个终端，向/var/log/yujiang.log文件中写入hello world
[root@dev filebeat]# filebeat -e -c ./test_log.yml 
2019-09-17T05:27:25.513-0400	INFO	crawler/crawler.go:106	Loading and starting Inputs completed. Enabled inputs: 1
2019-09-17T05:28:55.525-0400	INFO	log/harvester.go:255	Harvester started for file: /var/log/yujiang.log
{
  "@timestamp": "2019-09-17T09:28:55.525Z",
  "@metadata": {
    "beat": "filebeat",
    "type": "doc",
    "version": "6.8.3"
  },
  "beat": {
    "version": "6.8.3",
    "name": "dev",
    "hostname": "dev"
  },
  "host": {
    "name": "dev"
  },
  "offset": 0,
  "log": {
    "file": {
      "path": "/var/log/yujiang.log"
    }
  },
  "message": "hello world",
  "source": "/var/log/yujiang.log",
  "prospector": {
    "type": "log"
  },
  "input": {
    "type": "log"
  }
}



# 向/var/log/yujiang.log文件中写入hello world
[root@dev log]# echo "hello world" >> yujiang.log
```

## 自定义tags配置及演示

```
[root@dev filebeat]# cat test_log_tags.yml 
filebeat.inputs:       # filebeat输入配置
  - type: log          # log表示收集日志文件
    enabled: true      # 用于启用或禁用模块
    paths:             # 日志文件位置
      - /var/log/yujiang.log
    tags: ["web", "ceph"]  # 自定义tags
output.console:        # Console output
  enabled: true        # 用于启用或禁用模块
  codec.json:          # 配置JSON编码
    pretty: true       # Pretty-print JSON event
    escape_html: true  # 在字符串中配置转义HTML symbols



# 启动filebeat，这时开启另一个终端，向/var/log/yujiang.log文件中写入hello tags
[root@dev filebeat]# filebeat -e -c ./test_log_tags.yml 
{
  "@timestamp": "2019-09-17T09:55:43.909Z",
  "@metadata": {
    "beat": "filebeat",
    "type": "doc",
    "version": "6.8.3"
  },
  "source": "/var/log/yujiang.log",
  "offset": 12,
  "beat": {
    "name": "dev",
    "hostname": "dev",
    "version": "6.8.3"
  },
  "message": "hello tags",
  "log": {
    "file": {
      "path": "/var/log/yujiang.log"
    }
  },
  "tags": [      # 自定义tags
    "web",
    "ceph"
  ],
  "prospector": {
    "type": "log"
  },
  "input": {
    "type": "log"
  },
  "host": {
    "name": "dev"
  }
}



# 向/var/log/yujiang.log文件中写入hello world
[root@dev log]# echo "hello tags" >> yujiang.log
```

## 自定义字段配置及演示

from添加到子节点

```
[root@dev filebeat]# cat test_log_tags_fields.yml
filebeat.inputs:       # filebeat输入配置
  - type: log          # log表示收集日志文件
    enabled: true      # 用于启用或禁用模块
    paths:             # 日志文件位置
      - /var/log/yujiang.log
    tags: ["web", "ceph"]  # 添加自定义tags
    fields:            # 添加自定义字段
      from: test-web-ceph
output.console:        # Console output
  enabled: true        # 用于启用或禁用模块
  codec.json:          # 配置JSON编码
    pretty: true       # Pretty-print JSON event
    escape_html: true  # 在字符串中配置转义HTML symbols



[root@dev filebeat]# filebeat -e -c test_log_tags_fields.yml 
{
  "@timestamp": "2019-09-17T10:14:23.323Z",
  "@metadata": {
    "beat": "filebeat",
    "type": "doc",
    "version": "6.8.3"
  },
  "source": "/var/log/yujiang.log",
  "tags": [
    "web",
    "ceph"
  ],
  "input": {
    "type": "log"
  },
  "offset": 23,
  "message": "hello tags fields",
  "prospector": {
    "type": "log"
  },
  "beat": {
    "hostname": "dev",
    "version": "6.8.3",
    "name": "dev"
  },
  "host": {
    "name": "dev"
  },
  "log": {
    "file": {
      "path": "/var/log/yujiang.log"
    }
  },
  "fields": {
    "from": "test-web-ceph"
  }
}



[root@dev log]# echo "hello tags fields" >> yujiang.log
```

from添加到root节点

```
[root@dev filebeat]# cat test_log_tags_fields.yml 
filebeat.inputs:       # filebeat输入配置
  - type: log          # log表示收集日志文件
    enabled: true      # 用于启用或禁用模块
    paths:             # 日志文件位置
      - /var/log/yujiang.log
    tags: ["web", "ceph"]  # 添加自定义tags
    fields:            # 添加自定义字段
      from: test-web-ceph
    fields_under_root: true # true为添加到根节点，false为添加到子节点
output.console:        # Console output
  enabled: true        # 用于启用或禁用模块
  codec.json:          # 配置JSON编码
    pretty: true       # Pretty-print JSON event
    escape_html: true  # 在字符串中配置转义HTML symbols



[root@dev filebeat]# filebeat -e -c test_log_tags_fields.yml
{
  "@timestamp": "2019-09-17T10:25:29.414Z",
  "@metadata": {
    "beat": "filebeat",
    "type": "doc",
    "version": "6.8.3"
  },
  "offset": 77,
  "tags": [
    "web",
    "ceph"
  ],
  "prospector": {
    "type": "log"
  },
  "input": {
    "type": "log"
  },
  "from": "test-web-ceph",    # from添加到root中
  "beat": {
    "version": "6.8.3",
    "name": "dev",
    "hostname": "dev"
  },
  "log": {
    "file": {
      "path": "/var/log/yujiang.log"
    }
  },
  "message": "hello tags fields fields_under_root",
  "source": "/var/log/yujiang.log",
  "host": {
    "name": "dev"
  }
}



[root@dev log]# echo "hello tags fields fields_under_root" >> yujiang.log
```

## 对接elasticsearch配置及演示

日志内容输出到elasticsearch

```
[root@dev filebeat]# cat test_es.yml 
filebeat.inputs:       # filebeat输入配置
  - type: log          # log表示收集日志文件
    enabled: true      # 用于启用或禁用模块
    paths:             # 日志文件位置
      - /var/log/yujiang.log
    tags: ["web", "ceph"]  # 添加自定义tags
    fields:            # 添加自定义字段
      from: test-web-ceph
    fields_under_root: true # true为添加到根节点，false为添加到子节点
setup.template.settings:
  index.number_of_shards: 3 #索引的分区数
output.elasticsearch:
  hosts: ["192.168.56.101:9200"]



[root@dev filebeat]# filebeat -e -c test_es.yml 
2019-09-17T23:10:34.272-0400	INFO	log/harvester.go:255	Harvester started for file: /var/log/yujiang.log
2019-09-17T23:10:35.273-0400	INFO	pipeline/output.go:95	Connecting to backoff(elasticsearch(http://192.168.56.101:9200))
2019-09-17T23:10:35.279-0400	INFO	elasticsearch/client.go:739	Attempting to connect to Elasticsearch version 6.8.3
2019-09-17T23:10:35.379-0400	INFO	template/load.go:128	Template already exists and will not be overwritten.
2019-09-17T23:10:35.379-0400	INFO	instance/beat.go:889	Template successfully loaded.
2019-09-17T23:10:35.380-0400	INFO	pipeline/output.go:105	Connection to backoff(elasticsearch(http://192.168.56.101:9200)) established



[root@dev log]# echo "hello" > yujiang.log
```

## Filebeat工作原理

Filebeat主要由两个组件组成：prospector和harvester。

- harvester

  - 负责读取单个文件的内容

  - 如果文件在读取时被删除或重命名，Filebeat将继续读取文件

- prospector

  - 负责管理harvester并找到所有要读取的文件来源

  - 如果输入类型为日志，则查找器将查找路径匹配的所有文件，并为每个文件启动一个harvester

  - Filebeat目前支持两种prospector类型：log和stdin

- Filebeat如何保持文件的状态

  - Filebeat保存每个文件的状态并将状态刷新到磁盘上的注册文件中

  - 该状态用于记住harvester正在读取的最后偏移量，并确保发送所有日志行

  - 如果输出（例如Elasticsearch或Logstash）无法访问，Filebeat会跟踪最后发送的行，并在输出再次可用时继续读取文件
  - 在Filebeat运行时，每个prospector内存中也会保存文件状态信息，当重新启动Filebeat时，将使用注册文件的数据来重建文件状态，Filebeat将每个harvester在从保存的最后偏移量继续读取
  - 文件状态记录在data/registry文件中

```
一般会保存在registry文件中，记录文件的偏移量
[root@dev filebeat]# cat /var/lib/filebeat/registry 
[{"source":"/var/log/boot.log","offset":0,"timestamp":"2019-09-17T21:50:45.201469161-04:00","ttl":-2,"type":"log","meta":null,"FileStateOS":{"inode":134315203,"device":64768}},{"source":"/var/log/yum.log","offset":9899,"timestamp":"2019-09-17T21:50:45.160502101-04:00","ttl":-2,"type":"log","meta":null,"FileStateOS":{"inode":134315217,"device":64768}},{"source":"/var/log/yujiang.log","offset":12,"timestamp":"2019-09-17T23:15:39.385983011-04:00","ttl":-1,"type":"log","meta":null,"FileStateOS":{"inode":134836485,"device":64768}}]
```

## 启动参数说明

```
[root@dev filebeat]# filebeat -e -c test_es.yml -d "publish"
-e	输出到标准输出，默认输出到syslog和logs下
-c	指定配置文件
-d	输出debug信息
```

## Filebeat Module

Filebeat中，有大量的Module，可以简化我们的配置，可直接使用。在filebeat.yml中必须配置`filebeat.config.modules.path`目录，否则会报错`Error in modules manager: modules management requires 'filebeat.config.modules.path' setting`。

```
[root@dev filebeat]# cat /etc/filebeat/filebeat.yml 
# ......
filebeat.config:
  modules:
    enabled: true
    path: /etc/filebeat/modules.d/*.yml
    reload.enabled: true
    reload.period: 10s
# ......
```

查看模块

```
[root@dev filebeat]# filebeat modules list
Enabled:
system

Disabled:
apache2
auditd
elasticsearch
haproxy
icinga
iis
iptables
kafka
kibana
logstash
mongodb
mysql
nginx
osquery
postgresql
redis
suricata
system
traefik
```

配置filebeat.yml

```
filebeat:

output:
    elasticsearch:
      hosts: ["localhost:9200"]

filebeat.config:
  modules:
    enabled: true
    path: /etc/filebeat/modules.d/*.yml
    reload.enabled: true
    reload.period: 10s
```

