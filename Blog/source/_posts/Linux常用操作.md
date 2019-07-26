---
title: Linux常用操作
date: 2019-07-02 11:13:31
tags: Linux
---

### Python pip

Python pip安装

```
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

