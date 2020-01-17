---
title: golangpkg被墙解决办法
date: 2018-07-08 23:26:29
tags:
---

原文：http://www.10tiao.com/html/489/201702/2247483836/1.html

```
$mkdir -p $GOPATH/src/golang.org/x/
$cd $GOPATH/src/golang.org/x/
$git clone https://github.com/golang/net.git net 
$go install net 
或者
$git clone https://github.com/golang/text.git $GOPATH/src/golang.org/x/text
$git clone https://github.com/golang/net.git $GOPATH/src/golang.org/x/net
```

### go get 获取第三方库

- go get命令演示
- 使用gopm来获取无法下载的包

```
yujiangdeMBP-13:~ yujiang$ go get -v github.com/gpmgo/gopm

yujiangdeMBP-13:~ yujiang$ cat ~/.bashrc
export PATH=$PATH:/home/go/bin:/root/go/bin
export GOPATH="/root/go"
yujiangdeMBP-13:~ yujiang$ source ~/.bashrc

yujiangdeMBP-13:~ yujiang$ gopm get -g -v golang.org/x/tools/cmd/goimports
yujiangdeMBP-13:~ yujiang$ ls go/src/golang.org/x/tools/imports/
fix.go         fix_test.go    imports.go     mkindex.go     mkstdlib.go    sortimports.go zstdlib.go
# 编译出goimports
yujiangdeMBP-13:~ yujiang$ go build  golang.org/x/tools/cmd/goimports
# 编译并安装到$GOPATH/bin/目录下
yujiangdeMBP-13:~ yujiang$ go install  golang.org/x/tools/cmd/goimports


go get -v github.com/golang/tools/go/...
go get -v github.com/golang/tools/...

go get -u golang.org/x/tools/...
```

- go build编译
- go install产生pkg文件和可执行文件
- go run直接编译运行