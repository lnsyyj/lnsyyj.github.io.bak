---
title: 配置 Golang 开发环境
date: 2018-04-06 23:17:23
tags: Golang
---

# 配置 Golang 开发环境

1、首先根据你所使用的系统，下载对应的Golang包。

国内可以在这里下载：`https://studygolang.com/dl`

2、然后配置Golang的环境变量，配置方法类似于Java环境变量

持久化方法：编辑 ~/.bash_profile 文件，配置自己的目录，例如：

```
GOPATH="/Users/yujiang/go"
GOROOT="/usr/local/go"

# 测试段
export GOPATH="/root/gocodes"
export GOROOT=/usr/local/go
export GOBIN=$GOROOT/bin
export GOPKG=$GOROOT/pkg/tool/linux_amd64 
export GOARCH=amd64
export GOOS=linux
export GOPATH=/Golang
export PATH=$PATH:$GOBIN:$GOPKG:$GOPATH/bin
```

使配置生效
`source ~/.bash_profile`

**主要的环境变量有:**

GOROOT：你所安装的go可执行文件的目录

`GOROOT="/usr/local/go"`

GOPATH：是自己的项目所在目录，也就是所谓的工作目录，可以配置多个目录，使用';'分隔。

GOPATH如果配置多个目录，使用`go install/go get`时，会默认匹配第一个目录，后面的忽略。

`GOPATH="/Users/yujiang/go"`

3、工作目录结构

通常在你的工作目录中，包含3个子目录
```
bin 存放编译后的二进制文件
src 存放源码文件
pkg 存放编译后的库文件*.a
```
