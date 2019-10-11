---
title: elastic stack - logstash
date: 2019-09-18 18:34:36
tags: elk
---

# Logstash

Logstash是开源的服务器端数据处理管道，能够同时从多个来源采集数据，转换数据，然后将数据发送到您最喜欢的“存储库”中。（我们的存储库是Elasticsearch）

![](<https://www.elastic.co/guide/en/logstash/current/static/images/basic_logstash_pipeline.png>)

## Logstash部署安装

```
# 检查jdk环境，要就jdk1.8+
java -version

# 解压安装包
tar zxvf logstash-6.5.4.tar.gz

# 第一个logstash示例
bin/logstash -e 'input { stdin { } } output { stdout { } }'
```

测试

```
[root@dev ~]# /usr/share/logstash/bin/logstash -e 'input { stdin { } } output { stdout { } }'
hello
{
       "message" => "hello",
      "@version" => "1",
    "@timestamp" => 2019-09-19T02:29:59.833Z,
          "host" => "dev"
}
```

## 配置

Logstash的配置有三部分组成：

```
input {	# 输入
    stdin { ... } #标准输入
}

filter { # 过滤，对数据进行分割、截取等处理
    ...
}

output { # 输出
    stdout { ... } #标准输出
}
```

- 输入

  采集各种样式、大小和来源的数据

- 过滤

  实时解析和转换数据

- 输出

  选择您的存储库，导出您的数据

## 读取自定义日志

如果是自定义结构的日志，这个时候就需要Logstash处理后才能使用。

```
# Logstash配置文件，规则定义
[root@dev conf.d]# cat 99-test-yujiang.conf 
input {
    file {
        path => "/var/log/yujiang.log"
        start_position => "beginning"
    }
}
filter {
    mutate {
        split => { "message"=>"|" }
    }
}
output {
    stdout { codec => rubydebug}
}


# 启动Logstash，等待自定义日志文件写入数据
[root@dev conf.d]# /usr/share/logstash/bin/logstash -f /etc/logstash/conf.d/99-test-yujiang.conf
{
      "@version" => "1",
       "message" => [
        [0] "2019-09-19 11:20",
        [1] "ERROR",
        [2] "hello world"
    ],
    "@timestamp" => 2019-09-19T03:20:21.116Z,
          "path" => "/var/log/yujiang.log",
          "host" => "dev"
}



# 向自定义日志文件中写入数据
[root@dev log]# echo "2019-09-19 11:20|ERROR|hello world" >> yujiang.log 
```

将自定义日志写入Elasticsearch

```
# Logstash配置文件，规则定义
[root@dev conf.d]# cat 99-test-yujiang.conf
input {
    file {
        path => "/var/log/yujiang.log"
        start_position => "beginning"
    }
}
filter {
    mutate {
        split => { "message"=>"|" }
    }
}
output {
    elasticsearch {
        hosts => [ "192.168.56.101:9200" ]
    }
}



# 在Elasticsearch-head中查看
{
	"_index": "logstash-2019.09.19",
	"_type": "doc",
	"_id": "ZNuTR20BQ8jxL59AtKFm",
	"_version": 1,
	"_score": 1,
	"_source": {
		"message": [
			"2019-09-19 11:20",
			"ERROR",
			"hello world"
		],
		"@timestamp": "2019-09-19T03:32:04.502Z",
		"@version": "1",
		"path": "/var/log/yujiang.log",
		"host": "dev"
	}
}
```

