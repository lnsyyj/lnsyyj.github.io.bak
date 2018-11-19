---
title: golang笔记一面向对象
date: 2018-09-13 21:37:21
tags: golang
---

- go语言仅支持封装，不支持继承和多态

- go语言没有class，只有struct

## 结构的创建

```
package main
import "fmt"
type treeNode struct {
	value int
	left, right *treeNode
}
func main() {
	var tree treeNode
	fmt.Println(tree)
}
===================================output===================================
{0 <nil> <nil>}
===================================output===================================

package main
import "fmt"
type treeNode struct {
	value int
	left, right *treeNode
}
func main() {
	var root treeNode

	root = treeNode{value: 3}
	root.left = &treeNode{}
	root.right = &treeNode{5, nil, nil}
	root.right.left = new(treeNode)

	nodes := []treeNode {
		{value: 3},
		{},
		{6, nil, &root},
	}

	fmt.Println(nodes)
}
===================================output===================================
[{3 <nil> <nil>} {0 <nil> <nil>} {6 <nil> 0xc420098020}]
===================================output===================================
不论地址还是结构本身，一律使用.来访问成员
```

go语言没有构造函数，但是可以自己创建工厂函数

```
package main
import "fmt"
type treeNode struct {
	value int
	left, right *treeNode
}
func createNode(value int) *treeNode {
	// 相当于在函数里建了一个局部变量，返回的地址是局部变量的地址，如果在c++中是典型的错误，在go语言中局部变量地址也是可以给别人用的
	// 这个局部变量是创建在堆上还是栈上？go语言不需要知道，有垃圾回收器
	return &treeNode{value: value}
}
func main() {
	var root treeNode

	root = treeNode{value: 3}
	root.left = &treeNode{}
	root.right = &treeNode{5, nil, nil}
	root.right.left = new(treeNode)
	root.left.right = createNode(2)

	fmt.Println(root)
}
===================================output===================================
{3 0xc42000a080 0xc42000a0a0}
===================================output===================================
树
      3
   /     \
  0       5
   \     / 
    2   0
```

为结构定义方法

- 显示定义，并为方法接收者命名
- 只有使用指针才可以改变结构内容，否则是值传递（是拷贝）
- nil指针也可以调用方法

```
package main
import (
	"fmt"
)
type treeNode struct {
	value int
	left, right *treeNode
}
// 给结构定义方法，(node treeNode)就相当于c++的this指针，叫做接收者。就相当于print（函数名）是给node接收的
// node treeNode是传值的还是传引用的？当然是传值的
func (node treeNode) print() {
	fmt.Printf("%d ", node.value)
}
// 与print其实是一样的
func print1(node treeNode) {
	fmt.Println(node.value)
}
// node treeNode参数实际上是传值的
func (node treeNode) setValue(value int) {
	node.value = value
}
// 传指针
func (node *treeNode) setValue1(value int) {
	node.value = value
}
// nil
func (node *treeNode) setValue2(value int) {
	if node == nil {
		fmt.Println("Setting value to nil node. Ignored.")
		return
	}
	node.value = value
}
// 树的中序遍历
func (node *treeNode) traverse() {
	if node == nil {
		return
	}
	node.left.traverse()
	node.print()
	node.right.traverse()
}
func createNode(value int) *treeNode {
	// 相当于在函数里建了一个局部变量，返回的地址是局部变量的地址，如果在c++中是典型的错误，在go语言中局部变量地址也是可以给别人用的
	return &treeNode{value: value}
}
func main() {
	var root treeNode

	root = treeNode{value: 3}
	root.left = &treeNode{}
	root.right = &treeNode{5, nil, nil}
	root.right.left = new(treeNode)
	root.left.right = createNode(2)

	// 使用为结构定义的方法
	root.print()
	fmt.Println()
	print1(root)
	fmt.Println()
	// 参数实际上是传值的，改不掉
	root.right.left.setValue(4)
	root.right.left.print()
	fmt.Println()
	// 传指针
	root.right.left.setValue1(4)
	root.right.left.print()
	fmt.Println()

	root.print()
	root.setValue1(100)

	pRoot := &root
	pRoot.print()
	pRoot.setValue1(200)
	pRoot.print()
	fmt.Println()

	// nil
	var pRoot2 *treeNode
	pRoot2.setValue2(200)
	pRoot2 = &root
	pRoot2.setValue2(300)
	pRoot2.print()
	fmt.Println()

	root.traverse()
}
===================================output===================================
3 
3

0 
4 
3 100 200 
Setting value to nil node. Ignored.
300 
0 2 300 4 5 
===================================output===================================
```

值接收者VS指针接收者

- 要改变内容必须使用指针接收者
- 结构过大也考虑使用指针接收者
- 一致性：如果指针接收者，最好都是指针接收者
- 值接受者是go语言特有
- 值/指针接收者均可以接收 值/指针，并不会改变调用者如何调用

## 封装

- 名字一般使用CamelCase
- 首字母大写：public
- 首字母小写：private

以上都是针对`包`来说的

## 包

- 每个目录一个包（包名不一定和目录名一样），每个目录只能放一个包
- main包包含可执行入口（目录中如果有一个main函数，这个目录下只能有一个main包）
- 为结构定义的方法必须放在同一个包内，可以是不同的文件

```
yujiangdeMBP-13:~ yujiang$ tree go/src/github.com/lnsyyj/GolangGrammar/tree/
go/src/github.com/lnsyyj/GolangGrammar/tree/
├── entry
│   └── entry.go
├── node.go
└── traversal.go

1 directory, 3 files



yujiangdeMBP-13:~ yujiang$ cat go/src/github.com/lnsyyj/GolangGrammar/tree/node.go 
package tree
import (
	"fmt"
)
type Node struct {
	Value int
	Left, Right *Node
}
// 给结构定义方法，(node Node)就相当于c++的this指针，叫做接收者。就相当于print（函数名）是给node接收的
// node Node是传值的还是传引用的？当然是传值的
func (node Node) Print() {
	fmt.Printf("%d ", node.Value)
}
// 与print其实是一样的
func Print1(node Node) {
	fmt.Println(node.Value)
}
// node Node参数实际上是传值的
func (node Node) SetValue(value int) {
	node.Value = value
}
// 传指针
func (node *Node) SetValue1(value int) {
	node.Value = value
}
// nil
func (node *Node) SetValue2(value int) {
	if node == nil {
		fmt.Println("Setting value to nil node. Ignored.")
		return
	}
	node.Value = value
}
func CreateNode(value int) *Node {
	// 相当于在函数里建了一个局部变量，返回的地址是局部变量的地址，如果在c++中是典型的错误，在go语言中局部变量地址也是可以给别人用的
	return &Node{Value: value}
}



yujiangdeMBP-13:~ yujiang$ cat go/src/github.com/lnsyyj/GolangGrammar/tree/traversal.go 
package tree
// 树的中序遍历
func (node *Node) Traverse() {
	if node == nil {
		return
	}
	node.Left.Traverse()
	node.Print()
	node.Right.Traverse()
}



yujiangdeMBP-13:~ yujiang$ cat go/src/github.com/lnsyyj/GolangGrammar/tree/entry/entry.go 
package main
import (
	"fmt"
	"github.com/lnsyyj/GolangGrammar/tree"
)
func main() {
	var root tree.Node
	root = tree.Node{Value: 3}
	root.Left = &tree.Node{}
	root.Right = &tree.Node{5, nil, nil}
	root.Right.Left = new(tree.Node)
	root.Left.Right = tree.CreateNode(2)
	// 使用为结构定义的方法
	root.Print()
	fmt.Println()
	tree.Print1(root)
	fmt.Println()
	// 参数实际上是传值的，改不掉
	root.Right.Left.SetValue(4)
	root.Right.Left.Print()
	fmt.Println()
	// 传指针
	root.Right.Left.SetValue1(4)
	root.Right.Left.Print()
	fmt.Println()
	root.Print()
	root.SetValue1(100)
	pRoot := &root
	pRoot.Print()
	pRoot.SetValue1(200)
	pRoot.Print()
	fmt.Println()
	// nil
	var pRoot2 *tree.Node
	pRoot2.SetValue2(200)
	pRoot2 = &root
	pRoot2.SetValue2(300)
	pRoot2.Print()
	fmt.Println()
	root.Traverse()
}
===================================output===================================
3 
3

0 
4 
3 100 200 
Setting value to nil node. Ignored.
300 
0 2 300 4 5 
===================================output===================================
```

### 如何扩充系统类型或者别人的类型

- 使用组合

```
yujiangdeMBP-13:~ yujiang$ tree go/src/github.com/lnsyyj/GolangGrammar/tree/
go/src/github.com/lnsyyj/GolangGrammar/tree/
├── entry
│   └── entry.go
├── node.go
└── traversal.go

1 directory, 3 files

yujiangdeMBP-13:~ yujiang$ cat go/src/github.com/lnsyyj/GolangGrammar/tree/node.go 
package tree
import (
	"fmt"
)
type Node struct {
	Value int
	Left, Right *Node
}
// 给结构定义方法，(node Node)就相当于c++的this指针，叫做接收者。就相当于print（函数名）是给node接收的
// node Node是传值的还是传引用的？当然是传值的
func (node Node) Print() {
	fmt.Printf("%d ", node.Value)
}
// 与print其实是一样的
func Print1(node Node) {
	fmt.Println(node.Value)
}
// node Node参数实际上是传值的
func (node Node) SetValue(value int) {
	node.Value = value
}
// 传指针
func (node *Node) SetValue1(value int) {
	node.Value = value
}
// nil
func (node *Node) SetValue2(value int) {
	if node == nil {
		fmt.Println("Setting value to nil node. Ignored.")
		return
	}
	node.Value = value
}
func CreateNode(value int) *Node {
	// 相当于在函数里建了一个局部变量，返回的地址是局部变量的地址，如果在c++中是典型的错误，在go语言中局部变量地址也是可以给别人用的
	return &Node{Value: value}
}



yujiangdeMBP-13:~ yujiang$ cat go/src/github.com/lnsyyj/GolangGrammar/tree/traversal.go 
package tree
// 树的中序遍历
func (node *Node) Traverse() {
	if node == nil {
		return
	}
	node.Left.Traverse()
	node.Print()
	node.Right.Traverse()
}



yujiangdeMBP-13:~ yujiang$ cat go/src/github.com/lnsyyj/GolangGrammar/tree/entry/entry.go 
package main
import (
	"fmt"
	"github.com/lnsyyj/GolangGrammar/tree"
)
type myTreeNode struct {
	node *tree.Node
}
func (myNode *myTreeNode) postOrder() {
	if myNode == nil || myNode.node == nil {
		return
	}
	left := myTreeNode{myNode.node.Left}
	right := myTreeNode{myNode.node.Right}
	left.postOrder()
	right.postOrder()
	myNode.node.Print()
}
func main() {
	var root tree.Node
	root = tree.Node{Value: 3}
	root.Left = &tree.Node{}
	root.Right = &tree.Node{5, nil, nil}
	root.Right.Left = new(tree.Node)
	root.Left.Right = tree.CreateNode(2)
	root.Right.Left.SetValue1(4)
	root.Traverse()
	fmt.Println()
	myRoot := myTreeNode{&root}
	myRoot.postOrder()
	fmt.Println()
}
===================================output===================================
0 2 3 4 5
2 0 4 5 3
===================================output===================================
树
      3
   /     \
  0       5
   \     / 
    2   4
```

- 定义别名


```
yujiangdeMacBook-Pro-13:~ yujiang$ tree go/src/github.com/lnsyyj/GolangGrammar/queue/
go/src/github.com/lnsyyj/GolangGrammar/queue/
├── entry
│   └── main.go
└── queue.go

1 directory, 2 files

yujiangdeMacBook-Pro-13:~ yujiang$ cat go/src/github.com/lnsyyj/GolangGrammar/queue/queue.go 
package queue
type Queue []int
func (q *Queue) Push(v int) {
	*q = append(*q, v)
}
func (q *Queue) Pop() int {
	head := (*q)[0]
	*q = (*q)[1:]
	return head
}
func (q *Queue) IsEmpty() bool {
	return len(*q) == 0
}



yujiangdeMacBook-Pro-13:~ yujiang$ cat go/src/github.com/lnsyyj/GolangGrammar/queue/entry/main.go 
package main
import (
	"github.com/lnsyyj/GolangGrammar/queue"
	"fmt"
)
func main() {
	q := queue.Queue{1}
	q.Push(2)
	q.Push(3)
	fmt.Println(q.Pop())
	fmt.Println(q.Pop())
	fmt.Println(q.IsEmpty())
	fmt.Println(q.Pop())
	fmt.Println(q.IsEmpty())
}
===================================output===================================
1
2
false
3
true
===================================output===================================
```

## GOPATH环境变量

- 默认在~/go(unix, linux)，%USERPROFILE%\go(windows)
- 官方推荐：所有项目和第三方库都放在同一个GOPATH下
- 也可以将每个项目放在不同的GOPATH下（环境变量可以配置多个GOPATH，编译时会去不同的GOPATH找到依赖的包）

```
yujiangdeMacBook-Pro-13:~ yujiang$ echo $GOPATH
/Users/yujiang/go

yujiangdeMacBook-Pro-13:~ yujiang$ cat ~/.bash_profile 
export GOPATH=/Users/yujiang/go
export GOBIN=/Users/yujiang/go/bin
export PATH=$PATH:$GOBIN
```

- GoLand自动清理import工具

```
Preferences | Tools | File Watchers
yujiangdeMBP-13:~ yujiang$ go get golang.org/x/tools/cmd/goimports
被墙了，获取不下来
```

### go get 获取第三方库

- go get命令演示
- 使用gopm来获取无法下载的包

```
yujiangdeMBP-13:~ yujiang$ go get -v github.com/gpmgo/gopm
yujiangdeMBP-13:~ yujiang$ gopm get -g -v golang.org/x/tools/cmd/goimports
yujiangdeMBP-13:~ yujiang$ ls go/src/golang.org/x/tools/imports/
fix.go         fix_test.go    imports.go     mkindex.go     mkstdlib.go    sortimports.go zstdlib.go
# 编译出goimports
yujiangdeMBP-13:~ yujiang$ go build  golang.org/x/tools/cmd/goimports
# 编译并安装到$GOPATH/bin/目录下
yujiangdeMBP-13:~ yujiang$ go install  golang.org/x/tools/cmd/goimports
```

- go build编译
- go install产生pkg文件和可执行文件
- go run直接编译运行

## GOPATH下目录结构

```
src
	git repository 1
	git repository 2
pkg
	git repository 1
	git repository 2
bin
	执行文件1，2，3
	
一般每个目录（包）下有一个main文件（package main     func main() {}）
```

