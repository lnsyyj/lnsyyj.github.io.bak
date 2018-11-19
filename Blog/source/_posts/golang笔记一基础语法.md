---
title: golang笔记一基础语法
date: 2018-09-04 21:26:28
tags: golang
---

课程地址：https://coding.imooc.com/class/chapter/180.html

## 变量的定义

```
package main
import "fmt"
//函数外定义变量不能用:=,作用域只在包package内部
var aa = 3
var ss = "string"
//或者放在括号内
var (
	bb = 4
	cc = 5
)
//不初始化变量值
func variableZeroValue(){
	var i int
	var s string
	fmt.Printf("%d %q\n", i, s)
}
//初始化变量值
func variableInitialValue(){
	var a, b int = 3, 4
	var s string = "Hello"
	fmt.Println(a, b, s)
}
//初始化多个不同类型变量
func variableTypeDeduction(){
	var a, b, c, s = 3, 4, true, "World"
	fmt.Println(a, b, c, s)
}
//:=，省略掉var
func variableShorter(){
	a, b, c, s := 3, 4, true, "World"
	b = 5
	fmt.Println(a, b, c, s)
}

func main() {
	fmt.Println("Hello World")
	variableZeroValue()
	variableInitialValue()
	variableTypeDeduction()
	variableShorter()
	fmt.Println(aa, ss, bb, cc)
}

===================================output===================================
Hello World
0 ""
3 4 Hello
3 4 true World
3 5 true World
3 string 4 5
```

### 内建变量类型

- bool, string

- (u)int, (u)int8, (u)int16, (u)int32, (u)int64, uintptr

- byte, rune(这是字符型，golang中不叫char，是32位的)
- float32, float64, complex64, complex128

```
package main
import (
	"fmt"
	"math/cmplx"
	"math"
)

func euler(){
	fmt.Printf("%.3f\n", cmplx.Exp(1i*math.Pi) + 1)
}

func main() {
	euler()
}

===================================output===================================
(0.000+0.000i)
```

### 强制类型转换

- 类型转换是强制的（golang只有强制类型转换，没有隐式类型转换）

```
import (
	"fmt"
	"math"
)
func triangle(){
	var a, b int = 3, 4
	var c int
	c = int(math.Sqrt(float64(a * a + b * b)))
	fmt.Println(c)
}

func main() {
	triangle()
}

===================================output===================================
5
```

## 常量的定义

```
package main
import (
	"math"
	"fmt"
)
const filename = "abc.txt"
func consts(){
	//不指定类型就是一个文本
	//如果指定类型，某些地方就需要强制类型转换
	//const a, b int = 3, 4
	//c = int(math.Sqrt(float64(a * a + b * b)))
	const a, b  = 3, 4
	const (
		name = "hello world"
		d, e= 5,6
	)
	var c int
	c = int(math.Sqrt(a * a + b * b))
	fmt.Println(filename, c, name, d, e)
}

func main() {
	consts()
}

===================================output===================================
abc.txt 5 hello world 5 6
```

### 使用常量定义枚举类型

```
package main
import "fmt"
func enums(){
	const (
		cpp = 0
		java = 1
		python = 2
		golang = 3
	)
	fmt.Println(cpp, java, python, golang)
}
func main() {
	enums()
}
===================================output===================================
0 1 2 3
===================================output===================================
package main
import "fmt"
func enums(){
	const (
		cpp = iota
		java
		python
		golang
	)
	fmt.Println(cpp, java, python, golang)
}
func main() {
	enums()
}
===================================output===================================
0 1 2 3
===================================output===================================
package main
import "fmt"
func enums(){
	const (
		cpp = iota
		_
		python
		golang
		javascript
	)
	fmt.Println(cpp, javascript, python, golang)
}
func main() {
	enums()
}
===================================output===================================
0 4 2 3
===================================output===================================

package main
import "fmt"
func enums(){
	const (
		b = 1 << (10 * iota)
		kb
		mb
		gb
		tb
		pb
	)
	fmt.Println(b, kb, mb, gb, tb, pb)
}
func main() {
	enums()
}
===================================output===================================
1 1024 1048576 1073741824 1099511627776 1125899906842624
===================================output===================================
```

## 条件语句

```
package main
import (
	"io/ioutil"
	"fmt"
)
func main() {
	const filename = "abc.txt"
	contents, err := ioutil.ReadFile(filename)
	if err != nil {
		fmt.Println(err)
	}else {
		fmt.Printf("%s\n", contents)
	}
}
===================================output===================================
open abc.txt: no such file or directory
===================================output===================================
hello  world
===================================output===================================
package main
import (
	"io/ioutil"
	"fmt"
)
func main() {
	const filename = "abc.txt"
	//条件里赋值的变量作用域就在这个if语句里
	if contents, err := ioutil.ReadFile(filename); err != nil {
		fmt.Println(err)
	}else {
		fmt.Printf("%s\n", contents)
	}
}
===================================output===================================
hello  world
===================================output===================================
package main
import (
	"fmt"
)
func grade(score int) string {
	g := ""
	//不带表达式也可以进行switch
	switch {
	case score < 0 || score > 100:
		panic(fmt.Sprintf("Wrong score: %d", score))
	case score < 60:
		g = "F"
	case score < 80:
		g = "C"
	case score < 90:
		g = "B"
	case score <=100:
		g = "A"
	}
	return g
}
func main() {
	fmt.Println(
		grade(0),
		grade(59),
		grade(60),
		grade(82),
		grade(99),
		grade(100),
	)
}
===================================output===================================
F F C B A A
===================================output===================================
```

## 循环

golang中只有for，没有while

```
package main
import (
	"strconv"
	"fmt"
)
func convertToBin(n int) string {
	result := ""
	for ; n > 0; n /= 2 {
		lsb := n % 2
		result = strconv.Itoa(lsb) + result
	}
	return result
}
func main() {
	fmt.Println(
		convertToBin(5),
		convertToBin(13),
		convertToBin(789),
		convertToBin(0),
	)
}
===================================output===================================
101 1101 1100010101 
===================================output===================================

package main
import (
	"fmt"
	"os"
	"bufio"
)
func readFile(filename string) {
	if file, err := os.Open(filename); err != nil {
		panic(err)
	}else {
		scanner := bufio.NewScanner(file)
		for scanner.Scan() {
			fmt.Println(scanner.Text())
		}
	}
}
func main() {
	readFile("/Users/yujiang/go/src/github.com/lnsyyj/GolangGrammar/abc.txt")
}
===================================output===================================
hello  world
yes
===================================output===================================
//不写条件，死循环
for {
    fmt.Println("abc")
}
```

## 函数

```
package main
import "fmt"
//函数可以返回多个值
func eval(a, b int ,op string) (int, error) {
	switch op {
	case "+":
		return a + b, nil
	case "-":
		return a - b, nil
	case "*":
		return a * b, nil
	case "/":
		q , _ := div(a, b)
		return q, nil
	default:
		return 0, fmt.Errorf("unsupported operator: %s", op)
	}
}
//返回值可以起名
func div(a, b int) (q, r int){
	return a / b, a % b
}
func main() {
	if result, err := eval(3, 4, "；"); err != nil {
		fmt.Println(err)
	}else {
		fmt.Println(result)
	}
	q, r := div(13, 3)
	fmt.Println(q, r)
}
===================================output===================================
unsupported operator: ；
4 1
===================================output===================================
```

### 函数式编程

```
package main
import (
	"fmt"
	"reflect"
	"runtime"
	"math"
)
func apply(op func(int, int) int, a, b int) int {
	//拿到函数名
	p := reflect.ValueOf(op).Pointer()
	opName := runtime.FuncForPC(p).Name()
	fmt.Printf("Calling function %s with args " + "(%d, %d)\n", opName, a, b)
	return op(a, b)
}
func pow(a, b int) int {
	return int(math.Pow(float64(a), float64(b)))
}
func main() {
	fmt.Println(apply(pow, 3, 4))
	fmt.Println(apply(
		func(a int, b int) int {
			return int(math.Pow(float64(a), float64(b)))
		}, 3, 4))
}
===================================output===================================
Calling function main.pow with args (3, 4)
81
Calling function main.main.func1 with args (3, 4)
81
===================================output===================================
```

### 可变参数列表

```
package main
import (
	"fmt"
)
func sum(numbers ...int) int {
	s := 0
	for i := range numbers {
		s += numbers[i]
	}
	return s
}
func main() {
	fmt.Println(sum(1, 2, 3, 4, 5))
}
===================================output===================================
15
===================================output===================================
```

## 指针

golang指针不能运算，go语言只有值传递一种方式。没有引用传递。

```
package main
import "fmt"
func swap1(a, b int) {
	a, b = b, a
}
//指针
func swap2(a ,b *int) {
	*a, *b = *b, *a
}
func swap3(a, b int) (int, int) {
	return b, a
}
func main() {
	a, b := 3, 4
	swap1(a, b)
	fmt.Println(a, b)
	swap2(&a, &b)
	fmt.Println(a, b)
	a, b = swap3(a, b)
	fmt.Println(a, b)
}
===================================output===================================
3 4
4 3
3 4
===================================output===================================
```

## 数组

```
package main
import "fmt"
func main() {
	//数组的定义，数量写在类型的前面
	var arr1 [5]int
	arr2 := [3]int{1,3,5}
	//数组必须用...，否则是切片
	arr3 := [...]int{2,4,6,8,10}
	//二维数组
	var grid [4][5]int
	fmt.Println(arr1, arr2, arr3)
	fmt.Println(grid)

	//遍历数组
	for i, v := range arr3 {
		fmt.Println(i, v)
	}
	fmt.Println("++++++++++")
	//可通过 _ 省略变量。不仅range，任何地方都可以通过 _ 省略变量
	for _, v := range arr3 {
		fmt.Println(v)
	}
}
===================================output===================================
[0 0 0 0 0] [1 3 5] [2 4 6 8 10]
[[0 0 0 0 0] [0 0 0 0 0] [0 0 0 0 0] [0 0 0 0 0]]
0 2
1 4
2 6
3 8
4 10
++++++++++
2
4
6
8
10
===================================output===================================
```

数组是值类型

```
package main
import "fmt"
func printArray(arr [5]int) {
	arr[0] = 100
	for i, v := range arr {
		fmt.Println(i, v)
	}
}
func main() {
	//数组的定义，数量写在类型的前面
	var arr1 [5]int
	//数组必须用...，否则是切片
	arr3 := [...]int{2,4,6,8,10}
	printArray(arr1)
	fmt.Println("+++++++++++++")
	printArray(arr3)
	fmt.Println("+++++++++++++")
	fmt.Println(arr1, arr3)
}
===================================output===================================
0 100
1 0
2 0
3 0
4 0
+++++++++++++
0 100
1 4
2 6
3 8
4 10
+++++++++++++
[0 0 0 0 0] [2 4 6 8 10]
===================================output===================================
//[10]int和[20]int是不同类型，调用func f(arr [10]int)会拷贝数组

package main
import "fmt"
func printArray(arr *[5]int) {
	arr[0] = 100
	for i, v := range arr {
		fmt.Println(i, v)
	}
}
func main() {
	//数组的定义，数量写在类型的前面
	var arr1 [5]int
	//数组必须用...，否则是切片
	arr3 := [...]int{2,4,6,8,10}
	printArray(&arr1)
	fmt.Println("+++++++++++++")
	printArray(&arr3)
	fmt.Println("+++++++++++++")
	fmt.Println(arr1, arr3)
}
===================================output===================================
0 100
1 0
2 0
3 0
4 0
+++++++++++++
0 100
1 4
2 6
3 8
4 10
+++++++++++++
[100 0 0 0 0] [100 4 6 8 10]
===================================output===================================
//go语言一般不直接使用数组
```

## Slice（切片）

```
package main
import "fmt"
func main() {
	arr := [...]int{0, 1, 2, 3, 4, 5, 6, 7}
	fmt.Println("arr[2:6] = ", arr[2:6])
	fmt.Println("arr[:6] = ", arr[:6])
	fmt.Println("arr[2:] = ", arr[2:])
	fmt.Println("arr[:] = ", arr[:])
}
===================================output===================================
arr[2:6] =  [2 3 4 5]
arr[:6] =  [0 1 2 3 4 5]
arr[2:] =  [2 3 4 5 6 7]
arr[:] =  [0 1 2 3 4 5 6 7]
===================================output===================================
//文档中说slice是array的view

package main
import "fmt"
func updateSlice(s []int) {
	s[0] = 100
}
func main() {
	arr := [...]int{0, 1, 2, 3, 4, 5, 6, 7}
	fmt.Println("arr[2:6] = ", arr[2:6])
	fmt.Println("arr[:6] = ", arr[:6])
	s1 := arr[2:]
	fmt.Println("s1 = ", s1)
	s2 := arr[:]
	fmt.Println("s2 = ", s2)

	fmt.Println("After updateSlice(s1)")
	updateSlice(s1)
	fmt.Println(s1)
	fmt.Println(arr)

	fmt.Println("After updateSlice(s2)")
	updateSlice(s2)
	fmt.Println(s2)
	fmt.Println(arr)

	fmt.Println("Reslice")
	fmt.Println(s2)
	s2 = s2[:5]
	fmt.Println(s2)
	s2 = s2[2:]
	fmt.Println(s2)
}
===================================output===================================
arr[2:6] =  [2 3 4 5]
arr[:6] =  [0 1 2 3 4 5]
s1 =  [2 3 4 5 6 7]
s2 =  [0 1 2 3 4 5 6 7]
After updateSlice(s1)
[100 3 4 5 6 7]
[0 1 100 3 4 5 6 7]
After updateSlice(s2)
[100 1 100 3 4 5 6 7]
[100 1 100 3 4 5 6 7]
Reslice
[100 1 100 3 4 5 6 7]
[100 1 100 3 4]
[100 3 4]
===================================output===================================

//数组版，slice
package main
import "fmt"
func printArray(arr []int) {
	arr[0] = 100
	for i, v := range arr {
		fmt.Println(i, v)
	}
}
func main() {
	//数组的定义，数量写在类型的前面
	var arr1 [5]int
	//数组必须用...，否则是切片
	arr3 := [...]int{2,4,6,8,10}
	//**********使用arr1[:]获得数组的切片**********
	printArray(arr1[:])
	fmt.Println("+++++++++++++")
	printArray(arr3[:])
	fmt.Println("+++++++++++++")
	fmt.Println(arr1, arr3)
}
===================================output===================================
0 100
1 0
2 0
3 0
4 0
+++++++++++++
0 100
1 4
2 6
3 8
4 10
+++++++++++++
[100 0 0 0 0] [100 4 6 8 10]
===================================output===================================
//slice本身没有数据，是对底层array的一个view
//arr的值变为[0 1 10 3 4 5 6 7]
```

Slice的扩展

```
package main
import "fmt"
func main() {
	arr := [...]int{0, 1, 2, 3, 4, 5, 6, 7}

	fmt.Println("Extending slice")
	arr[0], arr[2] = 0, 2
	s1 := arr[2:6]
	s2 := s1[3:5]
	fmt.Println("s1=", s1)
	fmt.Println("s2=", s2)
}
===================================output===================================
Extending slice
s1= [2 3 4 5]
s2= [5 6]
===================================output===================================
//6不在s1中，slice是怎么取到的？slice是数组的view
//s2:=arr[3:5]              [0 1 2]    s2下标，下标2在s2中是看不到的
//s1:=arr[2:6]        [0 1 2 3 4 5]    s1下标，下标4、5在s1中是看不到的，s1[4]会报错
//arr             [0 1 2 3 4 5 6 7]    数组中数值
//slice中有ptr、len、cap，只能取值到len的值，越界会报错
//slice可以向后扩展，不可以向前扩展
//s[i]不可以超越len(s)，向后扩展不可以超越底层数组cap(s)

package main
import "fmt"
func main() {
	arr := [...]int{0, 1, 2, 3, 4, 5, 6, 7}
	fmt.Println("Extending slice")
	arr[0], arr[2] = 0, 2
	fmt.Println("arr =", arr)
	s1 := arr[2:6]
	s2 := s1[3:5]
	fmt.Printf("s1=%v, len(s1)=%d, cap(s1)=%d\n", s1, len(s1), cap(s1))
	fmt.Printf("s2=%v, len(s2)=%d, cap(s2)=%d\n", s2, len(s2), cap(s2))
	//我的理解是下标虽然取不到，但是可以扩展
	fmt.Println(s1[3:6])
}
===================================output===================================
Extending slice
arr = [0 1 2 3 4 5 6 7]
s1=[2 3 4 5], len(s1)=4, cap(s1)=6
s2=[5 6], len(s2)=2, cap(s2)=3
[5 6 7]
===================================output===================================
```

向Slice添加元素

```
package main
import "fmt"
func main() {
	arr := [...]int{0, 1, 2, 3, 4, 5, 6, 7}
	fmt.Println("Extending slice")
	arr[0], arr[2] = 0, 2
	fmt.Println("arr =", arr)
	s1 := arr[2:6]
	s2 := s1[3:5]
	fmt.Printf("s1=%v, len(s1)=%d, cap(s1)=%d\n", s1, len(s1), cap(s1))
	fmt.Printf("s2=%v, len(s2)=%d, cap(s2)=%d\n", s2, len(s2), cap(s2))
	fmt.Println(s1[3:6])

	s3 := append(s2, 10)
	s4 := append(s3, 11) //append超出arr的下标范围就不是原来的arr了，是一个新的array，新的array会设置的长一些
	s5 := append(s4, 12) //go语言会开辟一个新的array，把arr中的元素拷贝过去
	fmt.Println("s3, s4, s5 =", s3, s4, s5)
	// s4 and s5 no longer view arr.
	fmt.Println("arr =", arr)
}
===================================output===================================
Extending slice
arr = [0 1 2 3 4 5 6 7]
s1=[2 3 4 5], len(s1)=4, cap(s1)=6
s2=[5 6], len(s2)=2, cap(s2)=3
[5 6 7]
s3, s4, s5 = [5 6 10] [5 6 10 11] [5 6 10 11 12]
arr = [0 1 2 3 4 5 6 10]
===================================output===================================
//添加元素时如果超越cap，系统会重新分配更大的底层数组
//由于值传递的关系，必须接收append的返回值
//s = append(s, val)
```

Slice的创建，拷贝，删除

```
package main
import "fmt"
func printSlice(s []int) {
	fmt.Printf("%v, len=%d, cap=%d\n", s, len(s), cap(s))
}
func main() {
	fmt.Println("Creating slice")
	var s []int //Zero value for slice is nil
	for i := 0; i < 10 ; i++ {
		printSlice(s)
		s = append(s, 2 * i + 1)
	}
	fmt.Println(s)
	s1 := []int{2, 4, 6, 8}
	printSlice(s1)
	s2 := make([]int, 16)
	printSlice(s2)
	s3 := make([]int, 10, 32)
	printSlice(s3)

	fmt.Println("Copying slice")
	copy(s2, s1)
	printSlice(s2)

	fmt.Println("Deleting elements from slice")
	s2 = append(s2[:3], s2[4:]...)
	printSlice(s2)

	fmt.Println("Popping from front")
	front := s2[0]
	s2 = s2[1:]
	fmt.Println(front)
	printSlice(s2)

	fmt.Println("Popping from back")
	tail := s2[len(s2) - 1]
	s2 = s2[:len(s2) - 1]
	fmt.Println(tail)
	printSlice(s2)
}
===================================output===================================
Creating slice
[], len=0, cap=0
[1], len=1, cap=1
[1 3], len=2, cap=2
[1 3 5], len=3, cap=4
[1 3 5 7], len=4, cap=4
[1 3 5 7 9], len=5, cap=8
[1 3 5 7 9 11], len=6, cap=8
[1 3 5 7 9 11 13], len=7, cap=8
[1 3 5 7 9 11 13 15], len=8, cap=8
[1 3 5 7 9 11 13 15 17], len=9, cap=16
[1 3 5 7 9 11 13 15 17 19]
[2 4 6 8], len=4, cap=4
[0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0], len=16, cap=16
[0 0 0 0 0 0 0 0 0 0], len=10, cap=32
Copying slice
[2 4 6 8 0 0 0 0 0 0 0 0 0 0 0 0], len=16, cap=16
Deleting elements from slice
[2 4 6 0 0 0 0 0 0 0 0 0 0 0 0], len=15, cap=16
Popping from front
2
[4 6 0 0 0 0 0 0 0 0 0 0 0 0], len=14, cap=15
Popping from back
0
[4 6 0 0 0 0 0 0 0 0 0 0 0], len=13, cap=15
===================================output===================================
```

## Map

定义：map[key]value            map[key_1]map[key_2]value

- 创建：make(map[string]int)
- 获取元素：m[key]
- key不存在时，获得value类型的初始值
- 用value, ok := m[key]来判断是否存在key
- 用delete删除一个key
- 使用range遍历key，或者遍历key, value对
- 不保证遍历顺序，如需顺序，需手动对key排序
- 使用len获取元素个数

```
package main
import "fmt"
func main() {
	// map的定义
	m := map[string]string {
		"name": "ccmouse",
		"course": "golang",
		"site": "imooc",
		"quality": "notbad",
	}
	m2 := make(map[string]int)	// m2 == empty map
	var m3 map[string]int	// m3 == nil
	fmt.Println(m, m2, m3)
	// map的遍历
	fmt.Println("Traversing map")
	for k, v := range m {
		fmt.Println(k, v)
	}
	// 获取value
	fmt.Println("Getting values")
	courseName, ok := m["course"]
	fmt.Println(courseName, ok)
	// key拼错了怎么办？会打印空串
	causeName, ok := m["cause"]
	fmt.Println(causeName, ok)
	if causeName, ok := m["cause"]; ok {
		fmt.Println(causeName)
	}else {
		fmt.Println("key does not exist")
	}
	// 删除
	fmt.Println("Deleting values")
	name, ok := m["name"]
	fmt.Println(name, ok)
	delete(m, "name")
	name, ok = m["name"]
	fmt.Println(name, ok)
}
===================================output===================================
map[name:ccmouse course:golang site:imooc quality:notbad] map[] map[]
Traversing map
site imooc
quality notbad
name ccmouse
course golang
Getting values
golang true
 false
key does not exist
Deleting values
ccmouse true
 false
===================================output===================================
```

### map的key

- map使用哈希表，必须可以比较相等
- 除了slice，map，function的内建类型都可以作为key
- struct类型不包含上述字段，也可作为key

```
例：寻找最长不含有重复字符的子串
abcabcbb -> abc
bbbbb -> b
pwwkew -> wke
思路：
	对于每一个字母x
		lastOccurred[x]不存在，或者 < start   ----->   无需操作
		lastOccurred[x] >= start   ----->   更新start
		更新lastOccurred[x]，更新maxLength

package main
import "fmt"
func lengthOfNonRepeatingSubStr(s string) int {
	lastOccurred := make(map[byte]int)
	start := 0
	maxLength := 0
	for i, ch := range []byte(s) {
		if lastI, ok := lastOccurred[ch]; ok && lastI >= start {
		//if lastOccurred[ch] >= start {
			start = lastI + i
		}
		if i - start + 1 >  maxLength {
			maxLength = i - start + 1
		}
		lastOccurred[ch] = i
	}
	return  maxLength
}
func main() {
	fmt.Println(lengthOfNonRepeatingSubStr("abcabcbb"))
	fmt.Println(lengthOfNonRepeatingSubStr("bbbbb"))
	fmt.Println(lengthOfNonRepeatingSubStr("pwwkew"))
	fmt.Println(lengthOfNonRepeatingSubStr(""))
	fmt.Println(lengthOfNonRepeatingSubStr("b"))
	fmt.Println(lengthOfNonRepeatingSubStr("abcdef"))
}
===================================output===================================
3
1
3
0
1
6
===================================output===================================
```

### rune相当于go的char

- 使用range遍历pos，rune对
- 使用utf8.RuneCountInString获得字符数量
- 使用len获得字节长度
- 使用[]byte获得字节

```
package main
import (
	"fmt"
	"unicode/utf8"
)
func main() {
	s := "Yes我爱慕课网!"  //UTF-8
	fmt.Println(s)
	for _, b := range []byte(s) {
		fmt.Printf("%X ", b)
	}
	fmt.Println()
	for i, ch := range s {	// ch is a rune
		fmt.Printf("(%d %X) ", i ,ch)
	}
	fmt.Println()
	fmt.Println("Rune count:", utf8.RuneCountInString(s))
	bytes := []byte(s)
	for len(bytes) > 0 {
		ch, size := utf8.DecodeRune(bytes)
		bytes = bytes[size:]
		fmt.Printf("%c ", ch)
	}
	fmt.Println()
	for i, ch := range []rune(s) {
		fmt.Printf("(%d %c) ", i, ch)
	}
	fmt.Println()
}
===================================output===================================
Yes我爱慕课网!
59 65 73 E6 88 91 E7 88 B1 E6 85 95 E8 AF BE E7 BD 91 21 
(0 59) (1 65) (2 73) (3 6211) (6 7231) (9 6155) (12 8BFE) (15 7F51) (18 21) 
Rune count: 9
Y e s 我 爱 慕 课 网 ! 
(0 Y) (1 e) (2 s) (3 我) (4 爱) (5 慕) (6 课) (7 网) (8 !) 
===================================output===================================

package main
import (
	"fmt"
)
func lengthOfNonRepeatingSubStr(s string) int {
	lastOccurred := make(map[rune]int)
	for k, v := range lastOccurred  {
		fmt.Printf("%k=%v, v=%v ", k, v)
	}
	start := 0
	maxLength := 0
	for i, ch := range []rune(s) {
		lastI, ok := lastOccurred[ch]
		if ok && lastI >= start {
		//if lastOccurred[ch] >= start {
			start = lastI + i
		}
		if i - start + 1 >  maxLength {
			maxLength = i - start + 1
		}
		lastOccurred[ch] = i
	}
	return  maxLength
}
func main() {
	fmt.Println(lengthOfNonRepeatingSubStr("abcabcbb"))
	fmt.Println(lengthOfNonRepeatingSubStr("bbbbb"))
	fmt.Println(lengthOfNonRepeatingSubStr("pwwkew"))
	fmt.Println(lengthOfNonRepeatingSubStr(""))
	fmt.Println(lengthOfNonRepeatingSubStr("b"))
	fmt.Println(lengthOfNonRepeatingSubStr("abcdef"))
	fmt.Println(lengthOfNonRepeatingSubStr("一二三二一"))
	fmt.Println(lengthOfNonRepeatingSubStr("一二三四五六"))
}
===================================output===================================
3
1
3
0
1
6
3
6
===================================output===================================
```

### 其他字符串操作

- Fields，Split，Join
- Contains，Index
- ToLower，ToUpper
- Trim，TrimRight，TrimLeft

