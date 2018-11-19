---
title: CppVariadicTemplates
date: 2017-09-07 21:32:44
tags: Cpp2.0
---

数量不定的模板参数

``` c
首先确定编译器是否支持C++11
#include <iostream>
int main(int argc, char** argv) {
	std::cout << __cplusplus << std::endl;
	return 0;
}

```

VariadicTemplates特性最大的特点是可以用在函数递归（函数自己调用自己才叫recursive递归），tuple的做法