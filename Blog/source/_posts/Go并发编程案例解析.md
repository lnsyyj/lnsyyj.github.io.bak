---
title: Go并发编程案例解析
date: 2018-05-01 14:24:32
tags: Golang
---

Go并发编程案例解析学习笔记

课程地址：https://www.imooc.com/video/17021

课程教师：[麦可同学](https://www.imooc.com/u/6498986/courses?sort=publish)

## 课程介绍

- 并发编程基础知识介绍
- 日志监控系统实战
- 课程总结


### 准备知识

- 有一定的编程基础


- 了解Golang基本语法
- 有并发编程经验就更好了

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/source/_posts/Go并发编程案例解析/WX20180501-143535%402x.png)

## 常见并发模型讲解

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/source/_posts/Go并发编程案例解析/WX20180501-144809@2x.png)

### Golang并发实现

- 程序并发执行（goroutine）
- 多个goroutine间的数据同步与通信（channels）
- 多个channels选择数据读取或者写入（select）

### Goroutines（程序并发执行）

```
foo()		//执行函数foo，程序等待函数foo返回

go foo()	//执行函数foo
bar()		//不用等待foo返回
```

### Channels（多个goroutines间的数据同步与通信）

```
c := make(chan string)	//创建一个channel
go func(){
    time.Sleep(1 * time.Second)
    c <- "message from closure"	//发送数据到channel中
}()	//这个()表示调用该函数
msg := <-c	//阻塞直到接收到数据
```

### Select（从多个channel中读取或写入数据）

```
select {
    case v := <-c1:
    	fmt.Println("channel 1 sends", v)
    case v := <-c2:
    	fmt.Println("channel 2 sends", v")
    default:	//可选
    	fmt.Println("neither channel was ready")
}
```

## 并发拓展：并发与并行

定义：

- 并发：指同一时刻，系统通过调度，来回切换交替的运行多个任务，“看起来”是同时进行
- 并行：指同一时刻，两个任务“真正的”同时进行

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/source/_posts/Go并发编程案例解析/WX20180501-151148@2x.png)

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/source/_posts/Go并发编程案例解析/WX20180501-151435@2x.png)

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/source/_posts/Go并发编程案例解析/WX20180501-152335@2x.png)

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/source/_posts/Go并发编程案例解析/WX20180501-152356@2x.png)

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/source/_posts/Go并发编程案例解析/WX20180501-152503@2x.png)

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/source/_posts/Go并发编程案例解析/WX20180501-152603@2x.png)

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/source/_posts/Go并发编程案例解析/WX20180501-152657@2x.png)

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/source/_posts/Go并发编程案例解析/WX20180501-152740@2x.png)

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/source/_posts/Go并发编程案例解析/WX20180501-152814@2x.png)

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/source/_posts/Go并发编程案例解析/WX20180501-152904@2x.png)

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/source/_posts/Go并发编程案例解析/WX20180501-143535@2x.png)

可以把《实时读取》《解析》《写入》拆成多个模块，使用多个goroutine。那么这么多个goroutine是并行执行还是并发执行呢？换句话说，多个goroutine执行，是一个CPU核心通过不断的切换时间片，并发的执行？还是将goroutine分散到多核的CPU并行的执行？这个问题Golang为我们屏蔽掉了，编程人员不需要考虑这个问题。

### 总结

- 将复杂的任务拆分，通过goroutine去并发执行
- 通过channel做数据的同步与通信

## Golang中的面向对象

Golang中没有类和对象的概念，但是支持

- struct
- interface

传统的面向对象中，继承、封装、多态都可以基于这两个特性来实现。

### 封装

```go
// 结构体，baz相当于成员变量，可以看做是类
type Foo struct {
    baz string
}

// 接收者Receiver，接收之后，就可以使用结构体中的字段了，相当于成员函数
func (f *Foo) echo(){
    fmt.Println(f.baz)
}

// 在main中初始化结构体，相当于实例化一个类，然后调用成员方法
func main(){
    f := Foo{baz: "hello, struct"}
    f.echo()
}
```

### 继承（个人认为这是设计模式中的组合）

```go
// Foo结构体
type Foo struct {
    baz string
}

// 接收者Receiver，Foo成员函数
func (f *Foo) echo() {
    fmt.Println(f.baz)
}

// 在Bar结构体中直接把Foo结构体写进来，这个叫匿名字段。这样写之后，Bar结构体就拥有了Foo结构体的所有特性
type Bar struct {
    Foo
}

// 在main函数中初始化Bar结构体，然后直接调用echo()方法，echo()方法其实是Foo中的成员函数
func main() {
    b := Bar{Foo{baz: "hello, struct"}}
    b.echo()
}
```

### 多态

```go
// 定义一个interface，可以看成是一组方法的集合，通过interface定义一些对象的行为
type Foo interface {
    qux()
}

// 定义Bar和Baz两个结构体
type Bar struct {}
type Baz struct {}

// Bar和Baz两个结构体分别实现了qux()函数，这样就可以说Bar和Baz都是Foo这种类型了。这里并没有显示的说我实现了Foo这个接口，只要结构体中实现了qux()这个函数，就认为它实现了这个接口，这就是所谓的非侵入式接口
func (b Bar) qux() {}
func (b Baz) qux() {}

// 在main中，定义了f的变量，它的类型是Foo。无论是Bar还是Baz都可以赋值给f这个变量，这也证明了Bar{}和Baz{}是同一类型。可以说类型相同，实现不同
func main() {
    var f Foo
    f = Bar{}
    f = Baz{}
    fmt.Println(f)
}
```

## 日志监控程序的实现

### 日志分析系统实战

```go
package main

import (
	"strings"
	"fmt"
	"time"
)

type LogProcess struct {
	rc chan string
	wc chan string
	path string			// 读取文件的路径
	influxDBDsn	string	// influx data source
}

func (l *LogProcess) ReadFromFile() {	// 需要代码优化，只能从文件中读取数据
	// 读取模块
	line := "message"
	l.rc <- line
}

func (l *LogProcess) Process() {
	// 解析模块
	data := <- l.rc
	l.wc <- strings.ToUpper(data)

}

func (l *LogProcess) WriteToInfluxDB() {	// 需要代码优化，只能向influxDB中写入数据
	// 写入模块
	fmt.Println(<-l.wc)
}

func main() {
	lp := LogProcess{
		rc : make(chan string),
		wc : make(chan string),
		path: "/tmp/access.log",
		influxDBDsn: "",
	}
	go lp.ReadFromFile()
	go lp.Process()
	go lp.WriteToInfluxDB()
	time.Sleep(time.Second)
}
```

### 代码优化

```go
package main

import (
	"strings"
	"fmt"
	"time"
)

// 定义接口，抽象读取模块
type Reader interface {
	Read(rc chan string)
}

type ReadFromFile struct {
	path string			// 读取文件的路径
}

func (l *ReadFromFile) Read(rc chan string) {
	// 读取模块
	line := "message"
	rc <- line
}

// 定义接口，抽象写入模块
type Writer interface {
	Write(wc chan string)
}

type WriteToInfluxDB struct {
	influxDBDsn	string	//	influx data source
}

func (l *WriteToInfluxDB) Write(wc chan string) {
	// 写入模块
	fmt.Println(<-wc)
}

type LogProcess struct {
	rc chan string
	wc chan string
	read Reader
	write Writer
}

func (l *LogProcess) Process() {
	// 解析模块
	data := <- l.rc
	l.wc <- strings.ToUpper(data)
}

func main() {

	r := &ReadFromFile{
		path: "/tmp/access.log",
	}

	w := &WriteToInfluxDB{
		influxDBDsn: "",
	}

	lp := LogProcess{
		rc : make(chan string),
		wc : make(chan string),
		read: r,
		write: w,
	}
    
	go lp.read.Read(lp.rc)
	go lp.Process()
	go lp.write.Write(lp.wc)
	time.Sleep(time.Second)
}
```

### 读取模块实现

```go
package main

import (
	"strings"
	"fmt"
	"time"
	"os"
	"bufio"
	"io"
)

// 定义两个接口，抽象读取模块和写入模块
type Reader interface {
	Read(rc chan []byte)
}

type ReadFromFile struct {
	path string			// 读取文件的路径
}

func (l *ReadFromFile) Read(rc chan []byte) {
	// 读取模块

	// 1. 打开文件
	f,err := os.Open(l.path)
	if err != nil {
		panic(fmt.Sprintf("open file error : %s \n", err))
	}

	// 2. 从文件末尾开始逐行读取文件内容
	f.Seek(0,2)
	rd := bufio.NewReader(f)

	// 3. 写入到Read Channel中
	for {
		line, err := rd.ReadBytes('\n')
		if err == io.EOF {
			time.Sleep(500 * time.Millisecond)
			continue
		} else if err != nil {
			panic(fmt.Sprintf("ReadBytes error : %s", err.Error()))
		}
		rc <- line[:len(line) - 1]
	}
}

type Writer interface {
	Write(wc chan string)
}

type WriteToInfluxDB struct {
	influxDBDsn	string	//	influx data source
}

func (l *WriteToInfluxDB) Write(wc chan string) {
	// 写入模块
	for v := range wc{
		fmt.Println(v)
	}
}

type LogProcess struct {
	rc chan []byte
	wc chan string
	read Reader
	write Writer
}

func (l *LogProcess) Process() {
	// 解析模块
	for v := range l.rc {
		l.wc <- strings.ToUpper(string(v))
	}
}

func main() {

	r := &ReadFromFile{
		path: "/tmp/access.log",
	}

	w := &WriteToInfluxDB{
		influxDBDsn: "",
	}

	lp := LogProcess{
		rc : make(chan []byte),
		wc : make(chan string),
		read: r,
		write: w,
	}

	go lp.read.Read(lp.rc)
	go lp.Process()
	go lp.write.Write(lp.wc)

	time.Sleep(60 * time.Second)
}
```

### 解析模块的实现

```go
package main

import (
	"strings"
	"fmt"
	"time"
	"os"
	"bufio"
	"io"
	"regexp"
	"log"
	"strconv"
	"net/url"
)

// 定义两个接口，抽象读取模块和写入模块
type Reader interface {
	Read(rc chan []byte)
}

type ReadFromFile struct {
	path string			// 读取文件的路径
}

func (l *ReadFromFile) Read(rc chan []byte) {
	// 读取模块

	// 1. 打开文件
	f,err := os.Open(l.path)
	if err != nil {
		panic(fmt.Sprintf("open file error : %s \n", err))
	}

	// 2. 从文件末尾开始逐行读取文件内容
	f.Seek(0,2)
	rd := bufio.NewReader(f)

	// 3. 写入到Read Channel中
	for {
		line, err := rd.ReadBytes('\n')
		if err == io.EOF {
			time.Sleep(500 * time.Millisecond)
			continue
		} else if err != nil {
			panic(fmt.Sprintf("ReadBytes error : %s", err.Error()))
		}
		rc <- line[:len(line) - 1]
	}
}

type Writer interface {
	Write(wc chan *Message)
}

type WriteToInfluxDB struct {
	influxDBDsn	string	//	influx data source
}

func (l *WriteToInfluxDB) Write(wc chan *Message) {
	// 写入模块
	for v := range wc{
		fmt.Println(v)
	}
}

type LogProcess struct {
	rc chan []byte
	wc chan *Message
	read Reader
	write Writer
}

type Message struct {
	TimeLocal	time.Time
	ByteSent	int
	Path, Method, Scheme, Status	string
	UpstreamTime, RequestTime	float64
}

func (l *LogProcess) Process() {
	// 解析模块
	// 172.0.0.12 - - [04/Mar/2018:13:49:52 +0000] http "GET /foo?query=t HTTP:1.0" 200 2133 "-" "KeepAliveClient" "-" 1.005 1.854

	r := regexp.MustCompile(`([\d\.]+)\s+([^ \[]+)\s+([^ \[]+)\s+\[([^\]]+)\]\s+([a-z]+)\s+\"([^"]+)\"\s+(\d{3})\s+(\d+)\s+\"([^"]+)\"\s+\"(.*?)\"\s+\"([\d\.-]+)\"\s+([\d\.-]+)\s+([\d\.-]+)`)
	loc , _ := time.LoadLocation("Asia/Shanghai")

	// 1. 从Read Channel中读取每行日志数据
	for v := range l.rc {

		// 2. 正则提取所需的监控数据（path，status，method等）
		ret := r.FindStringSubmatch(string(v))
		if len(ret) != 14 {
			log.Println("FindStringSubmatch failed : ", string(v))
			continue
		}

		message := &Message{}
		t, err := time.ParseInLocation("02/Jan/2006:15:04:05 +0000", ret[4], loc)
		if err != nil {
			log.Println("ParseInLocation failed : ", err.Error(), ret[4])
		}
		message.TimeLocal = t

		byteSent, _ := strconv.Atoi(ret[8])
		message.ByteSent = byteSent

		reqSli := strings.Split(ret[6]," ")
		if len(reqSli) != 3 {
			log.Println("string.Split failed : ", ret[6])
			continue
		}
		message.Method = reqSli[0]
		u, err := url.Parse(reqSli[1])
		if err != nil {
			log.Println("url parse failed : ", err)
			continue
		}
		message.Path = u.Path

		message.Scheme = ret[5]
		message.Status = ret[7]

		upstreamTime, err := strconv.ParseFloat(ret[12],64)
		message.UpstreamTime = upstreamTime

		requestTime, err := strconv.ParseFloat(ret[13], 64)
		message.RequestTime = requestTime

		// 3. 写入Write Channel
		l.wc <- message
	}
}

func main() {

	r := &ReadFromFile{
		path: "/tmp/access.log",
	}

	w := &WriteToInfluxDB{
		influxDBDsn: "",
	}

	lp := LogProcess{
		rc : make(chan []byte),
		wc : make(chan *Message),
		read: r,
		write: w,
	}

	go lp.read.Read(lp.rc)
	go lp.Process()
	go lp.write.Write(lp.wc)

	time.Sleep(60 * time.Second)
}
```

### 写入模块流程讲解

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/source/_posts/Go并发编程案例解析/WX20180504-145716@2x.png)

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/source/_posts/Go并发编程案例解析/WX20180504-145859@2x.png)

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/source/_posts/Go并发编程案例解析/WX20180504-150125@2x.png)

在URL中，`db=mydb`指定database。使用逗号做分隔','，`cpu_usage`表示measurement。`host=server01,region=us-west value=0.64 1434055562000000000`表示points。`host=server01,region=us-west`表示points中的tags，`value=0.64`表示points中的fields，`1434055562000000000`表示points中的time。

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/source/_posts/Go并发编程案例解析/WX20180504-160707@2x.png)

influxDB提供了Golang的客户端，可以使用这个客户端很方便的写入数据`https://github.com/influxdata/influxdb/tree/master/client`，首先先引入包`"github.com/influxdata/influxdb/client/v2"`

```go
package main

import (
	"strings"
	"fmt"
	"time"
	"os"
	"bufio"
	"io"
	"regexp"
	"log"
	"strconv"
	"net/url"

	"github.com/influxdata/influxdb/client/v2"
	"flag"
)

// 定义两个接口，抽象读取模块和写入模块
type Reader interface {
	Read(rc chan []byte)
}

type ReadFromFile struct {
	path string			// 读取文件的路径
}

func (l *ReadFromFile) Read(rc chan []byte) {
	// 读取模块

	// 1. 打开文件
	f,err := os.Open(l.path)
	if err != nil {
		panic(fmt.Sprintf("open file error : %s \n", err))
	}

	// 2. 从文件末尾开始逐行读取文件内容
	f.Seek(0,2)
	rd := bufio.NewReader(f)

	// 3. 写入到Read Channel中
	for {
		line, err := rd.ReadBytes('\n')
		if err == io.EOF {
			time.Sleep(500 * time.Millisecond)
			continue
		} else if err != nil {
			panic(fmt.Sprintf("ReadBytes error : %s", err.Error()))
		}
		rc <- line[:len(line) - 1]
	}
}

type Writer interface {
	Write(wc chan *Message)
}

type WriteToInfluxDB struct {
	influxDBDsn	string	//	influx data source
}

func (l *WriteToInfluxDB) Write(wc chan *Message) {
	// 写入模块
	// 1.初始化influxDB client
	// 2.从Write Channel中读取监控数据
	// 3.构造数据并写入influxDB

	infSli := strings.Split(l.influxDBDsn, "@")

	// Create a new HTTPClient
	c, err := client.NewHTTPClient(client.HTTPConfig{
		Addr:     infSli[0],
		Username: infSli[1],
		Password: infSli[2],
	})
	if err != nil {
		log.Fatal(err)
	}


	for v := range wc{
		fmt.Println(v)
		defer c.Close()

		// Create a new point batch
		bp, err := client.NewBatchPoints(client.BatchPointsConfig{
			Database:  infSli[3],
			Precision: infSli[4],
		})
		if err != nil {
			log.Fatal(err)
		}

		// Create a point and add to batch
		tags := map[string]string{"Path": v.Path, "Method": v.Method, "Scheme": v.Scheme, "Status": v.Status}
		fields := map[string]interface{}{
			"UpstreamTime":   v.UpstreamTime,
			"RequestTime": v.RequestTime,
			"BytesSent":   v.ByteSent,
		}

		pt, err := client.NewPoint("nginx_log", tags, fields, v.TimeLocal)
		if err != nil {
			log.Fatal(err)
		}
		bp.AddPoint(pt)

		// Write the batch
		if err := c.Write(bp); err != nil {
			log.Fatal(err)
		}

		// Close client resources
		if err := c.Close(); err != nil {
			log.Fatal(err)
		}
		fmt.Println("write success")
	}
}

type LogProcess struct {
	rc chan []byte
	wc chan *Message
	read Reader
	write Writer
}

type Message struct {
	TimeLocal	time.Time
	ByteSent	int
	Path, Method, Scheme, Status	string
	UpstreamTime, RequestTime	float64
}

func (l *LogProcess) Process() {
	// 解析模块
	// 172.0.0.12 - - [04/Mar/2018:13:49:52 +0000] http "GET /foo?query=t HTTP:1.0" 200 2133 "-" "KeepAliveClient" "-" 1.005 1.854

	r := regexp.MustCompile(`([\d\.]+)\s+([^ \[]+)\s+([^ \[]+)\s+\[([^\]]+)\]\s+([a-z]+)\s+\"([^"]+)\"\s+(\d{3})\s+(\d+)\s+\"([^"]+)\"\s+\"(.*?)\"\s+\"([\d\.-]+)\"\s+([\d\.-]+)\s+([\d\.-]+)`)
	loc , _ := time.LoadLocation("Asia/Shanghai")

	// 1. 从Read Channel中读取每行日志数据
	for v := range l.rc {

		// 2. 正则提取所需的监控数据（path，status，method等）
		ret := r.FindStringSubmatch(string(v))
		if len(ret) != 14 {
			log.Println("FindStringSubmatch failed : ", string(v))
			continue
		}

		message := &Message{}
		t, err := time.ParseInLocation("02/Jan/2006:15:04:05 +0000", ret[4], loc)
		if err != nil {
			log.Println("ParseInLocation failed : ", err.Error(), ret[4])
		}
		message.TimeLocal = t

		byteSent, _ := strconv.Atoi(ret[8])
		message.ByteSent = byteSent

		reqSli := strings.Split(ret[6]," ")
		if len(reqSli) != 3 {
			log.Println("string.Split failed : ", ret[6])
			continue
		}
		message.Method = reqSli[0]
		u, err := url.Parse(reqSli[1])
		if err != nil {
			log.Println("url parse failed : ", err)
			continue
		}
		message.Path = u.Path

		message.Scheme = ret[5]
		message.Status = ret[7]

		upstreamTime, err := strconv.ParseFloat(ret[12],64)
		message.UpstreamTime = upstreamTime

		requestTime, err := strconv.ParseFloat(ret[13], 64)
		message.RequestTime = requestTime

		// 3. 写入Write Channel
		l.wc <- message
	}
}

func main() {

	var path, influxDsn string
	flag.StringVar(&path, "path", "/tmp/access.log", "read file path")
	flag.StringVar(&influxDsn, "influxDsn", "http://127.0.0.1:8086@imooc@imoocpass@nginx@s", "influx data source")
	flag.Parse()

	r := &ReadFromFile{
		path: path,
	}

	w := &WriteToInfluxDB{
		influxDBDsn: influxDsn,
	}

	lp := LogProcess{
		rc : make(chan []byte),
		wc : make(chan *Message),
		read: r,
		write: w,
	}

	go lp.read.Read(lp.rc)
	go lp.Process()
	go lp.write.Write(lp.wc)

	time.Sleep(6000 * time.Second)
}
```

### 监控模块的实现

1、总处理日志行数

2、系统吞吐量

3、read channel长度

4、write channel长度

5、运行总时间

6、错误数

```shell
package main

import (
	"strings"
	"fmt"
	"time"
	"os"
	"bufio"
	"io"
	"regexp"
	"log"
	"strconv"
	"net/url"

	"github.com/influxdata/influxdb/client/v2"
	"flag"
	"net/http"
	"encoding/json"
)

// 定义两个接口，抽象读取模块和写入模块
type Reader interface {
	Read(rc chan []byte)
}

type ReadFromFile struct {
	path string			// 读取文件的路径
}

func (l *ReadFromFile) Read(rc chan []byte) {
	// 读取模块

	// 1. 打开文件
	f,err := os.Open(l.path)
	if err != nil {
		panic(fmt.Sprintf("open file error : %s \n", err))
	}

	// 2. 从文件末尾开始逐行读取文件内容
	f.Seek(0,2)
	rd := bufio.NewReader(f)

	// 3. 写入到Read Channel中
	for {
		line, err := rd.ReadBytes('\n')
		if err == io.EOF {
			time.Sleep(500 * time.Millisecond)
			continue
		} else if err != nil {
			panic(fmt.Sprintf("ReadBytes error : %s", err.Error()))
		}
		TypeMonitorChan <- TypeHandleLine
		rc <- line[:len(line) - 1]
	}
}

type Writer interface {
	Write(wc chan *Message)
}

type WriteToInfluxDB struct {
	influxDBDsn	string	//	influx data source
}

func (l *WriteToInfluxDB) Write(wc chan *Message) {
	// 写入模块
	// 1.初始化influxDB client
	// 2.从Write Channel中读取监控数据
	// 3.构造数据并写入influxDB

	infSli := strings.Split(l.influxDBDsn, "@")

	// Create a new HTTPClient
	c, err := client.NewHTTPClient(client.HTTPConfig{
		Addr:     infSli[0],
		Username: infSli[1],
		Password: infSli[2],
	})
	if err != nil {
		log.Fatal(err)
	}


	for v := range wc{
		fmt.Println(v)
		defer c.Close()

		// Create a new point batch
		bp, err := client.NewBatchPoints(client.BatchPointsConfig{
			Database:  infSli[3],
			Precision: infSli[4],
		})
		if err != nil {
			log.Fatal(err)
		}

		// Create a point and add to batch
		tags := map[string]string{"Path": v.Path, "Method": v.Method, "Scheme": v.Scheme, "Status": v.Status}
		fields := map[string]interface{}{
			"UpstreamTime":   v.UpstreamTime,
			"RequestTime": v.RequestTime,
			"BytesSent":   v.ByteSent,
		}

		pt, err := client.NewPoint("nginx_log", tags, fields, v.TimeLocal)
		if err != nil {
			log.Fatal(err)
		}
		bp.AddPoint(pt)

		// Write the batch
		if err := c.Write(bp); err != nil {
			log.Fatal(err)
		}

		// Close client resources
		if err := c.Close(); err != nil {
			log.Fatal(err)
		}
		fmt.Println("write success")
	}
}

type LogProcess struct {
	rc chan []byte
	wc chan *Message
	read Reader
	write Writer
}

type Message struct {
	TimeLocal	time.Time
	ByteSent	int
	Path, Method, Scheme, Status	string
	UpstreamTime, RequestTime	float64
}

//	系统状态监控
type SystemInfo struct {
	HandleLine 		int 		`json:"HandleLine"`		//	总处理日志行数
	Tps 			float64		`json:"tps"`			//	系统吞吐量
	ReadChanLen		int 		`json:"ReadChanLen"`	//	read channel 长度
	WriteChanLen 	int			`json:"WriteChanLen"`	//	write channel 长度
	RunTime			string		`json:"RunTime"`		//	运行总时间
	ErrNum			int			`json:"ErrNum"`			//	错误数
}

const (
	TypeHandleLine = 0
	TypeErrNum = 1
)
var TypeMonitorChan = make(chan int, 200)

type Monitor struct {
	startTime time.Time
	data SystemInfo
	tpsSli	[]int
}

func (m *Monitor) start(lp *LogProcess) {
	// 消费数据
	go func() {
		for n := range TypeMonitorChan {
			switch n {
			case TypeErrNum:
				m.data.ErrNum += 1
			case TypeHandleLine:
				m.data.HandleLine += 1
			}
		}
	}()

	ticker := time.NewTicker(time.Second * 5)
	go func() {
		for {
			<-ticker.C
			m.tpsSli = append(m.tpsSli, m.data.HandleLine)
			if len(m.tpsSli) > 2 {
				m.tpsSli = m.tpsSli[1:]
			}
		}
	}()

	http.HandleFunc("/monitor", func(writer http.ResponseWriter, request *http.Request) {
		m.data.RunTime = time.Now().Sub(m.startTime).String()
		m.data.WriteChanLen = len(lp.wc)
		m.data.ReadChanLen = len(lp.rc)

		if len(m.tpsSli) >= 2 {
			m.data.Tps = float64(m.tpsSli[1] - m.tpsSli[0]) / 5
		}

		ret, _ := json.MarshalIndent(m.data,"","\t")
		io.WriteString(writer, string(ret))
	})
	http.ListenAndServe(":9193", nil)
}

func (l *LogProcess) Process() {
	// 解析模块
	// 172.0.0.12 - - [04/Mar/2018:13:49:52 +0000] http "GET /foo?query=t HTTP:1.0" 200 2133 "-" "KeepAliveClient" "-" 1.005 1.854

	r := regexp.MustCompile(`([\d\.]+)\s+([^ \[]+)\s+([^ \[]+)\s+\[([^\]]+)\]\s+([a-z]+)\s+\"([^"]+)\"\s+(\d{3})\s+(\d+)\s+\"([^"]+)\"\s+\"(.*?)\"\s+\"([\d\.-]+)\"\s+([\d\.-]+)\s+([\d\.-]+)`)
	loc , _ := time.LoadLocation("Asia/Shanghai")

	// 1. 从Read Channel中读取每行日志数据
	for v := range l.rc {

		// 2. 正则提取所需的监控数据（path，status，method等）
		ret := r.FindStringSubmatch(string(v))
		if len(ret) != 14 {
			TypeMonitorChan <- TypeErrNum
			log.Println("FindStringSubmatch failed : ", string(v))
			continue
		}

		message := &Message{}
		t, err := time.ParseInLocation("02/Jan/2006:15:04:05 +0000", ret[4], loc)
		if err != nil {
			TypeMonitorChan <- TypeErrNum
			log.Println("ParseInLocation failed : ", err.Error(), ret[4])
			continue
		}
		message.TimeLocal = t

		byteSent, _ := strconv.Atoi(ret[8])
		message.ByteSent = byteSent

		reqSli := strings.Split(ret[6]," ")
		if len(reqSli) != 3 {
			TypeMonitorChan <- TypeErrNum
			log.Println("string.Split failed : ", ret[6])
			continue
		}
		message.Method = reqSli[0]
		u, err := url.Parse(reqSli[1])
		if err != nil {
			TypeMonitorChan <- TypeErrNum
			log.Println("url parse failed : ", err)
			continue
		}
		message.Path = u.Path

		message.Scheme = ret[5]
		message.Status = ret[7]

		upstreamTime, err := strconv.ParseFloat(ret[12],64)
		message.UpstreamTime = upstreamTime

		requestTime, err := strconv.ParseFloat(ret[13], 64)
		message.RequestTime = requestTime

		// 3. 写入Write Channel
		l.wc <- message
	}
}

func main() {

	var path, influxDsn string
	flag.StringVar(&path, "path", "/tmp/access.log", "read file path")
	flag.StringVar(&influxDsn, "influxDsn", "http://127.0.0.1:8086@imooc@imoocpass@nginx@s", "influx data source")
	flag.Parse()

	r := &ReadFromFile{
		path: path,
	}

	w := &WriteToInfluxDB{
		influxDBDsn: influxDsn,
	}

	lp := LogProcess{
		rc : make(chan []byte),
		wc : make(chan *Message),
		read: r,
		write: w,
	}

	go lp.read.Read(lp.rc)
	go lp.Process()
	go lp.write.Write(lp.wc)

	m := &Monitor{
		startTime: time.Now(),
		data: SystemInfo{},
	}
	m.start(&lp)

	//time.Sleep(6000 * time.Second)
}
```



