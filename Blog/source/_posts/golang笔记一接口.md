---
title: golang笔记一接口
date: 2018-09-22 19:44:01
tags: golang
---

靠继承和多态能完成的事情，在go语言中是通过接口来完成的。

## duck typing

大黄鸭是鸭子吗？

- 传统类型系统：脊索动物门，脊椎动物门，鸟纲雁形目... ...
- duck typing：是鸭子。

### duck typing

- 像鸭子走路，像鸭子叫（长得像鸭子），那么就是鸭子
- 描述事物的外部行为而非内部结构
- 严格说go属于结构化类型系统，类似duck typing

python中的duck typing

```
def download(retriever):
	return retriever.get("www.imooc.com")

- 运行时才知道传入的retriever有没有get
- 需要注释来说明接口
```

c++中的duck typing

```
template <class R>
string download(const R& retriever) {
    return retriever.get("www.imooc.com")
}

- 编译时才知道传入的retriever有没有get
- 需要注释来说明接口
```

java中的类似代码

```
<R extends Retriever>
String download(R r) {
    return r.get("www.imooc.com")
}

- 传入的参数必须实现Retriever接口
- 不是duck typing
```

go语言的duck typing

```
同时需要Readable，Appendable怎么办？(java有个apache polygene)
同时具有python，c++的duck typing的灵活性
又具有java的类型检查
```

接口的定义

```
 使用者                实现者
download     -->     retriever

go语言的接口由使用者定义(和传统的面向对象思维不同，传统面向对象是实现者告诉大家我实现了某个接口，然后你们可以通过这个接口来用我)
```

demo

```
yujiangdeMacBook-Pro-13:GolangGrammar yujiang$ tree retrieve/
retrieve/
├── main.go
├── mock
│   └── mockretiever.go
└── real
    └── retriever.go

2 directories, 3 files

文件main.go
package main
import (
	"fmt"
	"github.com/lnsyyj/GolangGrammar/retrieve/mock"
	"github.com/lnsyyj/GolangGrammar/retrieve/real"
)
type Retriever interface {
	Get(url string) string
}
func download(r Retriever) string {
	return r.Get("http://www.imooc.com")
}
func main() {
	var r Retriever
	r = mock.Retriever{"This is a fack imooc.com"}
	fmt.Println(download(r))
	r = real.Retriever{}
	fmt.Println(download(r))
}

文件retriever.go
package real
import (
	"time"
	"net/http"
	"net/http/httputil"
)
type Retriever struct {
	UserAgent string
	TimeOut time.Duration
}
func (r Retriever) Get(url string) string {
	resp, err := http.Get(url)
	if err != nil {
		panic(err)
	}
	result, err := httputil.DumpResponse(resp, true)
	resp.Body.Close()
	if err != nil {
		panic(err)
	}
	return string(result)
}

文件mockretiever.go
package mock
type Retriever struct {
	Contents string
}
func (r Retriever) Get(url string) string {
	return r.Contents
}

===================================output===================================
This is a fack imooc.com
HTTP/1.1 200 OK
Accept-Ranges: bytes
Age: 9
Connection: keep-alive
Content-Type: text/html; charset=utf-8
Date: Wed, 10 Oct 2018 14:38:23 GMT
Server: nginx
Vary: Accept-Encoding
Via: 1.1 varnish (Varnish/6.0)
X-Cache: HIT from CS42
X-Varnish: 280162519 281222076
...
...body省略
...
===================================output===================================

看看interface里面有什么？
func main() {
	var r Retriever
	r = mock.Retriever{"This is a fack imooc.com"}
	fmt.Printf("%T %v\n", r, r)
	//fmt.Println(download(r))
	r = real.Retriever{}
	fmt.Printf("%T %v\n", r, r)
	//fmt.Println(download(r))
}
===================================output===================================
mock.Retriever {This is a fack imooc.com}
real.Retriever { 0s}
===================================output===================================
```









