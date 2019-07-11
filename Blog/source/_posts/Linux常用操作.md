---
title: Linux常用操作
date: 2019-07-02 11:13:31
tags: Linux
---

Python pip安装

```
curl "https://bootstrap.pypa.io/get-pip.py" -o "get-pip.py"
python get-pip.py
```

Python pip更新包

```
pip install --upgrade setuptools
```

Python pip国内源指定

```
pip install setuptools -i https://pypi.tuna.tsinghua.edu.cn/simple
```



### Centos 7升级GCC 7

```
1、确定g++默认使用的C++标准：
g++ -dM -E -x c++ /dev/null | grep -F __cplusplus

2、安装devtoolset-7
yum install centos-release-scl && yum install devtoolset-7-gcc-c++

3、使用GCC 7
scl enable devtoolset-7 bash
```

