---
title: LTP C Test Case教程
date: 2018-06-11 23:19:03
tags: LTP
---

LTP测试可以用C或Shell脚本编写。

## Getting Started

Git-clone主LTP repository，如自述文件中所述, 并更改目录到checked-out的Git repository。我们建议安装LTP并运行Quick guide（在自述文件中）中提到的测试之一，以确保您从良好的状态开始。我们还建议克隆Linux kernel repository以供参考，本指南将涉及主线kernel 4.12内的文件和目录。

```
$ git clone git://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git
```

还有许多其他的repositories可以参考，包括GNU C library glibc和可选的C library musl。一些system calls被部分或完全实现在user land作为标准C  library的一部分。所以在这些情况下，C library是一个重要的参考。glibc是Linux最常用的C library，然而musl通常更容易理解。

实现system calls的方式因体系结构，以及内核版本和C library版本而异。要找出system calls是否真的在任何机器上访问内核，可以使用strace。这会拦截system calls并打印它们。我们将在后面的教程中使用它。

## Choose a System Call to test

我们将使用`statx()` system call来提供测试的具体示例。在撰写本文时，没有测试Linux内核版本4.11中的system call。Linux system call特定的测试主要包含在testcases/kernel/syscalls目录，但你也可以grep整个LTP repository以检查任何现有system call的用法。

找到当前未被LTP测试的system call的方法是在内核树中查看include/linux/syscalls.h。

### Find an untested System call

尝试找到一个未经测试的系统调用，它有一个手册页（即man syscall产生一个结果）。Git-clone最新的kernel man-pages repository是一个好主意。

```
$ git clone git://git.kernel.org/pub/scm/docs/man-pages/man-pages.git
```

在写这篇文章时，最新的man-pages release和repository HEAD之间的差异超过了100次commits。这代表了大约9周的变化。如果你使用的是稳定的Linux发行版，那么你的man-pages package可能已经有几年了。就像kernel一样，最好有Git repository作为参考。

## Create the test skeleton

我将调用我的测试statx01.c，在你读这个文件名的时候，可能会采用这个文件名，所以可以根据需要增加文件名中的数字，或者用练习2.1中选择的system call替换statx。（怕被占用）

```
$ mkdir testcases/kernel/syscalls/statx
$ cd testcases/kernel/syscalls/statx
$ echo statx >> .gitignore
```

接下来打开statx01.c并添加如下。确保将right notice更改为您的name/company，如有必要，更正test name和最低kernel version。我将在下面解释代码的作用。

```
/*
 * Copyright (c) 2017 Instruction Ignorer <"can't"@be.bothered.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Test statx
 *
 * All tests should start with a description of _what_ we are testing.
 * Non-trivial explanations of _how_ the code works should also go here.
 * Include relevant links, Git commit hashes and CVE numbers.
 * Inline comments should be avoided.
 */

#include "tst_test.h"

static void run(void)
{
	tst_res(TPASS, "Doing hardly anything is easy");
}

static struct tst_test test = {
	.test_all = run,
	.min_kver = "4.11",
};
```

从`#include`语句开始，我们将复制main LTP test library headers。这包括最常见的test API函数和test harness初始化代码。需要注意的是，这是一个完全普通的，独立的C程序，但缺少main()，因为它在tst_test.h中实现。

我们使用tst_test结构指定运行一部分测试代码。测试编写器可以设置各种回调，包括我们已经设置为运行（）的test.test_all。test writer可以设置各种回调, 包括test.test_all，我们已经设置run()。测试工具将在一个单独的进程（使用fork()）中执行此回调，如果在test.timeout秒后没有返回，则强制终止它。



## 原文

【1】https://github.com/linux-test-project/ltp/wiki/C-Test-Case-Tutorial