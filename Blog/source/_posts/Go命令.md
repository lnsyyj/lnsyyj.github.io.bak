---
title: Go命令
date: 2018-04-07 13:32:32
tags: Golang
---

# Go命令

Go发行版包含了一个名为go的命令，实现Go包和命令的自动下载，编译，安装和测试。

## 动机

Go从开始就明确目标：能够仅使用源代码本身的信息来构建Go代码，而不需要编写makefile或Makefile。

起初，在没有Go编译器的时候，为了达到自动化构建的目的，Go单独使用一个程序生成makefile。新的go命令的目的使我们回到这个理想。

## Go的惯例

该go命令要求代码遵循几个关键的惯例：
```
1、导入路径源自源码的URL。对于Bitbucket、GitHub、Google Code和Launchpad来说， 其代码仓库的根目录由该仓库的主URL确定，无需 http:// 前缀。 子目录名附加在该路径之后。
例如https://github.com/golang/example这个项目，仓库根目录的导入路径为“github.com/golang/example”。 stringutil 包存储在子目录中，因此它的导入路径为“github.com/golang/example/stringutil”。
2、本地文件系统中存储源码的位置源自导入路径。特别地，首选路径为 $GOPATH/src/<导入路径>。若 $GOPATH 未设置， go命令会回到标准Go包存储源码的地方，即 $GOROOT/src/pkg/<导入路径>。 若 $GOPATH 设置了多个路径，go命令就会尝试这些目录的每一个 <目录>/src/<导入路径>。
3、源码树中的每个目录都对应于单个包。通过将一个目录限定为单个包， 我们无需先指定目录，再从该目录中指定包来创建混合的导入路径了。此外， 大部分文件管理工具和用户界面，都是将目录作为基本工作单元的。 将基本的Go单元—包—同化为文件系统的结构，也就意味着文件系统工具成了Go包的工具。 复制、移动或删除一个包就对应于复制、移动或删除一个目录。
4、包只通过源码中的信息构建。这会让它更容易适应构建环境和条件的改变。 例如，若我们允许额外的配置（如编译器标志或命令行选项等），那么每当构建工具被更改后， 相应的配置也需要更新；它天生还会绑定至特定的工具链。
```

## go 命令基础

首先配置GOPATH，进入此目录后，我们就可以添加一些源码了。假设我们想要使用codesearch项目中的索引包， 以及一个左倾红黑树包。我们可以用“go get”子命令安装二者：
```
$ go get code.google.com/p/codesearch/index
$ go get github.com/petar/GoLLRB/llrb
```

这两个包现在已被下载并安装到我们的 $GOPATH 目录中了。该目录树现在包含了 src/code.google.com/p/codesearch/index/ 和 src/github.com/petar/GoLLRB/llrb/ 这两个目录，以及那些库和依赖的已编译包 （在 pkg/ 中)

"go list"子命令会列出其对应实参的导入路径，而模式"./..." 意为从当前目录（"./"）开始，查找该目录下的所有包（"..."）：

```
yujiangdeMBP-13:go yujiang$ go list ./...
github.com/prometheus/prometheus/cmd/prometheus
github.com/prometheus/prometheus/cmd/promtool
```

我们也可以测试这些包:

```
go test ./...
```

"go install"子命令会将包的最新副本安装到pkg目录中。由于 go 命令会分析依赖关系，因此，除非该包导入的其它包已过期，否则 "go install" 也会递归地根据依赖关系安装它们。

### go get

可以根据要求和实际情况从互联网上下载或更新指定的代码包及其依赖包，并对它们进行编译和安装。

```
[root@centos7 ~]# go help get
usage: go get [-d] [-f] [-fix] [-insecure] [-t] [-u] [-v] [build flags] [packages]
```

-d标志指示在下载软件包后停止，不安装软件包。

-f标志仅在设置-u时有效，该标记会让命令程序忽略掉对已下载代码包的导入路径的检查。如果下载并安装的代码包所属的项目是你从别人那里Fork过来的，那么这样做就尤为重要了。

-fix标志指示命令程序在下载代码包后先执行修正动作，而后再进行编译和安装。

-insecure标志允许命令程序使用非安全的scheme（如HTTP）去下载指定的代码包。如果你用的代码仓库（如公司内部的Gitlab）没有HTTPS支持，可以添加此标记。请在确定安全的情况下使用它。

-t标志指示让命令程序同时下载并安装指定的代码包中的测试源码文件中依赖的代码包。

-u标志指示让命令利用网络来更新已有代码包及其依赖包。默认情况下，该命令只会从网络上下载本地不存在的代码包，而不会更新已有的代码包。

-v标志启用详细的进度和调试输出。

-x标志可以看到`go get`命令执行过程中所使用的所有命令

Get也接受build flags来控制安装。请参阅`go help build`。

当检出一个新的包时，get创建目标目录`GOPATH/src/<import-path>`。如果GOPATH包含多个条目，get使用第一个条目。欲了解更多详情，请参阅：`go help gopath`。

在checkout或update软件包时，请查找与本地安装的Go版本相匹配的branch或tag。最重要的规则是，如果本地安装运行版本“go1”，则搜索名为“go1”的branch或tag。如果不存在这样的版本，它将检索软件包的默认branch。

当去checkout或update git repository时，它还会更新repository引用的任何git子模块。

永远不要checksout或update存储在vendor目录中的代码。

有关指定软件包的更多信息，请参阅`go help packages`。

有关`go get`如何查找要下载的源代码的更多信息，请参阅`go help importpath`。

另请参见：`go build`，`go install`，`go clean`。

### 参考资料

【1】http://wiki.jikexueyuan.com/project/go-command-tutorial/0.3.html