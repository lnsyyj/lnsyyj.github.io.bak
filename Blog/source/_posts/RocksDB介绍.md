---
title: RocksDB介绍
date: 2018-04-12 14:53:12
tags: RocksDB
---

## Welcome to RocksDB

RocksDB：A Persistent Key-Value Store for Flash and RAM Storage

RocksDB是一个嵌入式key-value store C ++库，其中keys和values是任意byte streams。RocksDB由Facebook Database Engineering Team开发和维护，基于LevelDB二次开发，并对LevelDB API提供向后兼容。它支持原子读取和写入。RocksDB借鉴了开源leveldb项目中的重要代码以及Apache HBase的重要思想。最初的代码是从开源的leveldb 1.5中分离出来的。它还建立在Facebook之前在RocksDB开发的代码和想法之上。

RocksDB针对Flash进行了优化，延迟极低。RocksDB使用Log Structured Database Engine进行存储，完全用C ++编写。一个名为RocksJava的Java版本目前正在开发中。

RocksDB具有高度灵活的配置选项，可以调整以运行在各种生产环境上的不同设备之上，包括pure memory，Flash，hard disks或HDFS。它支持各种压缩算法并且为production support和debugging提供良好的工具。

## Features

- 专门希望存储在本地Flash drives或RAM中，高达几TB数据的应用程序服务器而设计
- 针对fast storage设备，存储small到medium size key-values进行了优化( flash devices 或 in-memory )
- 与CPU数量线性扩展，以便在具有多核的处理器上运行良好

 ## Features Not in LevelDB

RocksDB引入了几十个新的主要features。不在LevelDB中的feature[列表](https://github.com/facebook/rocksdb/wiki/Features-Not-in-LevelDB)

## Assumptions and Goals

### Performance

RocksDB设计主要专注于fast storage和server workloads。它应该利用Flash或RAM子系统提供的高速率读/写全部潜力。它应该支持高效的point lookups以及range scans。它应该可配置为支持high random-read workloads，high update workloads或两者的组合（最优解）。其架构应易于调整Read Amplification, Write Amplification 和 Space Amplification。

### Production Support



### Compatibility

