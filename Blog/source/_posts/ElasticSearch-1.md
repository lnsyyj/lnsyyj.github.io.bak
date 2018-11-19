---
title: ElasticSearch(1)
date: 2017-09-17 12:46:13
tags: ES
---

ElasticSearch入门
笔记来自：http://www.imooc.com/video/15762
## ElasticSearch简介 ##
1、什么是ElasticSearch？

	基于Apache Lucene构建的开源搜索引擎
	采用Java编写，提供简单易用的RESTFul API
	轻松的横向扩展，可支持PB级的结构化或非结构化数据处理

2、应用场景

	海量数据分析引擎
	站内搜索引擎
	数据仓库

3、一线公司实际应用场景

	英国卫报 - 实时分析公众对文章的回应
	维基百科、GitHub - 站内实时搜索
	百度 - 实时日志监控平台
	等等...

4、前置知识

	熟悉用Maven构建项目
	了解Spiring Boot的基本使用

5、环境要求

	IDE工具 IntelliJ IDEA、Eclipse等常规用IDE即可
	Java JDK1.8
	其他依赖 Maven、NodeJs（6.0以上）

6、课程简介

	安装
	基础概念
	基本用法
	高级查询
	实战演练
	课程总结

## 安装 ##

1、版本问题

	版本历史 1.x->2.x->5.x
	版本选择 5.x

2、单实例安装
（1）官网下载elasticsearch

	https://www.elastic.co/cn/

	yujiang@ubuntu001:~$ wget https://artifacts.elastic.co/downloads/elasticsearch/elasticsearch-5.6.0.tar.gz


（2）解压并修改配置文件

	yujiang@ubuntu001:~$ tar zxvf elasticsearch-5.6.0.tar.gz

	yujiang@ubuntu001:~$ cd elasticsearch-5.6.0/

	yujiang@ubuntu001:~/elasticsearch-5.6.0$ vi config/elasticsearch.yml
	添加
	network.host: 192.168.30.134


（3）启动elasticsearch


	yujiang@ubuntu001:~$ sudo sysctl -w vm.max_map_count=262144
	或
	yujiang@ubuntu001:~$ sudo vi /etc/sysctl.conf
	vm.max_map_count=262144

	yujiang@ubuntu001:~/elasticsearch-5.6.0$ sh ./bin/elasticsearch


（4）打开浏览器

	http://192.168.30.134:9200/

3、实用插件Head安装
（1）安装nodejs

	yujiang@ubuntu001:~$ wget https://nodejs.org/dist/v6.11.3/node-v6.11.3-linux-x64.tar.xz
	yujiang@ubuntu001:~$ xz -d node-v6.11.3-linux-x64.tar.xz 
	yujiang@ubuntu001:~$ tar -xvf node-v6.11.3-linux-x64.tar 
	yujiang@ubuntu001:~$ sudo ln -s /home/yujiang/node-v6.11.3-linux-x64/bin/node  /usr/bin/node 
	yujiang@ubuntu001:~$ sudo ln -s /home/yujiang/node-v6.11.3-linux-x64/bin/npm  /usr/bin/npm 

（2）修改ElasticSearch配置文件，支持跨域，并在后台运行ElasticSearch

	yujiang@ubuntu001:~/elasticsearch-5.6.0$ vi config/elasticsearch.yml
	添加
	http.cors.enabled: true
	http.cors.allow-origin: "*"
	yujiang@ubuntu001:~/elasticsearch-5.6.0$ sh ./bin/elasticsearch -d

（3）运行elasticsearch-head

	https://github.com/mobz/elasticsearch-head
	yujiang@ubuntu001:~$ wget https://github.com/mobz/elasticsearch-head/archive/master.zip
	yujiang@ubuntu001:~$ unzip master.zip
	yujiang@ubuntu001:~$ cd elasticsearch-head-master/
	yujiang@ubuntu001:~/elasticsearch-head-master$ npm install
	yujiang@ubuntu001:~/elasticsearch-head-master$ npm run start

（4）打开浏览器

	http://192.168.30.134:9100/

	web页面输入
	http://192.168.30.134:9200/

4、分布式安装

（1）master节点修改配置文件

	yujiang@ubuntu001:~/elasticsearch-5.6.0$ vi config/elasticsearch.yml
	添加 
	cluster.name: escluster
	node.name: master
	node.master: true
（2）slave节点修改配置文件
	
	cluster.name: escluster
	node.name: slave1
	network.host: 192.168.30.135
	#http.port: 8200
	discovery.zen.ping.unicast.hosts: ["192.168.30.134"]
	yujiang@ubuntu002:~/elasticsearch-5.6.0$ sh ./bin/elasticsearch

（3）安装java jdk

	yujiang@ubuntu002:~$ sudo add-apt-repository ppa:openjdk-r/ppa
	yujiang@ubuntu002:~$ sudo apt-get update 
	yujiang@ubuntu002:~$ sudo apt-get install -f openjdk-8-jdk
	yujiang@ubuntu002:~$ java -version
	openjdk version "1.8.0_131"
	OpenJDK Runtime Environment (build 1.8.0_131-8u131-b11-2ubuntu1.16.04.3-b11)
	OpenJDK 64-Bit Server VM (build 25.131-b11, mixed mode)

## ElasticSearch基础概念 ##
集群和节点

	master + slave1 + slave2 + slave...n
索引

	含有相同属性的文档集合
类型

	索引可以定义一个或多个类型，文档必须属于一个类型
文档

	文档是可以被索引的基本数据单位
分片

	每个索引都有多个分片，每个分片是一个Lucene索引
	分片的好处，加入一个索引的数据量很大，就会造成硬盘压力很大，就会出现搜索瓶颈，可以将索引分成多个分片，从而分担压力，还允许用户进行水平的扩展和拆分，及分布式的操作，可以提高搜索和其他操作的效率
备份
	
	拷贝一份分片就完成了分片的备份
	当一个主分片失败或者出现问题时，备份分片可以代替工作，从而提高了es的可用性。备份的分片还可以执行搜索的操作，以分摊搜索的压力。es在创建索引时会创建5和分片和1个备份，数量可以修改，分片只有在创建索引的时候指定而不能在后期进行修改，备份是可以动态修改的

## ElasticSearch的基本用法 ##
RESTFul API

（1）API基本格式

	http://<ip>:<port>/<索引>/<类型>/<文档id>
（2）常用HTTP动词

	GET/PUT/POST/DELETE
