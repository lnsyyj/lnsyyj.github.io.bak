---
title: ceph中的单元测试
date: 2019-01-07 16:31:08
tags: ceph
---

Ceph中的单元测试使用的是Google Test，嵌入到Ceph源码中。Google Test的使用这里有一篇[入门文章](http://www.yeolar.com/note/2014/12/21/gtest/#id15)，大体思路讲的很清晰。我们主要重心是如何在Ceph源码中添加单元测试。

在添加之前我们先读一下已经存在的单元测试，在代码中不同的单元测试实现方式不同，可能按照以下Demo无法完全复制，但是慢慢看代码总会理解的。我们挑一个最简单的，不讲解详细测试内容（因为涉及知识较多，目前我还无法完全理解）。

下面我们以`ceph_test_filejournal`为例，它存在于ceph-test*.rpm包中，在打社区版Ceph rpm包时（根据ceph.spec）会生成出来《[ceph build rpm](https://lnsyyj.github.io/2018/09/13/ceph-build-rpm/)》，在我们的被测Ceph节点上安装该rpm，会在`/usr/bin/`目录中找到`ceph_test_filejournal`（其实会有很多类似`ceph_test_`的二进制文件，这些都是Ceph的单元测试）。

# CMakeLists

单元测试总CMakeLists文件位置在`src/test/CMakeLists.txt`，如果想自己编写测试代码添加到测试集中，需要修改这个CMakeLists。在社区Ceph我们编译出来可执行测试二进制`ceph_test_*`，都是在这里通过CMakeLists `add_executable`定义的。

# 单元测试代码位置

```
add_executable(ceph_test_filejournal
  test_filejournal.cc
  )
```

在`add_executable`中同时定义了测试代码源文件`test_filejournal.cc`。

```
  ::testing::InitGoogleTest(&argc, argv);
```







# 参考

【1】http://www.yeolar.com/note/2014/12/21/gtest/#id15

【2】https://www.ibm.com/developerworks/cn/aix/library/au-googletestingframework.html



