---
title: Package flag
date: 2018-04-27 22:27:39
tags: Golang
---

# Package flag

import "flag"

Package flag实现了command-line flag解析

### Usage:

定义flags使用flag.String(), Bool(), Int()等。

以下声明了一个整型flag，名字是-flagname，存储在指针ip中，类型为*int。

```
import "flag"
var ip = flag.Int("flagname", 1234, "help message for flagname")
```

如果你喜欢，你可以使用Var()函数将flag绑定到一个变量。

```
var flagvar int
func init() {
	flag.IntVar(&flagvar, "flagname", 1234, "help message for flagname")
}
```

或者，您可以创建满足Value接口（指针接收器）的自定义flags

```
flag.Var(&flagVal, "name", "help message for flagname")
```

对于这样的flags，默认值是变量的初始值。

所有flags定义后，调用：

```
flag.Parse()
```

将命令行解析为所定义的flags。

flags可以直接使用。如果你使用flags本身，它们都是指针；如果你绑定到变量，它们就是值。

```
fmt.Println("ip has value ", *ip)
fmt.Println("flagvar has value ", flagvar)
```

解析后，flags后面的参数可作为slice，flag.Args()。或者独立的一个值，flag.Arg(i)。参数索引，从0到flag.NArg() - 1。

Command line flag语法：

```
-flag
-flag=x
-flag x  // non-boolean flags only
```

可以使用一个或两个减号；它们是等价的。最后一种形式不允许用于boolean flags

```
cmd -x *
```

其中*是一个Unix shell通配符，如果有一个名为0或false等文件时它将会更改。您必须使用-flag=false形式来关闭boolean flag。

flag解析在第一个non-flag参数之前（"-"是一个non-flag参数）或终止符“--”之后停止。

整数flags接受1234，0664，0x1234并且可能是负数。Boolean flags可能是：

```
1, 0, t, f, T, F, true, false, TRUE, FALSE, True, False
```

Duration flags接受任何有效的time.ParseDuration。

默认的一组command-line flags由顶层函数控制。FlagSet类型允许您定义独立的flags集，例如在command-line interface中实现子命令。FlagSet的方法类似于command-line flag集的顶层函数。

```
// These examples demonstrate more intricate uses of the flag package.
package main

import (
	"errors"
	"flag"
	"fmt"
	"strings"
	"time"
)

// Example 1: A single string flag called "species" with default value "gopher".
// 示例1：一个名为“species”的字符串flag，默认值为“gopher”。
var species = flag.String("species", "gopher", "the species we are studying")

// Example 2: Two flags sharing a variable, so we can have a shorthand.
// The order of initialization is undefined, so make sure both use the
// same default value. They must be set up with an init function.
// 示例2：两个flags共享一个变量，所以我们可以用简写。
// 初始化的顺序是未定义的，所以确保两者都使用相同的默认值。 它们必须用init函数来设置。
var gopherType string

func init() {
	const (
		defaultGopher = "pocket"
		usage         = "the variety of gopher"
	)
	flag.StringVar(&gopherType, "gopher_type", defaultGopher, usage)
	flag.StringVar(&gopherType, "g", defaultGopher, usage+" (shorthand)")
}

// Example 3: A user-defined flag type, a slice of durations.
type interval []time.Duration

// String is the method to format the flag's value, part of the flag.Value interface.
// The String method's output will be used in diagnostics.
func (i *interval) String() string {
	return fmt.Sprint(*i)
}

// Set is the method to set the flag value, part of the flag.Value interface.
// Set's argument is a string to be parsed to set the flag.
// It's a comma-separated list, so we split it.
func (i *interval) Set(value string) error {
	// If we wanted to allow the flag to be set multiple times,
	// accumulating values, we would delete this if statement.
	// That would permit usages such as
	//	-deltaT 10s -deltaT 15s
	// and other combinations.
	if len(*i) > 0 {
		return errors.New("interval flag already set")
	}
	for _, dt := range strings.Split(value, ",") {
		duration, err := time.ParseDuration(dt)
		if err != nil {
			return err
		}
		*i = append(*i, duration)
	}
	return nil
}

// Define a flag to accumulate durations. Because it has a special type,
// we need to use the Var function and therefore create the flag during
// init.

var intervalFlag interval

func init() {
	// Tie the command-line flag to the intervalFlag variable and
	// set a usage message.
	flag.Var(&intervalFlag, "deltaT", "comma-separated list of intervals to use between events")
}

func main() {
	// All the interesting pieces are with the variables declared above, but
	// to enable the flag package to see the flags defined there, one must
	// execute, typically at the start of main (not init!):
	//	flag.Parse()
	// We don't run it here because this is not a main function and
	// the testing suite has already parsed the flags.
}
```







原文：https://golang.org/pkg/flag/