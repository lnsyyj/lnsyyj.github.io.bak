---
title: ceph 官方librbd接口demo
date: 2020-08-01 21:25:37
tags: ceph
---

以下代码来自ceph master分支，examples/librbd/hello_world.cc目录，该目录中有操作librbd接口的demo。

实验使用ceph-14.2.10

```
// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab
/*
 * Ceph - scalable distributed file system
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software
 * Foundation. See file COPYING.
 */

// ceph 14.2.10需要安装libradospp-dev，否则无法找到rados/librados.hpp
// 安装librados-dev 和librbd-dev package，便可include rados/librados.hpp与rbd/librbd.hpp头文件
#include <rados/librados.hpp>
#include <rbd/librbd.hpp>
#include <iostream>
#include <string>
#include <sstream>

int main(int argc, const char **argv) {

    int ret = 0;

    // 我们将在下面使用这些变量
    const char *pool_name = "hello_world_pool";
    std::string hello("hello world!");
    std::string object_name("hello_object");
    librados::IoCtx io_ctx;

    // 首先，我们创建一个Rados对象并对其进行初始化
    librados::Rados rados;
    {
        ret = rados.init("admin"); // 只需使用 client.admin keyring
        if (ret < 0) { // 让我们处理可能出现的任何错误
            std::cerr << "couldn't initialize rados! error " << ret << std::endl;
            ret = EXIT_FAILURE;
            goto out;
        } else {
            std::cout << "we just set up a rados cluster object" << std::endl;
        }
    }

    /*
     * 现在我们需要获取rados对象的config info.
     * 它可以为我们解析argv来找到ID，monitors等，因此我们就使用它。
     */
    {
        ret = rados.conf_parse_argv(argc, argv);
        if (ret < 0) {
            // 这不太可能发生，但是我们需要努力成为一个好公民。
            std::cerr << "failed to parse config options! error " << ret << std::endl;
            ret = EXIT_FAILURE;
            goto out;
        } else {
            std::cout << "we just parsed our config options" << std::endl;
            // 如果用户指定了config file，我们也想使用它，而conf_parse_argv不会为我们这样做。
            for (int i = 0; i < argc; ++i) {
                if ((strcmp(argv[i], "-c") == 0) || (strcmp(argv[i], "--conf") == 0)) {
                    ret = rados.conf_read_file(argv[i + 1]);
                    if (ret < 0) {
                        // 如果config file格式错误，这可能会失败
                        std::cerr << "failed to parse config file " << argv[i + 1]
                                  << "! error" << ret << std::endl;
                        ret = EXIT_FAILURE;
                        goto out;
                    }
                    break;
                }
            }
        }
    }

    /*
     * 接下来，我们connect到cluster
     */
    {
        ret = rados.connect();
        if (ret < 0) {
            std::cerr << "couldn't connect to cluster! error " << ret << std::endl;
            ret = EXIT_FAILURE;
            goto out;
        } else {
            std::cout << "we just connected to the rados cluster" << std::endl;
        }
    }

    /*
     * 让我们创建自己的pool
     * 请注意，此命令将创建默认PG数的pool（PG数由monitors指定），这可能不适合生产使用，但可以进行测试。
     */
    {
        ret = rados.pool_create(pool_name);
        if (ret < 0) {
            std::cerr << "couldn't create pool! error " << ret << std::endl;
            return EXIT_FAILURE;
        } else {
            std::cout << "we just created a new pool named " << pool_name << std::endl;
        }
    }

    /*
     * 创建一个IoCtx，用于对pool进行IO
     */
    {
        ret = rados.ioctx_create(pool_name, io_ctx);
        if (ret < 0) {
            std::cerr << "couldn't set up ioctx! error " << ret << std::endl;
            ret = EXIT_FAILURE;
            goto out;
        } else {
            std::cout << "we just created an ioctx for our pool" << std::endl;
        }
    }

    /*
     * 创建一个rbd image并向其写入数据
     */
    {
        std::string name = "librbd_test";
        uint64_t size = 2 << 20;
        int order = 0;
        librbd::RBD rbd;
        librbd::Image image;

        ret = rbd.create(io_ctx, name.c_str(), size, &order);
        if (ret < 0) {
            std::cerr << "couldn't create an rbd image! error " << ret << std::endl;
            ret = EXIT_FAILURE;
            goto out;
        } else {
            std::cout << "we just created an rbd image" << std::endl;
        }

        ret = rbd.open(io_ctx, image, name.c_str(), NULL);
        if (ret < 0) {
            std::cerr << "couldn't open the rbd image! error " << ret << std::endl;
            ret = EXIT_FAILURE;
            goto out;
        } else {
            std::cout << "we just opened the rbd image" << std::endl;
        }

        int TEST_IO_SIZE = 512;
        char test_data[TEST_IO_SIZE + 1];
        int i;

        for (i = 0; i < TEST_IO_SIZE; ++i) {
            test_data[i] = (char) (rand() % (126 - 33) + 33);
        }
        test_data[TEST_IO_SIZE] = '\0';

        size_t len = strlen(test_data);
        ceph::bufferlist bl;
        bl.append(test_data, len);

        ret = image.write(0, len, bl);
        if (ret < 0) {
            std::cerr << "couldn't write to the rbd image! error " << ret << std::endl;
            ret = EXIT_FAILURE;
            goto out;
        } else {
            std::cout << "we just wrote data to our rbd image " << std::endl;
        }

        /*
         * 读取image并将其与我们编写的数据进行比较
         */
        ceph::bufferlist bl_r;
        int read;
        read = image.read(0, TEST_IO_SIZE, bl_r);
        if (read < 0) {
            std::cerr << "we couldn't read data from the image! error" << std::endl;
            ret = EXIT_FAILURE;
            goto out;
        }

        std::string bl_res(bl_r.c_str(), read);

        int res = memcmp(bl_res.c_str(), test_data, TEST_IO_SIZE);
        if (res != 0) {
            std::cerr << "what we read didn't match expected! error" << std::endl;
        } else {
            std::cout << "we read our data on the image successfully" << std::endl;
        }

        image.close();

        /*
         * 现在删除image
         */
        ret = rbd.remove(io_ctx, name.c_str());
        if (ret < 0) {
            std::cerr << "failed to delete rbd image! error " << ret << std::endl;
            ret = EXIT_FAILURE;
            goto out;
        } else {
            std::cout << "we just deleted our rbd image " << std::endl;
        }
    }

    ret = EXIT_SUCCESS;
    out:
    /*
     * 现在我们已经完成了，因此让我们删除pool，然后正常关闭connection。
     */
    int delete_ret = rados.pool_delete(pool_name);
    if (delete_ret < 0) {
        // be careful not to
        std::cerr << "We failed to delete our test pool!" << std::endl;
        ret = EXIT_FAILURE;
    }

    rados.shutdown();

    return ret;
}
```

examples/librbd/Makefile文件

```

CXX?=g++
CXX_FLAGS?=-std=c++11 -Wno-unused-parameter -Wall -Wextra -Werror -g
CXX_LIBS?=-lboost_system -lrbd -lrados
CXX_INC?=$(LOCAL_LIBRADOS_INC)
CXX_CC=$(CXX) $(CXX_FLAGS) $(CXX_INC) $(LOCAL_LIBRADOS)

# Relative path to the Ceph source:
CEPH_SRC_HOME?=../../src
CEPH_BLD_HOME?=../../build

LOCAL_LIBRADOS?=-L$(CEPH_BLD_HOME)/lib/ -Wl,-rpath,$(CEPH_BLD_HOME)/lib
LOCAL_LIBRADOS_INC?=-I$(CEPH_SRC_HOME)/include

all: hello_world_cpp 

# Build against the system librados instead of the one in the build tree:
all-system: LOCAL_LIBRADOS=
all-system: LOCAL_LIBRADOS_INC=
all-system: all

hello_world_cpp: hello_world.cc
	$(CXX_CC) -o hello_world_cpp hello_world.cc $(CXX_LIBS)

clean:
	rm -f hello_world_cpp
```

# 安装调试包

```
apt-get install -y ceph-base-dbg ceph-common-dbg ceph-fuse-dbg ceph-mds-dbg ceph-mgr-dbg ceph-mon-dbg ceph-osd-dbg libcephfs2-dbg librados2-dbg libradosstriper1-dbg librbd1-dbg librgw2-dbg python-cephfs-dbg python-rados-dbg python-rbd-dbg python-rgw-dbg python3-cephfs-dbg python3-rados-dbg python3-rbd-dbg python3-rgw-dbg radosgw-dbg rbd-fuse-dbg rbd-mirror-dbg rbd-nbd-dbg

ubuntu包的命名规则，引致  https://blog.csdn.net/anlian523/article/details/90733234
通常情况下
-dev后缀（develope）：包含了库的接口（.h文件即头文件），这个为了当你开发一个程序时想要链接到这个包时。
-dbg后缀（debug）：包含调试符号，通常仅供开发人员使用该软件或调试软件的人员使用。
-utils后缀（utility）：通常提供一些额外的命令行工具。 它可能会将用户暴露给内部功能或仅提供CLI。
```

编译并使用gdb调试

```
make -d

gdb ./hello_world_cpp
(gdb) b main
(gdb) r -c /etc/ceph/ceph.conf 

### 66 break;
(gdb) p argv[0]
$2 = 0x7fffffffe77b "/root/codes/hello_world_cpp"
(gdb) p argv[1]
$3 = 0x7fffffffe797 "-c"
(gdb) p argv[2]
$4 = 0x7fffffffe79a "/etc/ceph/ceph.conf"

### 119             uint64_t size = 2 << 20;
(gdb) p size
$7 = 2097152

### gdb跳出循环方法   until NUM
### gdb跳出函数方法   finish
(gdb) until 149
main (argc=3, argv=0x7fffffffe528) at hello_world.cc:149

```





