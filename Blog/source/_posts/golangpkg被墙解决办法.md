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

