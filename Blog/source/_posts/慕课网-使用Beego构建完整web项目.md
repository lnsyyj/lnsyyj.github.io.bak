---
title: 慕课网-使用Beego构建完整web项目
date: 2018-10-28 16:25:42
tags: Golang
---

## 课程介绍

- go语言简介
- beego框架介绍
- beego环境搭建
- 应用实践 —— 看图猜电影项目
- 项目部署与发布
- 项目总结

## go简介

- 语法简明紧凑
  - 语法一致
  - 噪音少
- 简洁的并发
  - go在语言级并发抽象
  - 简洁优雅
- 执行速度与开发效率兼优
  - 编译型语言
  - 语法简明

## 环境搭建

### beego安装

- beego的安装

```
yujiangdeMBP-13:~ yujiang$ go get -u -v github.com/astaxie/beego
```

- bee工具安装

bee工具是什么？

​	一个工具包，为了协助快速开发beego项目而创建的项目。（beego项目创建、热编译、开发测试、部署）

```
yujiangdeMBP-13:~ yujiang$ go get -u -v github.com/beego/bee

yujiangdeMBP-13:~ yujiang$ bee version
______
| ___ \
| |_/ /  ___   ___
| ___ \ / _ \ / _ \
| |_/ /|  __/|  __/
\____/  \___| \___| v1.10.0

├── Beego     : 1.10.1
├── GoVersion : go1.10
├── GOOS      : darwin
├── GOARCH    : amd64
├── NumCPU    : 4
├── GOPATH    : /Users/yujiang/go
├── GOROOT    : /usr/local/go
├── Compiler  : gc
└── Date      : Sunday, 28 Oct 2018
```

## beego框架介绍

### 特点

- 快速开发

- MVC架构

- 文档齐全，社区活跃

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/source/_posts/%E6%85%95%E8%AF%BE%E7%BD%91-%E4%BD%BF%E7%94%A8Beego%E6%9E%84%E5%BB%BA%E5%AE%8C%E6%95%B4web%E9%A1%B9%E7%9B%AE/beego-framework.png)

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/source/_posts/%E6%85%95%E8%AF%BE%E7%BD%91-%E4%BD%BF%E7%94%A8Beego%E6%9E%84%E5%BB%BA%E5%AE%8C%E6%95%B4web%E9%A1%B9%E7%9B%AE/beego-cache.png)

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/source/_posts/%E6%85%95%E8%AF%BE%E7%BD%91-%E4%BD%BF%E7%94%A8Beego%E6%9E%84%E5%BB%BA%E5%AE%8C%E6%95%B4web%E9%A1%B9%E7%9B%AE/beego-config.png)

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/source/_posts/%E6%85%95%E8%AF%BE%E7%BD%91-%E4%BD%BF%E7%94%A8Beego%E6%9E%84%E5%BB%BA%E5%AE%8C%E6%95%B4web%E9%A1%B9%E7%9B%AE/beego-context.png)

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/source/_posts/%E6%85%95%E8%AF%BE%E7%BD%91-%E4%BD%BF%E7%94%A8Beego%E6%9E%84%E5%BB%BA%E5%AE%8C%E6%95%B4web%E9%A1%B9%E7%9B%AE/beego-httplibs.png)

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/source/_posts/%E6%85%95%E8%AF%BE%E7%BD%91-%E4%BD%BF%E7%94%A8Beego%E6%9E%84%E5%BB%BA%E5%AE%8C%E6%95%B4web%E9%A1%B9%E7%9B%AE/beego-logs.png)

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/source/_posts/%E6%85%95%E8%AF%BE%E7%BD%91-%E4%BD%BF%E7%94%A8Beego%E6%9E%84%E5%BB%BA%E5%AE%8C%E6%95%B4web%E9%A1%B9%E7%9B%AE/beego-%E5%85%B6%E4%BB%96.png)

## bee工具应用

- bee new：新建项目结构

- bee run：自动编译部署

- bee generate：自动生成代码

```
yujiangdeMBP-13:~ yujiang$ bee new imooc
yujiangdeMBP-13:imooc yujiang$ bee run
浏览器输入 http://localhost:8080/

yujiangdeMBP-13:imooc yujiang$ tree
.
├── conf
│   └── app.conf
├── controllers
│   └── default.go
├── main.go
├── models
├── routers
│   └── router.go
├── static
│   ├── css
│   ├── img
│   └── js
│       └── reload.min.js
├── tests
│   └── default_test.go
└── views
    └── index.tpl

10 directories, 7 files
```

### beego项目运行基本流程

```
文件 - main.go
package main
import (
	_ "imooc/routers"	// _表示只执行imooc/routers的init()函数
	"github.com/astaxie/beego"
)
func main() {
	beego.Run()
}

文件 - routers/router.go
package routers
import (
	"imooc/controllers"
	"github.com/astaxie/beego"
)
func init() {
	// 
    beego.Router("/", &controllers.MainController{})
}

文件 - controllers/default.go
package controllers
import (
	"github.com/astaxie/beego"
)
type MainController struct {
	beego.Controller
}
func (c *MainController) Get() {
	// 这里就是处理get请求的逻辑
	c.Data["Website"] = "beego.me"	// 向模板传递的数据
	c.Data["Email"] = "astaxie@gmail.com"	// 向模板传递的数据
	c.TplName = "index.tpl"  // 渲染的模板文件
}

beego通过浏览器传递参数的方法：（http://localhost:8080/?name="yujiang"）
package controllers
import (
	"github.com/astaxie/beego"
)
type MainController struct {
	beego.Controller
}
func (c *MainController) Get() {
	name := c.GetString("name")
	//c.Data["Website"] = "beego.me"
	c.Data["Website"] = name
	c.Data["Email"] = "astaxie@gmail.com"
	c.TplName = "index.tpl"
}
```

### beego如何进行数据交互



```
[root@localhost ~]# yum install mariadb-server -y
[root@localhost ~]# systemctl start mariadb
[root@localhost ~]# systemctl enable mariadb
[root@localhost ~]# mysql -u root -p

MariaDB [(none)]> create database imooc;
Query OK, 1 row affected (0.00 sec)
MariaDB [(none)]> use imooc;
MariaDB [imooc]> CREATE TABLE `user` ( `id` int(11) NOT NULL AUTO_INCREMENT, `name` varchar(128) NOT NULL DEFAULT '', `gender` tinyint(4) NOT NULL DEFAULT '0', `age` int(11) NOT NULL DEFAULT '0', PRIMARY KEY (`id`) )ENGINE=InnoDB DEFAULT CHARSET=utf8;
Query OK, 0 rows affected (0.00 sec)

MariaDB [imooc]> INSERT INTO user (name,gender,age) values('zhangsan',1,21);
Query OK, 1 row affected (0.00 sec)
MariaDB [imooc]> INSERT INTO user (name,gender,age) values('lisi',0,22);
Query OK, 1 row affected (0.01 sec)
MariaDB [imooc]> INSERT INTO user (name,gender,age) values('wangwu',1,20);
Query OK, 1 row affected (0.00 sec)

MariaDB [imooc]> select * from user;
+----+----------+--------+-----+
| id | name     | gender | age |
+----+----------+--------+-----+
|  1 | zhangsan |      1 |  21 |
|  2 | lisi     |      0 |  22 |
|  3 | wangwu   |      1 |  20 |
+----+----------+--------+-----+
3 rows in set (0.00 sec)

生成代码
yujiangdeMBP-13:imooc yujiang$ bee generate scaffold user -fields="id:int64,name:string,gender:int,age:int" -driver=mysql -conn="root:@tcp(192.168.56.101:3306)/imooc"
______
| ___ \
| |_/ /  ___   ___
| ___ \ / _ \ / _ \
| |_/ /|  __/|  __/
\____/  \___| \___| v1.10.0
2018/10/28 19:30:25 INFO     ▶ 0001 Do you want to create a 'user' model? [Yes|No] 
Yes
2018/10/28 19:31:34 INFO     ▶ 0002 Using 'User' as model name
2018/10/28 19:31:34 INFO     ▶ 0003 Using 'models' as package name
	create	 /Users/yujiang/go/src/imooc/models/user.go
2018/10/28 19:31:34 INFO     ▶ 0004 Do you want to create a 'user' controller? [Yes|No] 
Yes
2018/10/28 19:31:43 INFO     ▶ 0005 Using 'User' as controller name
2018/10/28 19:31:43 INFO     ▶ 0006 Using 'controllers' as package name
2018/10/28 19:31:43 INFO     ▶ 0007 Using matching model 'User'
	create	 /Users/yujiang/go/src/imooc/controllers/user.go
2018/10/28 19:31:43 INFO     ▶ 0008 Do you want to create views for this 'user' resource? [Yes|No] 
Yes
2018/10/28 19:31:52 INFO     ▶ 0009 Generating view...
	create	 /Users/yujiang/go/src/imooc/views/user/index.tpl
	create	 /Users/yujiang/go/src/imooc/views/user/show.tpl
	create	 /Users/yujiang/go/src/imooc/views/user/create.tpl
	create	 /Users/yujiang/go/src/imooc/views/user/edit.tpl
2018/10/28 19:31:52 INFO     ▶ 0010 Do you want to create a 'user' migration and schema for this resource? [Yes|No] 
No
2018/10/28 19:32:11 INFO     ▶ 0011 Do you want to migrate the database? [Yes|No] 
No
2018/10/28 19:32:13 SUCCESS  ▶ 0012 All done! Don't forget to add  beego.Router("/user" ,&controllers.UserController{}) to routers/route.go

2018/10/28 19:32:13 SUCCESS  ▶ 0013 Scaffold successfully generated!

yujiangdeMBP-13:imooc yujiang$ tree
.
├── conf
│   └── app.conf
├── controllers
│   ├── default.go
│   └── user.go
├── imooc
├── main.go
├── models
│   └── user.go
├── routers
│   └── router.go
├── static
│   ├── css
│   ├── img
│   └── js
│       └── reload.min.js
├── tests
│   └── default_test.go
└── views
    ├── index.tpl
    └── user
        ├── create.tpl
        ├── edit.tpl
        ├── index.tpl
        └── show.tpl

11 directories, 14 files
```

安装`go-sql-driver`

```
yujiangdeMBP-13:~ yujiang$ go get -v -u github.com/go-sql-driver/mysql

文件 - routers/router.go
package routers
import (
	"github.com/astaxie/beego"
	"imooc/controllers"
)
func init() {
	//beego.Router("/", &controllers.MainController{})
	beego.Include(&controllers.UserController{})
}

文件 - main.go
package main
import (
	_ "imooc/routers"
	"github.com/astaxie/beego"
	"github.com/astaxie/beego/orm"
	_ "github.com/go-sql-driver/mysql"
)
func main() {
	orm.RegisterDataBase("default","mysql","root:root@tcp(192.168.56.101:3306)/imooc")
	beego.Run()
}

yujiangdeMBP-13:imooc yujiang$ bee run
______
| ___ \
| |_/ /  ___   ___
| ___ \ / _ \ / _ \
| |_/ /|  __/|  __/
\____/  \___| \___| v1.10.0
2018/10/28 20:31:44 INFO     ▶ 0001 Using 'imooc' as 'appname'
2018/10/28 20:31:44 INFO     ▶ 0002 Initializing watcher...
imooc
2018/10/28 20:31:46 SUCCESS  ▶ 0003 Built Successfully!
2018/10/28 20:31:46 INFO     ▶ 0004 Restarting 'imooc'...
2018/10/28 20:31:46 SUCCESS  ▶ 0005 './imooc' is running...
2018/10/28 20:31:46.697 [I] [router.go:269]  /Users/yujiang/go/src/imooc/controllers no changed
2018/10/28 20:31:46.719 [I] [asm_amd64.s:2361]  http server Running on http://:8080
2018/10/28 20:31:55.475 [D] [server.go:2694]  |            ::1| 200 |   3.979416ms|   match| GET      /     r:/
```

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/source/_posts/%E6%85%95%E8%AF%BE%E7%BD%91-%E4%BD%BF%E7%94%A8Beego%E6%9E%84%E5%BB%BA%E5%AE%8C%E6%95%B4web%E9%A1%B9%E7%9B%AE/beego-database.png)

## 应用实践

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/source/_posts/%E6%85%95%E8%AF%BE%E7%BD%91-%E4%BD%BF%E7%94%A8Beego%E6%9E%84%E5%BB%BA%E5%AE%8C%E6%95%B4web%E9%A1%B9%E7%9B%AE/beego-project-1.png)

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/source/_posts/%E6%85%95%E8%AF%BE%E7%BD%91-%E4%BD%BF%E7%94%A8Beego%E6%9E%84%E5%BB%BA%E5%AE%8C%E6%95%B4web%E9%A1%B9%E7%9B%AE/beego-project-2.png)













