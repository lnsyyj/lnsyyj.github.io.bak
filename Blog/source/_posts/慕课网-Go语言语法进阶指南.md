---
title: 慕课网-Go语言语法进阶指南
date: 2020-03-01 20:40:18
tags: Golang
---

课程地址：https://www.imooc.com/video/21088

Golang语言趋势图：https://www.tiobe.com/tiobe-index/go/

# 内建方法

在任何的包、任何的文件都可以引用这样的方法，无需导入包就可以使用。

## 常用的内建方法

### make

```
make：创建slice、map、chan （这三种类型必须使用make创建，这三种类型必须要Golang帮我们初始化好，避免在使用中出现问题）
make返回类型引用
（slice类似于数组，数组在golang中是不可变化长度的，slice是可变长度的。map是一个key、value数据结构。chan是管道。）



func main() {
	makeSlice()
	makeMap()
	makeChan()
}
// makeSlice 创建切片
func makeSlice()  {
	mSlice := make([]string, 3)
	mSlice[0] = "dog"
	mSlice[1] = "cat"
	mSlice[2] = "tiger"
	fmt.Println(mSlice)
}
// makeMap 创建map
func makeMap()  {
	mMap := make(map[int]string)
	mMap[10] = "dog"
	mMap[100] = "cat"
	fmt.Println(mMap)
}
// makeChan 创建没有缓存的chan
func makeChan() {
	mChan := make(chan int)
	close(mChan)
}

程序执行结果：
[dog cat tiger]
map[10:dog 100:cat]

```

### new

```
内存置零
返回传入类型的指针地址

func main() {
	NewMap()
}

func NewMap() {
	mNewMap := new(map[int]string)
	mMakeMap := make(map[int]string)
	fmt.Println(reflect.TypeOf(mNewMap))
	fmt.Println(reflect.TypeOf(mMakeMap))
}

程序执行结果：
*map[int]string
map[int]string
```

### append & delete & copy

```
slice -> append & copy
map -> delete



// 向切片中添加元素并打印出切片的长度和容量
func main() {
	appendElementForSlice()
}

func appendElementForSlice() {
	mIDSlice := make([]string, 2)
	mIDSlice[0] = "id-1"
	mIDSlice[1] = "id-2"
	fmt.Println("len=",len(mIDSlice))
	fmt.Println("cap=",cap(mIDSlice))
	mIDSlice = append(mIDSlice, "id-3")
	fmt.Println(mIDSlice)
	fmt.Println("After len=",len(mIDSlice))
	fmt.Println("After cap=",cap(mIDSlice))
}

程序执行结果：
len= 2
cap= 2
[id-1 id-2 id-3]
After len= 3
After cap= 4



// 拷贝切片 test 1
func main() {
	copyForSlice()
}

func copyForSlice() {
	mIDSliceDst := make([]string, 2)
	mIDSliceDst[0] = "id-dst-1"
	mIDSliceDst[1] = "id-dst-2"

	mIDSliceSrc := make([]string, 2)
	mIDSliceSrc[0] = "id-src-1"
	mIDSliceSrc[1] = "id-src-2"

	copy(mIDSliceDst, mIDSliceSrc)
	fmt.Println(mIDSliceDst)
}

程序执行结果：
[id-src-1 id-src-2]

// 拷贝切片 test 2
func main() {
	copyForSlice()
}

func copyForSlice() {
	mIDSliceDst := make([]string, 3)
	mIDSliceDst[0] = "id-dst-1"
	mIDSliceDst[1] = "id-dst-2"
	mIDSliceDst[2] = "id-dst-3"

	mIDSliceSrc := make([]string, 2)
	mIDSliceSrc[0] = "id-src-1"
	mIDSliceSrc[1] = "id-src-2"

	copy(mIDSliceDst, mIDSliceSrc)
	fmt.Println(mIDSliceDst)
}

程序执行结果：
[id-src-1 id-src-2 id-dst-3]

// 拷贝切片 test 3
func main() {
	copyForSlice()
}

// 拷贝切片
func copyForSlice() {
	mIDSliceDst := make([]string, 2)
	mIDSliceDst[0] = "id-dst-1"
	mIDSliceDst[1] = "id-dst-2"

	mIDSliceSrc := make([]string, 3)
	mIDSliceSrc[0] = "id-src-1"
	mIDSliceSrc[1] = "id-src-2"

	copy(mIDSliceDst, mIDSliceSrc)
	fmt.Println(mIDSliceDst)
}

程序执行结果：
[id-src-1 id-src-2]

说明copy操作不会为mIDSliceDst扩容

// delete test 1
func main() {
	deleteFormMap()
}

func deleteFormMap() {
	mIDMap := make(map[int]string)
	mIDMap[0] = "id-1"
	mIDMap[1] = "id-2"
	delete(mIDMap, 0)
	fmt.Println(mIDMap)
}

程序执行结果：
map[1:id-2]

```

### panic & recover

```
处理异常，panic抛出异常，recover捕获异常

// panic test 1
func main() {
	receivePanic()
}

func receivePanic() {
	panic("I am panic")
}

程序执行结果：
panic: I am panic

goroutine 1 [running]:
main.receivePanic(...)
	D:/SourceCode/GitHub/Golang/src/github.com/lnsyyj/gin_test_project/test/main.go:13
main.main()
	D:/SourceCode/GitHub/Golang/src/github.com/lnsyyj/gin_test_project/test/main.go:9 +0x40

// panic test 2
func main() {
	receivePanic()
}

func receivePanic() {
	defer func() {
		recover()
	}()
	panic("I am panic")
}

程序执行结果：
Process finished with exit code 0

// panic test 3
func main() {
	receivePanic()
}

func receivePanic() {
	defer coverPanic()
	panic("I am panic")
}

func coverPanic() {
	message := recover()
	switch message.(type) {
	case string:
		fmt.Println("string message : ", message)
	case error:
		fmt.Println("error message : ", message)
	default:
		fmt.Println("unknown panic : ", message)
	}
}
程序执行结果：
string message :  I am panic

```

### len & cap & close

```
len -> string、array、slice、map、chan
cap -> slice、array、chan
close -> chan

// len & cap test
func main() {
	getLen()
}

func getLen() {
	mIDSliceDst := make([]string, 2, 5)
	mIDSliceDst[0] = "id-dst-1"
	mIDSliceDst[1] = "id-dst-2"
	mIDSliceDst = append(mIDSliceDst, "id-dst-3")
	mIDSliceDst = append(mIDSliceDst, "id-dst-4")
	mIDSliceDst = append(mIDSliceDst, "id-dst-5")
	mIDSliceDst = append(mIDSliceDst, "id-dst-6")
	fmt.Println("mIDSliceDst len : ", len(mIDSliceDst))
	fmt.Println("mIDSliceDst cap: ", cap(mIDSliceDst))
}

程序执行结果：
mIDSliceDst len :  6
mIDSliceDst cap:  10

// close test
func main() {
	closeChan()
}

func closeChan() {
	mChan := make(chan int, 1)
	defer close(mChan)
	mChan <- 1
	// 业务代码
}
```

# 结构体

## 创建&初始化

```
func main() {
	TestForStruct()
}

type Dog struct {
	ID int
	Name string
	Age int
}

func TestForStruct() {
	// 方式1
	var dog Dog
	dog.ID = 0
	dog.Name = "KiKi"
	dog.Age = 3
	fmt.Println(dog)
	// 方式2
	dog_2 := Dog{ID: 1, Name: "Yaya", Age: 2}
	fmt.Println(dog_2)
	// 方式3
	dog_3 := new(Dog)
	dog_3.ID = 3
	dog_3.Name = "Tom"
	dog_3.Age = 4
	fmt.Println(dog_3)
}

程序执行结果：
{0 KiKi 3}
{1 Yaya 2}
&{3 Tom 4}
```

## 属性及函数

```
func main() {
	TestForStruct()
}

type Dog struct {
	ID int
	Name string
	Age int
}

func (d *Dog)Run()  {
	fmt.Println("ID : ", d.ID, " Dog is running")
}

func TestForStruct() {
	// 方式1
	var dog Dog
	dog.ID = 0
	dog.Name = "KiKi"
	dog.Age = 3
	dog.Run()
}

程序执行结果：
ID :  0  Dog is running
```

## 组合

```

```









### 