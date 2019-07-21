---
title: Ansible常用模块与操作
date: 2019-07-20 12:50:26
tags: Ansible
---

# ansible

## 命令模式


### 如何查看模块帮助

```
列出所有模块
ansible-doc -l

查看指定模块帮助
ansible-doc -s MODULE_NAME
```

### ansible命令应用基础

```
语法（新版）
ansible <host-pattern> [options]

语法（旧版）
ansible <host-pattern> [-f forks] [-m module_name] [-a args]
	-f forks：启动的并发线程数
	-m module_name：要使用的模块
	-a args：模块特有的参数
```

### 常见模块

```
command：命令模块，默认模块，用于在远程执行命令
	例如：
	ansible all -a 'date'
	ansible all -m command -a 'date'

cron：
	state：
		present：安装
		absent：移除
	例如：
	ansible webserver -m cron -a 'minute="*/10" job="/bin/echo hello" name="test cron job"'
	ansible webserver -a 'crontab -l'
	ansible webserver -m cron -a 'minute="*/10" job="/bin/echo hello" name="test cron job" state=absent'

user：
	name=：指明创建的用户的名字
	例如：
	ansible all -m user -a 'name="user1"'
	ansible all -m user -a 'name="user1" state=absent'

group：
	例如：
	ansible webserver -m group -a 'name=mysql gid=306 system=yes'
	ansible webserver -m user -a 'name=mysql uid=306 system=yes group=mysql'
	
copy：
	src=：定义本地源文件路径
	dest=：定义远程目标文件路径
	content=：取代src=，表示直接用此处指定的信息生成为目标文件内容
	例如:
	ansible all -m copy -a 'src=/etc/fstab dest=/tmp/fstab.ansible owner=root mode=640'
	ansible all -m copy -a 'content="Hello Ansible\nHi MageEdu" dest=/tmp/test.ansible'

file：设定文件属性
	path=：指定文件路径，可以使用name或dest来替换
	创建文件的符号链接：
		src=：指明源文件
		path=：指明符号链接文件路径
	例如：
	ansible all -m file -a 'owner=mysql group=mysql mode=644 path=/tmp/fstab.ansible'
	ansible all -m file -a 'path=/tmp/fstab.link src=/tmp/fstab.ansible state=link'

ping：测试指定主机是否能连接
	例如：
	ansible all -m ping

service：指定服务运行状态
	enabled=：是否开机自动启动，取值为true或者false
	name=：服务名称
	state=：状态，取值有started，stopped，restarted
	例如：
	ansible webserver -a 'service httpd status'
	ansible webserver -a 'chkconfig --list httpd'
	ansible webserver -m service -a 'enabled=true name=httpd state=started'
	
shell：在远程主机上运行命令，尤其是用到管道等功能的复杂命令
	例如：
	ansible all -m user -a 'name=user1'
	ansible all -m command -a 'echo mageedu | passwd --stdin user1' （command模块|管道符无法送过去）
	ansible all -m shell -a 'echo mageedu | passwd --stdin user1' （有管道或变量最好使用shell模块）

script：将本地脚本复制到远程主机并运行
	注意：要使用相对路径指定脚本
	例如：
	ansible all -m script -a "test.sh"

yum：安装程序包
	name=：指明要安装的程序包，可以带上版本号
	state=：present，latest表示安装，absent表示卸载
	例如：
	ansible all -m yum -a "name=vim"

setup：收集远程主机的facts
	每个被管理节点在接收并运行管理命令之前，会将自己主机相关信息，如操作系统版本、IP地址等报告给远程的ansible主机
	ansible all -m setup
```

# ansible-playbook预备知识

## YMAL语法

```
YAML的语法和其他高阶语言类似，并且可以简单表达清单、散列表、标量等数据结构。其结构（Structure）通过空格来展示，序列（Sequence）里的项用"-"来代表，Map里的键值对用":"分隔。下面是一个示例。
name: John Smith
age: 41
gender: Male
spouse:
    name: Jane Smith
    age: 37
    gender: Female
children:
    - name: Jimmy Smith
      age: 17
      gender: Male
    - name: Jenny Smith
      age: 13
      gender: Female
```

### YMAL常见的数据类型

`list` 列表的所有元素均使用"-"开头，例如：

```
# A list of stasty fruits
- Apple
- Orange
- Strawberry
- Mango
```

`dictionary` 字典通过key与value进行标识，例如：

```
# An employee record
name: Example Developer
job: Developer
skill: Elite

也可以将key: value放置于{}中进行表示，例如：
---
# An employee record
{name: Example Developer, job: Developer, skill: Elite}
```

## 基础元素

### 变量

- 变量命名

变量名仅能由字母、数字和下划线组成，且只能以字母开头。

- facts

facts是由正在通信的远程目标主机发回的信息，这些信息被保存在ansible变量中，要获取指定的远程主机所支持的所有facts，可以使用命令：

```
ansible hostname -m setup
```

- register

把任务的输出定义为变量，然后用于其他任务，示例如下：

```
tasks:
    - shell: /usr/bin/foo
      register: foo_result
      ignore_errors: True
```

- 通过命令行传递变量

在运行playbook的时候也可以传递一些变量供playbook使用，示例如下：

```
ansible-playbook test.yml --extra-vars "hosts=www user=mageedu"
```

- 通过roles传递变量

当给一个主机应用角色的时候可以传递变量，然后在角色内使用这些变量，示例如下：

```
- hosts: webservers
  roles:
    - common
    - { role: foo_app_instance, dir: '/web/htdocs/a.com', port: 8080 }
```

### Inventory

ansible主要用在批量主机操作，为了便携的使用其中的部分主机，可以在inventory file中将其分组命名。默认的inventory file为/etc/ansible/hosts。

inventory file可以有多个，且也可以通过Dynamic Inventory来动态生成。

- inventory文件格式

inventory文件遵循INI文件风格，中括号中的字符为组名。可以将同一个主机同时归并到多个不同的组中。此外，如果目标主机使用了非默认的SSH端口，还可以在主机名称之后使用冒号加端口号来标明。

```
[webservers]
www1.magedu.com:2222
www2.magedu.com

[dbservers]
db1.magedu.com
db2.magedu.com
db3.magedu.com

如果主机名称遵循相似的命名模式，还可以使用列表的方式标识各主机，例如：
[webservers]
www[01:50].example.com

[databases]
db-[a:f].example.com
```

- 主机变量

可以在inventory中定义主机时为其添加主机变量以便于在playbook中使用，例如：

```
[webservers]
www1.magedu.com http_port=80 maxRequestsPerChild=808
www2.magedu.com http_port=8080 maxRequestsPerChild=909
```

- 组变量

组变量是赋予给指定组内所有主机上的在playbook中可用的变量。例如：

```:
[webservers]
www1.magedu.com
www2.magedu.com

[webservers:vars]
ntp_server=ntp.magedu.com
nfs_server=nfs.magedu.com
```

- 组嵌套

inventory中，组还可以包含其他的组，并且也可以向组中的主机指定变量。不过，这些变量只能在ansible-playbook中使用，而ansible不支持。例如：

```
[apache]
httpd1.magedu.com
httpd2.magedu.com

[nginx]
ngx1.magedu.com
ngx2.magedu.com

[webservers:children]
apache
nginx

[webservers:vars]
ntp_server=ntp.magedu.com
```

- inventory参数

ansible基于ssh连接inventory中指定的远程主机时，还可以通过参数指定其交互方式，https://docs.ansible.com/ansible/latest/user_guide/intro_inventory.html

# ansible-playbook

playbook是由一个或多个play组成的列表，play的主要功能在于将事先归并为一组的主机装扮成事先通过ansible中的task定义好的角色。从根本上来讲，所谓task无非是调用ansible的一个module。将多个play组织在一个playbook中，即可以让它们连同起来按事先编排的机制同唱一台大戏。下面是一个简单示例。

```
- host: webnodes
  vars:
    http_port: 80
    max_clients: 256
  remote_user: root
  tasks:
  - name: ensure apache is at the latest version
    yum: name=httpd state=latest
  - name: ensure apache is running
    service: name=httpd state=started
  handlers:
    - name: restart apache
      service: name=httpd state=restarted
```

















