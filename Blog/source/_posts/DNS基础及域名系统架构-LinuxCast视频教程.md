---
title: 'DNS基础及域名系统架构 [LinuxCast视频教程]'
date: 2019-01-04 00:21:54
tags: DNS
---

域名和IP地址是一一对应的，方便我们记忆。

![](https://github.com/lnsyyj/lnsyyj.github.io/blob/hexo/Blog/source/_posts/DNS%E5%9F%BA%E7%A1%80%E5%8F%8A%E5%9F%9F%E5%90%8D%E7%B3%BB%E7%BB%9F%E6%9E%B6%E6%9E%84-LinuxCast%E8%A7%86%E9%A2%91%E6%95%99%E7%A8%8B/DNS-1.jpg?raw=true)

DNS服务

![](https://github.com/lnsyyj/lnsyyj.github.io/blob/hexo/Blog/source/_posts/DNS%E5%9F%BA%E7%A1%80%E5%8F%8A%E5%9F%9F%E5%90%8D%E7%B3%BB%E7%BB%9F%E6%9E%B6%E6%9E%84-LinuxCast%E8%A7%86%E9%A2%91%E6%95%99%E7%A8%8B/DNS-2.jpg?raw=true)



![](https://github.com/lnsyyj/lnsyyj.github.io/blob/hexo/Blog/source/_posts/DNS%E5%9F%BA%E7%A1%80%E5%8F%8A%E5%9F%9F%E5%90%8D%E7%B3%BB%E7%BB%9F%E6%9E%B6%E6%9E%84-LinuxCast%E8%A7%86%E9%A2%91%E6%95%99%E7%A8%8B/DNS-3.jpg?raw=true)



![](https://github.com/lnsyyj/lnsyyj.github.io/blob/hexo/Blog/source/_posts/DNS%E5%9F%BA%E7%A1%80%E5%8F%8A%E5%9F%9F%E5%90%8D%E7%B3%BB%E7%BB%9F%E6%9E%B6%E6%9E%84-LinuxCast%E8%A7%86%E9%A2%91%E6%95%99%E7%A8%8B/DNS-4.jpg?raw=true)



![](https://github.com/lnsyyj/lnsyyj.github.io/blob/hexo/Blog/source/_posts/DNS%E5%9F%BA%E7%A1%80%E5%8F%8A%E5%9F%9F%E5%90%8D%E7%B3%BB%E7%BB%9F%E6%9E%B6%E6%9E%84-LinuxCast%E8%A7%86%E9%A2%91%E6%95%99%E7%A8%8B/DNS-5.jpg?raw=true)



![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/Blog/source/_posts/DNS%E5%9F%BA%E7%A1%80%E5%8F%8A%E5%9F%9F%E5%90%8D%E7%B3%BB%E7%BB%9F%E6%9E%B6%E6%9E%84-LinuxCast%E8%A7%86%E9%A2%91%E6%95%99%E7%A8%8B/DNS-6.jpg)



![](https://github.com/lnsyyj/lnsyyj.github.io/blob/hexo/Blog/source/_posts/DNS%E5%9F%BA%E7%A1%80%E5%8F%8A%E5%9F%9F%E5%90%8D%E7%B3%BB%E7%BB%9F%E6%9E%B6%E6%9E%84-LinuxCast%E8%A7%86%E9%A2%91%E6%95%99%E7%A8%8B/DNS-6.jpg?raw=true)



![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/Blog/source/_posts/DNS%E5%9F%BA%E7%A1%80%E5%8F%8A%E5%9F%9F%E5%90%8D%E7%B3%BB%E7%BB%9F%E6%9E%B6%E6%9E%84-LinuxCast%E8%A7%86%E9%A2%91%E6%95%99%E7%A8%8B/DNS-8.jpg)



![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/Blog/source/_posts/DNS%E5%9F%BA%E7%A1%80%E5%8F%8A%E5%9F%9F%E5%90%8D%E7%B3%BB%E7%BB%9F%E6%9E%B6%E6%9E%84-LinuxCast%E8%A7%86%E9%A2%91%E6%95%99%E7%A8%8B/DNS-9.jpg)



![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/Blog/source/_posts/DNS%E5%9F%BA%E7%A1%80%E5%8F%8A%E5%9F%9F%E5%90%8D%E7%B3%BB%E7%BB%9F%E6%9E%B6%E6%9E%84-LinuxCast%E8%A7%86%E9%A2%91%E6%95%99%E7%A8%8B/DNS-10.jpg)



![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/Blog/source/_posts/DNS%E5%9F%BA%E7%A1%80%E5%8F%8A%E5%9F%9F%E5%90%8D%E7%B3%BB%E7%BB%9F%E6%9E%B6%E6%9E%84-LinuxCast%E8%A7%86%E9%A2%91%E6%95%99%E7%A8%8B/DNS-11.jpg)



![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/Blog/source/_posts/DNS%E5%9F%BA%E7%A1%80%E5%8F%8A%E5%9F%9F%E5%90%8D%E7%B3%BB%E7%BB%9F%E6%9E%B6%E6%9E%84-LinuxCast%E8%A7%86%E9%A2%91%E6%95%99%E7%A8%8B/DNS-12.jpg)



![](https://raw.githubusercontent.com/lnsyyj/lnsyyj.github.io/hexo/Blog/source/_posts/DNS%E5%9F%BA%E7%A1%80%E5%8F%8A%E5%9F%9F%E5%90%8D%E7%B3%BB%E7%BB%9F%E6%9E%B6%E6%9E%84-LinuxCast%E8%A7%86%E9%A2%91%E6%95%99%E7%A8%8B/DNS-13.jpg)



参考：

【1】https://www.youtube.com/channel/UCNxDA1Ip2e_SKsvhKddfQ1w

【2】https://zh.wikipedia.org/wiki/%E5%9F%9F%E5%90%8D















