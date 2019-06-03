---
title: Ceph数据正确性与一致性守护者Scrub读书笔记
date: 2019-06-03 16:17:02
tags: Ceph
---

![]([https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/Blog/source/_posts/Ceph%E6%95%B0%E6%8D%AE%E6%AD%A3%E7%A1%AE%E6%80%A7%E4%B8%8E%E4%B8%80%E8%87%B4%E6%80%A7%E5%AE%88%E6%8A%A4%E8%80%85Scrub%E8%AF%BB%E4%B9%A6%E7%AC%94%E8%AE%B0/Scrub.png](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/Blog/source/_posts/Ceph数据正确性与一致性守护者Scrub读书笔记/Scrub.png))

临时结论：

1、目前无法避免静默错误，Scrub也无法修复严重的静默错误

2、Scrub是Ceph检测静默错误的机制

3、目前发现静默错误也没有好的方法修复（只能通知客户尽可能挽回数据）



想法：

1、借助Scrub作为测试正确性与一致性的工具（需要解决覆盖写问题）