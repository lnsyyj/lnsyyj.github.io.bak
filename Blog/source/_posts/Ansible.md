---
title: Ansible
date: 2018-07-24 21:58:25
tags: Ansible
---



## 环境准备

```
Python
Setuptools
pip(可选)
```

## Ansible快速安装

```
安装pip：easy_install pip
安装Ansible：pip install ansible
```

## Ansible源码安装

```
获取源码
解压源码
进入源码目录
运行source ./hacking/env-setup
```

## Ansible系统源安装

```
Centos
 - yum install ansible
Ubuntu
 - apt-get install software-properties-common
 - apt-add-repository ppa:ansible/ansible
 - apt-get update
 - apt-get install ansible
```

## Ansible运行

```
      client              server
Ansible --> ssh =====> ssh --> shell
```

## Ansible配置文件路径搜索优先级

```
export ANSIBLE_CONFIG
./ansible.cfg
~/.ansible.cfg
/etc/ansible/ansible.cfg
```

## Ansible配置文件获取

```
/etc/ansible/目录下
https://raw.github.com/ansible/ansible/devel/examples/ansible.cfg
```

## Ansible配置详细

```
defaults默认配置项
privilege_escalation执行命令的用户权限设置(ssh到客户机以哪个用户执行命令，关于这个用户的权限设置)
paramiko_connection paramika插件设置(python插件实现的ssh协议，如果不设置会走Ansible自己的ssh协议通道)
ssh_connection ssh连接设置
accelerate
selinux & colors
```

```
ask_pass & ask_sudo_pass
ask_pass：可以控制，Ansible剧本playbook是否会自动默认弹出密码输入框(遇到输入密码的情况下)
ask_sudo_pass：用户使用的系统平台开启了sudo密码的话，应该开启这一参数(如果目标机开启了sudo免密码的话，应该开启该项，否则报错)
```

```
gather_subset
gather_subset：设置手机的内容，包括all，network，hardware，virtual，facter，ohai
```

```
remote_port & remote_tmp & remote_user
remote_port：设置远程客户机ssh监听的端口
remote_tmp：设置远程客户机临时目录
remote_user：设置远程客户机执行命令的用户(以哪个用户执行shell)
```

```
sudo_exe & sudo_flags & sudo_user
sudo_exe：sudo命令路径(默认/usr/bin)
sudo_flags：sudo参数
sudo_user：能够使用sudo的user
```

```
action_plugins & callback_plugins & connection_plugins & filter_plugins & lookup_plugins & vars_plugins
开发者中心的插件相关功能，开发者可以开发相应的插件，来完成自己的功能。
action_plugins：激活事件(在某个事件发生的时候，去执行插件中的内容)
callback_plugins：回调
connection_plugins：连接
filter_plugins：过滤器
lookup_plugins：加载路径
vars_plugins：任何地方加载
```

```
forks
forks：最大开辟的进程数，这个数不宜过大，过大性能损耗高，不易过小，过小并发性能低，一般的设置方法是cpu核数*2
```

```
module_name
module_name：这个是/usr/bin/ansible的默认模块名(-m)。默认是'command'模块。之前提到过，command模块不支持shell变量，管道，配额。所以也许你希望把这个参数改为'shell'。
```

 ```
vault_password_file
vault_password_file：这个文件也可以是脚本的形式。如果你使用脚本而不是单纯文件的话，请确保它可以执行并且密码可以在标准输出上打印出来。如果你的脚本需要提示请求数据，请求将会发到标准错误输出中。

存放远程客户机密码的文件，因为有时候sudo是需要输入密码的，而我们希望Ansible执行比较流畅(不需要人工干预)，这个时候可以通过机器代填密码的方式。这个文件可以是个脚本，也可以是单独的一条密码。如果是脚本，确保该脚本是可运行的，并且运行的，结果输出到标准输出中stdout。
 ```

```
pattern
pattern：如果没有提供"hosts"节点，这是playbook要通信的默认主机组。默认值是对所有主机通信，如果不想被惊吓到，最好还是设置这个选项
```

```
inventory & library
inventory：可通信主机的目录(默认/etc/ansible/hosts)
library：Ansible默认搜索模块路径(默认/usr/share/my_modules/)
```

## 新手上路

如何添加一台机器？

```
1、编辑/etc/ansible/hosts
2、添加本机的public SSH key到目标机器的authorized_keys
3、添加本机的私钥到Ansible
4、运行ansible all -m ping测试是否添加成功
```

Ansible命令格式——ansible all -m ping

```
1、ansible命令主体 —— ansible/ansible-playbook
2、被操作的目标机器的正则表达式 —— all
3、指定要使用的模块 —— -m ping
4、传入的参数（ ansible all -a 'ls' ） 
```

执行ping模块发生了什么？

```
唤起Ansible --> 查询hosts，获取指定的目标机器 --> 调用指定模块 --> 传入参数 --> 返回执行结果
```

Ansible命令详解

```
-a 指定传入模块的参数
-C -D 两个一起使用，检查hosts规则文件的修改
-l 限制匹配规则的主机数
--list-hosts 显示所有匹配规则的主机数
-m -M 指定所使用的模块和模块的路径
--syntax-check 检查语法
-v 显示详细的日志
```

```
ansible all -a 'ls'
ansible all -a 'ls' --list-hosts
```

Ansible——Inventory（分组）

什么是Inventory？

```
Ansible可同时操作属于一个组的多台主机，组和主机之间的关系通过inventory文件配置。默认的文件路径为/etc/ansible/hosts
中文：库存、清单
作用：分组

一个简单的实例
one.example.com
[webservers]
two.example.com
three.example.com
方括号[]中是组名，一个主机可以属于不同的组，比如一台服务器可以同时属于webserver组和vim组。这时，指定webserver或者vim组，都可以操作这台主机。

ansible webservers -a 'ls'
ansible <IP> -a 'ls'

如何自定义连接端口？
在编写hosts文件时，在域名或者ip后加上端口号，例如：
192.168.100.101:5000
one.example.com:6000

IP域名写起来太长？起个别名吧
jumper ansible_ssh_port=5555 ansible_ssh_host=192.168.1.50

不想以root用户登录
例如：
jumper ansible_ssh_port=5555 ansible_ssh_host=192.168.1.50 ansible_ssh_user=papa

机器太多，但是连续的，写起来好麻烦
[vim]
vim[1:50].example.com
vim[a-f].example.com

Inventory参数选项
ansible_ssh_host
ansible_ssh_port
ansible_ssh_user
ansible_ssh_pass  ssh密码(非公钥免密方式)
ansible_sudo_pass sudo密码
ansible_sudo_exe
ansible_connection 与主机的连接类型。比如：local，ssh或者paramiko。
ansible_ssh_private_key_file 私钥文件
ansible_shell_type 目标系统的shell类型
ansible_python_interpreter
```

分文件管理

```
分文件 —— 例如：
/etc/ansible/group_vars/vim         (一组)
/etc/ansible/group_vars/webservers  (一组)
/etc/ansible/host_vars/foosball     (不分组)
vim文件中的内容大致这样：
	---
	ntp_server:acme.example.org
	database_server:storage.example.org
这时候文件名就是组名
```

分文件夹管理

```
分文件 —— 例如：
/etc/ansible/group_vars/vim/install
/etc/ansible/group_vars/vim/init
/etc/ansible/host_vars/foosball
install文件中的内容大致这样：
	---
	ntp_server:acme.example.org
	database_server:storage.example.org
这时候文件夹名就是组名
```

Ansible——Patterns

```
什么是Patterns？
	在Ansible中，Patterns是指我们通过类正则表达式的方式，决定与哪台主机进行交互
	
1、所有hosts机器：all或者星号(*)
	例如：
		ansible all -m ping
2、通过域名或者IP指定一台特定的机器
	例如：
		ansible one.example.com -m ping
		ansible 192.168.1.2 -m ping
3、指定一组连续机器
	例如：
		ansible 192.168.1.* -m ping 指定192.168.1.0/24段的所有机器
4、指定一组不相关的机器
	例如：
		ansible one.example.com:two.example.com -m ping 指定one two两台机器
5、组也支持Patterns
	webservers -- 指定webserver组
	webservers:nginx -- 指定webservers和nginx两个组
6、与和非
	webservers:!phoenix -- 指定在webserver组但不在phoenix组
	webservers:&nginx -- 指定同时在webservers和nginx组

问题：webservers:nginx:&vim:!python:!mysql
如上的语句是什么意思？
温馨提示：匹配时从左到右依次匹配！
```

Ansible——Ad-Hoc

```
什么是Ad-Hoc
	这其实是一个概念性的名字，是相对于写Ansible playbook来说的。类似于在命令行敲入shell命令和写shell scripts两者之间的关系
	如果我们敲入一些命令去比较快的完成一些事情，而不需要将这些执行的命令保存下来，这样的命令就叫做ad-hoc命令。

执行shell命令：
	ansible atlanta -a "/sbin/reboot" -f 10
	这个命令中，atlanta是一个组，这个组里面有很多服务器，"/sbin/reboot"命令会在atlanta组下的所有机器上执行。这里ssh-agent会fork出10个子进程(bash)以并行的方式执行reboot命令。如前所说"每次重启10个"即是以这种方式实现。
	
指定用户执行shell命令：
	ansible atlanta -a "/usr/bin/foo" -u username
使用sudo：
	ansible atlanta -a "/usr/bin/foo" -u username --sudo [--ask-sudo-pass]

文件管理：
	创建文件夹：ansible webservers -m file -a "dest=/path/to/c mode=755 owner=mdehaan group=mdehaan state=directory"
	拷贝文件：ansible atlanta -m copy -a "src=/etc/hosts dest=/tmp/hosts"

软件包管理：
	确认一个软件包已经安装，但不去升级它：ansible webservers -m yum -a "name=acme state=present"
	确认一个软件包的安装版本：ansible webservers -m yum -a "name=acme-1.5 state=present"
	确认一个软件包还没有安装：ansible webservers -m yum -a "name=acme state=absent"

其他模块
	git模块：git相关
	service模块：系统服务相关
	setup模块：系统环境相关
```

如何执行一个耗时的任务

```
ansible all -B 3600 -P 0 -a "/usr/bin/long_running_operation --do-stuff"
-B 1800 表示最多运行60分钟，-P 0表示不获取状态

ansible all -B 1800 -P 60 -a "/usr/bin/long_running_operation --do-stuff"
-B 1800表示最多运行30分钟，-P 60表示每隔一分钟获取一次状态

ansible web1.example.com -m async_status -a "jid=488359678239.2844"
```

## Ansible——Playbook简介

什么是playbook？

```
简单来说，playbook是一种简单的配置管理系统与多机器部署系统的基础。与现有的其他系统有不同之处，且非常适合于复杂应用的部署。

playbook可用于声明配置，更强大的地方在于，在playbooks中可以编排有序的执行过程，甚至于做到在多组机器间，来回有序的执行特别指定的步骤。并且可以同步或异步的发起任务。
```

## Ansible API

API提供了哪些功能？

```
1、调用Ansible的模块
2、开发动态的Inventory数据源
3、更好的控制playbook等功能的运行
```

如何调用Ansible模块？

```
1、引入Ansible runner库
2、初始化runner对象，传入相关参数
3、运行runner对象的run函数

示例代码（Ansible2.0之前）：
import ansible.runner

runner = ansible.runner.Runner(
	module_name='ping',
	module_args='',
	pattern='web*',
	forks=10
)

datastructrue = runner.run()
```

如何调用Ansible2.0 API？

```
1、定义一个结果对象
2、初始化Ansible节点对象
3、初始化结果对象
4、创建一个任务
5、运行ansible节点

实例代码：

```



## 课程地址

https://www.imooc.com/video/15236