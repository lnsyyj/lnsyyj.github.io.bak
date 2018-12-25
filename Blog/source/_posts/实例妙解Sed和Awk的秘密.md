---
title: 实例妙解Sed和Awk的秘密
date: 2018-05-21 22:48:21
tags: shell
---

实例妙解Sed和Awk的秘密笔记

课程地址：http://www.imooc.com/learn/819

课程教师：annieose

# 正则

## 课程介绍

Linux工程师面对的问题

- 面对一堆文本要分析？

- 面对乱乱的数据要处理？
- 面对长长的服务器日志找报错？

sed和awk应用场景

- 例如：在服务器日志fresh.log中，找到所有报错的日期

````
sed -n '/Error/p' fresh.log | awk '{print $1}'
````

Linux三大利器

- grep：查找                                     |

- sed：行编辑器                               |        一行指令，轻松搞定

- awk：文本处理工具                       |

学习目标

- 掌握sed/awk常用处理方法
- 得心应手使用工具
- 提高效率，事半功倍

##  正则表达式

主要内容

- 正则表达式的应用与学习方法
- 正则中的字符/字符串
- 正则中的表达式

正则表达式目标

- 查找                                                        |

- 取出                                                        |        符合条件的某个字符或字符串

- 匹配                                                        |

## 正则单字符的表示方式

字符  ==》  特定字符、范围内字符、任意字符  ==》  组合

```
特定字符：某个具体的字符。例如：'a'，'1'
	grep 'a' passwd
	grep '1' passwd

范围内字符：单个字符[]
	数字字符：[0-9]，[259]
	grep [0-9] passwd
	grep [259] passwd
	
	小写字符：[a-z]
	grep '[a-z]' passwd
	
	大写字符：[A-Z]
	grep '[A-Z]' passwd
	
	扩大范围举例：
	grep '[a-zA-Z]' passwd
	grep '[,:/_]' passwd

范围内字符：反向字符^
	取反：[^0-9]，[^0]

任意字符：
	代表任何一个字符：'.'
	grep '.' passwd
	注意与[.]和\.的区别
	grep '[.]' passwd    代表小数点，而不是任意字符
	grep '\.' passwd     代表小数点，而不是任意字符
```

## 正则其他符号的表示方式

```
边界字符：头尾字符
	^：^root	注意与[^]的区别
	grep '^root' passwd
	
	$:bash$
	grep 'bash$' passwd
	
	空行的表示：^$
	grep '^$' passwd
	
元字符(代表普通字符或特殊字符)
	\w:匹配任何字类字符，包括下划线([A-Za-z0-9])
	\W:匹配任何非字类字符，([^A-Za-z0-9])
	grep '\w' passwd
	grep '\W' passwd
	
	\b:代表单词的分隔
	grep '\bx\b' passwd
```

## 正则组合-重复字符表示

```
字符串 'root'   '1000'   'm..c'
grep 'm..c' passwd   (以m开头c结尾，长度为4的字符串)
grep '[A-Z][a-z]' passwd    (大写字符与小写字符放在一起的字符串组合)
grep '\b[0-9][0-9]\b' passwd   (两位数字的组合)
```

字符串  ==》  重复 + 逻辑  ==》  组合表达式

```
重复：
	*：零次或多次匹配前面的字符或子表达式
	+：一次或多次匹配前面的字符或子表达式
	?：零次或一次匹配前面的字符或子表达式
[root@cephL ~]# cat test.txt 
sesesese
se
seeeeeeeee
eeeeee
soooooooo
+se+se+
[root@cephL ~]# grep 'se*' test.txt 
sesesese
se
seeeeeeeee
soooooooo
+se+se+
[root@cephL ~]# grep 'se\+' test.txt 
sesesese
se
seeeeeeeee
+se+se+
[root@cephL ~]# grep 'se\?' test.txt 
sesesese
se
seeeeeeeee
soooooooo
+se+se+
[root@cephL ~]# grep '\(se\)\+' test.txt 
sesesese
se
seeeeeeeee
+se+se+

重复特定次数：{n,m}
	*：{0,}
	+：{1,}
	?：{0,1}
[root@cephL ~]# grep '\b[0-9]\{2,3\}\b' passwd       (截取2位或3位数字)
```

## 正则组合-逻辑符合表示

```
任意字符串的表示：.*
	例如：^r.*    m.*c
	注意与m..c区别
grep '^r.*' passwd
grep 'm.*c' passwd
grep 'm..c' passwd
grep '\bm.*c\b' passwd
grep '\bm[a-z]*c\b' passwd
逻辑的表示：
	|：'/bin\(false\|true\)'
grep 'bin/\(bash\|nologin\)' passwd    (或者包含bash，或者包含nologin)

正则表达式字符组合小结：
	重复的表示：* + ? {,}
	逻辑的表示：|
```

## 正则-案例演

```
案例一
	匹配4-10位qq号
	grep '^[0-9]\{4,10\}$' qq.txt
案例二
	匹配15位或18位身份证号(支持带X的)
	grep '^[1-9]\([0-9]\{13\}\|[0-9]\{16\}\)[0-9xX]$' qq.txt
案例三
	匹配密码(由数字、26个字母和下划线组成)
	grep '^\w\+$' qq.txt
```

## 正则总结

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/Blog/source/_posts/实例妙解Sed和Awk的秘密/p1.png?raw=true)

# 巧妙破解sed

## 主要内容和原理介绍
sed工具用途

```
自动处理文件
分析日志文件
修改配置文件
```

### sed是如何进行文本处理的？

我们称sed是一个流处理编辑器

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/Blog/source/_posts/实例妙解Sed和Awk的秘密/QQ20180523-153042@2x.png?raw=true)

实际的处理流程是行处理

- sed一次处理一行内容
- sed不改变文件内容（除非重定向）

## sed应用的基本格式介绍

正则选定文本  ==》  sed进行处理

```
使用sed格式：
	命令行格式
		`sed [options] 'command' file(s)`
		`options: -e ; -n`
		`command: 行定位（正则）+ sed命令（操作）`
	脚本格式
		`sed -f scriptfile file(s)`
```

使用sed举例

```
sed -n '/root/p'
sed -e '10,20d' -e 's/false/true/g'
```

## sed基本操作命令-p命令

基本操作命令（1）

```
-p（打印相关的行）与-n配合使用
	sed -n 'p' passwd

定位一行：x;/pattern/         （x代表行号）
	sed -n '10p' passwd       (打印第10行)
	nl passwd | sed -n '10p'  (nl打印行号)
	sed -n '/root/p' passwd   (打印)

定位几行：x,y;/pattern/,x;    （x，y代表行号）
		/pattern1/,/pattern2/;
		x,y!				(取反)

nl passwd  | sed -n '10,20p'	(打印10行到20行)
nl passwd  | sed -n '/operator/,/games/p'
nl passwd | sed -n '10!p'        (不选择第10行)
nl passwd | sed -n '10,20!p'     (不选择10行到20行)

定位间隔几行：first~step
	nl passwd | sed -n '1~2p'	(从第1行开始，间隔2-1行，再打印)
```

基本操作命令（2）

```
-a （新增行）/ i（插入行）
-c （替代行）
-d（删除行）
	nl passwd | sed '5a ==========='     (第5行和第6行之间增加分隔符===========)
	nl passwd | sed '1,5a==========='    (第1行和第5行之间，每一行都增加分隔符===========）
	nl passwd | sed '5i==========='      (第5行之前插入分隔符===========)
	nl passwd | sed '1,5i==========='    (第1行和第5行之前，每一行都插入分隔符===========）
	nl passwd | sed '40c 123123'         (将40行替换成123123)
	nl passwd | sed '35,40c 123123'      (将35行到40行替换成123123，35行以后的内容就没有了，只有123123这一行，并不是每一行进行替换，而是对这些行做一个整体的替换)
	nl passwd | sed '/mooc/d'            (删除mooc所在行)
	sed不会改变源文件的内容，如果要保存结果需要重定向
```

案例：

```
案例一：优化服务器的配置
	在ssh的配置文件加入相应文本：
	"
	Port 52113
	PermitRootLogin no
	PermitEmptyPasswords no
	"
	sed '$a \    port\ 52113 \n    permitroologin no' sshd_config    (空格需要\转义)

案例二：文本处理
	删除文本中空行
	sed '/^$/d' sshd_config      (空行用正则/^$/表示)

案例三：服务器日志处理
	服务器log中找出error报错
	sed -n '/Error/p'
```

基本操作命令（3）

```
-s（替换）：分隔符/,#等
	sed 's/nologin/l/' passwd         (nologin替换为l)
-g（全局）：替换标志
	sed 's/:/%/' passwd      (只是把每一行的第一个':'替换为'%')
	sed 's/:/%/g' passwd     (所有的':'都被替换为'%')

案例四：数据筛选
	获取enp0s8的ip
	ifconfig enp0s8 | sed -n '/inet /p' | sed 's/inet//'        (centos7)
	ifconfig enp0s8 | sed -n '/inet /p' | sed 's/inet.*r://'    (Ubuntu上有:)
	ifconfig enp0s8 | sed -n '/inet /p' | sed 's/inet//' | sed 's/n.*$//'
```

高级操作命令（1）

```
-{}：多个sed命令，用;分开
	nl passwd | sed '{20,30d;s/nologin/login/}'   (先删除20行到30行，然后剩下的行nologin替换成login)
```

高级操作命令（2）

```
-n：读取下一个输入行（用下一个命令处理）
	nl passwd | sed -n '{n;p}'    (你会发现跳行，输出2、4、6、8行。执行过程是sed读入一行后，接着n又读入下一行。输出偶数行)
	nl passwd | sed -n '2~2p'     (输出偶数行)
	nl passwd | sed -n '{p;n}'    (输出奇数行)
	nl passwd | sed -n '1~2p'     (输出奇数行，与上一条命令相等)
```

高级操作命令（3）

```
-&：替换固定字符串
	sed 's/^[a-z_-]\+/&           /' passwd      (用户名后面加上空格)
	root:x:0:0:root:/root:/bin/bash              (源字符串)
	root           :x:0:0:root:/root:/bin/bash   (替换后字符串)
	如果替换的字符串与源字符串有重复串，可以用&。类似于在源字符串后追加
```

案例：

```
案例一：大小写转换
	将用户名的首字母转换为大写/小写
	（元字符\u \l \U \L：转换为大写/小写字符，小写的\u和\l是对首字母的转换，大写的\U和\L是对一串字符的转换）
	sed 's/^[a-z_-]\+/\u&/' passwd

案例二：大小写转换
	将文件夹下的.txt文件名转换为大写
	ls *.txt | sed 's/^\w\+/\U&/'

案例三：数据筛选
	获取passwd中USER、UID和GID
	sed 's/\(^[a-z_-]\+\):x:\([0-9]\+\):\([0-9]\+\):.*$/USER:\1 UID:\2 GID:\3/' passwd
```

高级操作命令（4）

```
-\( \)：替换某种(部分)字符串(\1,\2)
	案例二：数据筛选
	"获取eth0的ip"
	ifconfig enp0s9 | sed -n '/inet /p' | sed 's/inet \([0-9.]\+\) .*$/\1/' (centos 7)
```

高级操作命令（5）

```
-r：复制指定文件插入到匹配行(不改变文件内容只改变输出，如果想改变文件内容需要重定向)
-w：复制匹配行拷贝指定文件里(改变目标文件)
用处：源文件和目标文件互相操作。
sed '1r 123.txt' abc.txt    (从123.txt中读取内容，插入到abc.txt的第1行后 )
sed '1w abc.txt' 123.txt    (从123.txt中读入，写入到abc.txt。1w是指定123.txt中的哪一行写入到abc.txt)
```

高级操作命令（6）

```
q：退出sed
nl passwd | sed '10q'   (执行到第10行就结束)
nl passwd | sed '/false/q'   (找到第一个false就结束)
```

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/Blog/source/_posts/实例妙解Sed和Awk的秘密/QQ20180531-102131%402x.png)

## awk

主要用途：统计、制表

awk处理方式：awk一次处理一行内容、awk对每行可以切片处理

```
awk '{print $1}'	//输出首个单词
```

使用awk的格式：

```
1、命令行格式：
awk [options] 'command' file(s)
	command: pattern {awk 操作命令}
		pattern: 正则表达式; 逻辑判断式
		command1: pattern {awk 操作命令}
			操作命令: 内置函数: print() printf() getline..;
			控制指令: if(){...}else{...};while(){...};

2、脚本格式：
awk -f awk-script-file file(s)
```

awk内置参数应用：

```
awk 内置变量1
	$0：表示整个当前行
	$1：每行第一个字段
	$2：每行第二个字段
这些字段是怎样划分的呢？我们用分隔符
awk内置参数：分隔符
	options：-F field-separator(默认为空格)
	例如：awk -F ':' '{print $3}' /etc/passwd
	
	[root@cephL]# awk -F ':' '{print $1,$3}' passwd 
	              awk -F ':' '{print $1" "$3}' passwd
	              awk -F ':' '{print $1"\t"$3}' passwd
	              awk -F ':' '{print "User:"$1"\t""UID:"$3}' passwd
awk内置变量：
	NR：每行的记录号(行)
	NF：字段数量变量(列)
	FILENAME：正在处理的文件名
	例如：awk -F ':' 'print FILENAME' passwd
```

awk内置参数应用案例：

```
案例一：
	显示/etc/passwd每行的行号，每行的列数，对应行的用户名(print，printf)
	awk -F ':' '{print "Line: "NR,"Col: "NF,"User: "$1}' passwd
	awk -F ':' '{printf("Line: %3s Col: %s User: %s\n",NR,NF,$1)}' passwd
	
案例二：
	显示/etc/passwd中用户ID大于100的行号和用户名(if...else...)
	awk -F ':' '{if ($3>100) print "Line: "NR,"User: "$1}' passwd
案例三：
	在服务器log中找出'Error'的发生日期
	sed '/Error/p' fresh.log | awk '{print $1}'
	awk '/Error/{print $1}' fresh.log
```

使用awk——逻辑判断式

```
command: pattern {awk 操作命令}
	pattern: 正则表达式;逻辑判断式

awk逻辑
	~,!~:	匹配正则表达式
	==,!=,<,>:	判断逻辑表达式

1、第一个字段是'm'开头的
awk -F ':' '$1~/^m.*/{print $1}' passwd
2、第一个字段不是'm'开头的
awk -F ':' '$1!~/^m.*/{print $1}' passwd
3、第三个字段大于100
awk -F ':' '$3>100{print $1,$3}' passwd
```

使用awk——扩展格式

```
1、命令行格式：
awk [options] 'command' file(s)
	command2扩展
	BEGIN{print "start"} pattern{commands} END{print "end"}
	awk是以行为操作单位的，BEGIN是在没有读入第一行之间执行的，END是处理完最后一行执行的

案例一：
	制表显示/etc/passwd每行的行号，每行的列数，对应行的用户名
	awk -F ':' 'BEGIN{print "Line\tCol\tUser"}{print NR"\t"NF"\t"$1}END{print "---"FILENAME"---"}' passwd

案例二：
	统计当前文件夹下的文件/文件夹占用的大小
	ls -l | awk 'BEGIN{size=0}{size+=$5}END{print "size is "size/1024/1024"M"}'

案例三：
	统计显示/etc/passwd的账户总人数
	awk -F ':' 'BEGIN{count=0}$1!~/^$/{count++}END{print "count = "count}' passwd
	统计显示UID大于100的用户名
	awk -F ':' 'BEGIN{count=0}{if ($3 > 100) name[count++]=$1}END{for(i=0;i<count;i++) print i,name[i]}' passwd

案例四：
	统计netstat -anp 状态下为LISTEN和CONNECTED的连接数量
	netstat -anp | awk '$6~/CONNECTED|LISTEN/{sum[$6]++}END{for(i in sum)print i,sum[i]}'
```

awk VS sed

```
awk和sed都可以处理文本
awk侧重于复杂逻辑处理
sed侧重于正则处理

awk和sed可以共同使用
```

![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/Blog/source/_posts/实例妙解Sed和Awk的秘密/QQ20180530-000840%402x.png)









