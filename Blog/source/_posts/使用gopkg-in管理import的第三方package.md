---
title: 使用gopkg.in管理import的第三方package
date: 2018-04-18 14:42:15
tags: Golang
---

在近期项目中，想针对Prometheus的ceph_exporter做定制化修改，但是遇到了一些麻烦。

使用`go get github.com/digitalocean/ceph_exporter` 命令down下来的代码和GitHub上的master分支或tag中的代码都不一样，这是为什么呢？如果`go get`指向目标Repository的master分支，那么master分支怎样保证任何时刻都是可用的呢？于是带着疑问开始寻找答案。

### package管理工具

当你在代码中引用package时，通常使用看起来像URL的导入路径，例如github.com/jpoehls/gophermail。当使用go build构建代码时，Go工具使用此路径在GOPATH中查找这些软件package，如果找不到，则失败。

如何拉取这些package？

```
1、手动下载。你可以使用git clone这些package到你的GOPATH中。
2、使用go get命令( download and install packages and dependencies )。go get只是简单地将导入路径作为URL来对待，并尝试通过HTTP或HTTPS来下载它。它足够聪明，可以处理Git，Mercurial，Bazaar和Subversion。Go对GitHub和Bitbucket等常用的代码管理站点有特殊的支持，同时也支持自定义URL。
```

如何管理GitHub中单个Repository中的多个Versions？

有一种解决方法，可以让你在同一个Repository中，保存软件package的多个版本，并使用branches/tags来区分它们。`go get`支持自定义URL，你可以将版本号插入到package的导入路径中。这意味着需要编写一个代理服务，用于解析URL，并将请求代理到存储Repository的branch/tag上。

幸运的是，有人已经为我们做了这些工作。[GoPkg.in](http://labix.org/gopkg.in)完全符合上述内容。

例如：

使用此方法管理gophermail package。这意味着，不是使用github.com/jpoehls/gophermail导入软件package，而是使用gopkg.in/jpoehls/gophermail.v0。.v0是因为gophermail还没有达到1.0。当我发布1.0并声明稳定的API时，导入路径将更改为gopkg.in/jpoehls/gophermail.v1。

弄到这，好像和下面Prometheus的ceph_exporter输出没太大关系啊。。。。。。。。等我再研究研究。。。。

```
[root@centos7 ~]# go version
go version go1.10 linux/amd64
[root@centos7 ~]# go get -v github.com/digitalocean/ceph_exporter
github.com/digitalocean/ceph_exporter (download)
github.com/digitalocean/ceph_exporter/vendor/github.com/ceph/go-ceph/rados
github.com/digitalocean/ceph_exporter/vendor/github.com/beorn7/perks/quantile
github.com/digitalocean/ceph_exporter/vendor/github.com/golang/protobuf/proto
github.com/digitalocean/ceph_exporter/vendor/github.com/prometheus/client_model/go
github.com/digitalocean/ceph_exporter/vendor/github.com/matttproud/golang_protobuf_extensions/pbutil
github.com/digitalocean/ceph_exporter/vendor/github.com/prometheus/common/internal/bitbucket.org/ww/goautoneg
github.com/digitalocean/ceph_exporter/vendor/github.com/prometheus/common/model
github.com/digitalocean/ceph_exporter/vendor/github.com/prometheus/common/expfmt
github.com/digitalocean/ceph_exporter/vendor/github.com/prometheus/procfs/xfs
github.com/digitalocean/ceph_exporter/vendor/github.com/prometheus/procfs
github.com/digitalocean/ceph_exporter/vendor/github.com/prometheus/client_golang/prometheus
github.com/digitalocean/ceph_exporter/collectors
github.com/digitalocean/ceph_exporter/vendor/github.com/prometheus/client_golang/prometheus/promhttp
github.com/digitalocean/ceph_exporter/vendor/gopkg.in/yaml.v2
```





参考链接：

【1】http://zduck.com/2014/go-and-package-versioning
【2】https://stackoverflow.com/questions/24855081/how-do-i-import-a-specific-version-of-a-package-using-go-get

【3】https://ieevee.com/tech/2017/07/10/go-import.html