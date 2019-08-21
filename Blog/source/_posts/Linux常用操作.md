---
title: Linux常用操作
date: 2019-07-02 11:13:31
tags: Linux
---

### Python pip

Python pip安装

```
yum -y install python-setuptools
easy install pip
或者
curl "https://bootstrap.pypa.io/get-pip.py" -o "get-pip.py"
python get-pip.py
```

Python pip更新包

```
pip install --upgrade setuptools -i https://pypi.tuna.tsinghua.edu.cn/simple
```

Python pip国内源指定

```
pip install setuptools -i https://pypi.tuna.tsinghua.edu.cn/simple

或者在 ~/.pip/pip.conf 中添加
[global]
index-url=http://pypi.douban.com/simple/
#index-url=https://pypi.tuna.tsinghua.edu.cn/simple
```

Python开源项目编译安装

```
1、下载开源项目源代码
2、安装依赖
pip install -r requirements.txt
3、安装开源项目
python setup.py install
```

安装python3

```
yum -y install python36 python36-tools

安装python3的pip
curl "https://bootstrap.pypa.io/get-pip.py" -o "get-pip.py"
python3.6 get-pip.py
```

### Centos 7升级GCC 7

```
1、确定g++默认使用的C++标准：
g++ -dM -E -x c++ /dev/null | grep -F __cplusplus

2、安装devtoolset-7
yum install -y centos-release-scl && yum install -y devtoolset-7-gcc-c++

3、使用GCC 7
scl enable devtoolset-7 bash
```

### Centos安装扩展源

```
sudo yum install -y epel-release
```

### Linux批量创建嵌套目录

```
mkdir -pv roles/vdbench/{tasks,templates,meta,defaults,vars,files,plugins,handler}
```

### 压缩

- tar.gz格式

```
压缩（把ceph-14.2.1目录压缩为ceph-14.2.1.tar.gz）：tar zcf ceph-14.2.1.tar.gz ceph-14.2.1
解压缩：tar zxf ceph-14.2.1.tar.gz
```



