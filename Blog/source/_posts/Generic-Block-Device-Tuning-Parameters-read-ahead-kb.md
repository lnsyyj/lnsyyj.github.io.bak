---
title: Generic Block Device Tuning Parameters - read_ahead_kb
date: 2018-10-29 15:09:41
tags: Generic Block Device Tuning Parameters
---



- read_ahead_kb

定义sequential read operation期间OS可以预读的最大千字节数（kb）。因此，可能需要的信息已存在于kernel page cache中，以便进行下一次sequential read，从而提高read I/O性能。

Device mappers通常受益于高read_ahead_kb值。每个要mapped的设备128 KB是一个很好的起点，但将read_ahead_kb值增加到4-8 MB可能会提高sequential read大文件应用程序的性能。