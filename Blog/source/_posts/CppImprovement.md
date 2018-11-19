---
title: CppImprovement
date: 2017-09-07 22:50:54
tags: Cpp2.0
---

## Spaces in Template Expressions ##

The requirement to put a space between two closing template expressions has gone

``` c
vector<list<int> >; // OK in each C++ version
vector<list<int>>;  // OK since C++11 
```

## nullptr and std::nullptr_t ##
C++11 lets you use nullptr instead of 0 or NULL to specify that a pointer refers to no value (which differs from having an undefined value). This new feature especially helps to avoid mistakes that occurred when a null pointer was interpreted as an integral value. For example:
``` c
void f(int);
void f(void*);
f(0); // calls f(int)
f(NULL); // calls f(int) if NULL is 0, ambiguous otherwise
f(nullptr); // calls f(void*)
```
nullptr is a new keyword. It automatically converts into each pointer type but not to any integral type. It has type std::nullptr_t, defined in <cstddef> (see Section 5.8.1, page 161), so you can now even overload operations for the case that a null pointer is passed. Note that std::nullptr_t counts as a fundamental data type (see Section 5.4.2, page 127).

## Automatic Type Deduction with auto ##
With C++11, you can declare a variable or an object without specifying its specific type by using auto.
For example:
``` c
auto i = 42; // i has type int
double f();
auto d = f(); // d has type double
```
The type of a variable declared with auto is deduced from its initializer. Thus, an initialization is required: auto i; // ERROR: can’t dedulce the type of i Additional qualifiers are allowed. 
For example:
``` c
static auto vat = 0.19;
```
Using auto is especially useful where the type is a pretty long and/or complicated expression. 
For example:
``` c
vector<string> v;
...
auto pos = v.begin(); // pos has type vector<string>::iterator
auto l = [] (int x) -> bool { // l has the type of a lambda
..., // taking an int and returning a bool
};
```
The latter is an object, representing a lambda

## Uniform Initialization and Initializer Lists ##
Before C++11, programmers, especially novices, could easily become confused by the question of how to initialize a variable or an object. Initialization could happen with parentheses, braces, and/or assignment operators.
For this reason, C++11 introduced the concept of uniform initialization, which means that for any initialization, you can use one common syntax. This syntax uses braces, so the following is possible now:
``` C
int values[] { 1, 2, 3 };
std::vector<int> v { 2, 3, 5, 7, 11, 13, 17 };
std::vector<std::string> cities {
"Berlin", "New York", "London", "Braunschweig", "Cairo", "Cologne"
};
std::complex<double> c{4.0,3.0}; // equivalent to c(4.0,3.0)
```
其实是利用一个事实：编译器看到{t1,t2...tn}便做出一个关联至一个initializer_list<T>,它关联至一个array<T,n>。调用函数（例如ctor）时该array内的元素可被编译器分解逐一传给函数。但若函数参数是个initializer_list<T>，调用者却不能给予数个T参数然后以为它们会被自动转为一个initializer_list<T>传入）

std::vector<std::string> cities {"Berlin", "New York", "London", "Braunschweig", "Cairo", "Cologne"};，这形成一个initializer_list<string>,背后有个array<string,6>。调用vector<string>ctors时编译器找到一个vector<string>ctor接受initializer_list<string>。所有容器皆有类似于此ctor。

std::complex<double> c{4.0,3.0}; // equivalent to c(4.0,3.0)
这形成一个initializer_list<double>，背后有个array<double,2>。调用complex<double>ctor时该array内的2个元素被分解传给ctor。complex<double>并无任何ctor接受initializer_list

An initializer list forces so-called value initialization, which means that even local variables of fundamental data types, which usually have an undefined initial value, are initialized by zero (or nullptr, if it is a pointer):
``` c
int i; // i has undefined value
int j{}; // j is initialized by 0
int* p; // p has undefined value
int* q{}; // q is initialized by nullptr
```
Note, however, that narrowing initializations — those that reduce precision or where the supplied value gets modified— are not possible with braces. For example:
``` c
int x1(5.3); // OK, but OUCH: x1 becomes 5
int x2 = 5.3; // OK, but OUCH: x2 becomes 5
int x3{5.0}; // ERROR: narrowing
int x4 = {5.3}; // ERROR: narrowing
char c1{7}; // OK: even though 7 is an int, this is not narrowing
char c2{99999}; // ERROR: narrowing (if 99999 doesn’t fit into a char)
std::vector<int> v1 { 1, 2, 4, 5 }; // OK
std::vector<int> v2 { 1, 2.3, 4, 5.6 }; // ERROR: narrowing doubles to ints
```
To support the concept of initializer lists for user-defined types, C++11 provides the class template std::initializer_list<>. It can be used to support initializations by a list of values or in any other place where you want to process just a list of values. For example:
``` c
void print (std::initializer_list<int> vals)
{
  for (auto p=vals.begin(); p!=vals.end(); ++p) { // process a list of values
  std::cout << *p << "\n";
  }
}

print ({12,3,5,7,11,13,17}); // pass a list of values to print()
传给initializer_list者，一定必须也是个initializer_list(or{}形式)
```
cpp1.0(none explicit one argument ctor 才可以做隐式转换)
explicit for ctors taking one argument
cpp2.0
explicit for ctors taking more than one argument

## range-based for statement ##
``` c
for ( decl : coll ) {
  statement
}

for ( int i : { 2,3,4,5,6} ) {
  cout << i << endl;
}

vector<double> vec;
...
for ( auto elem : vec ) {
  cout << elem << endl;
}

for ( auto& elem : vec ) {
  elem *= 3;
}

```

Note that no explicit type conversions are possible when elements are initialized as decl inside the for loop. Thus, the following does not compile:
``` c
class C
{
public:
explicit C(const std::string& s); // explicit(!) type conversion from strings
...
};
std::vector<std::string> vs;
for (const C& elem : vs) { // ERROR, no conversion from string to C defined
std::cout << elem << std::endl;
}
```

## =default,=delete ##
如果你自行定义了一个ctor，那么编译器就不会再给你一个default ctor。
如果你强制加上=default，就可以重新获得并使用default ctor。


## Alias Template ( template typedef ) ##
``` c
template<typename T>
using Vec = std::vector<T,MyAlloc<T>>;   //standard vector using own allocator
Vec<int> coll;
is  equivalent to std::vector<int,MyAlloc<int>> coll;
It is not possible to partially or explicitly specialize an alias template.
```

## Type Alias (similar to typedef) ##
``` c
//type alias,identical to
//typedef void (*func)(int,int);
using func = void(*)(int,int);

//the name 'func' now denotes a pointer to function: void example(int,int){}
func fn = example;
```

## noexcept ##
``` c
void foo() noexcept;   ==>   void foo() noexcept(true);
```
declares that foo() won’t throw. If an exception is not handled locally inside foo() — thus, if foo() throws — the program is terminated, calling std::terminate(), which by default calls std::abort().

You can even specify a condition under which a function throws no exception. For example, for any type Type, the global swap() usually is defined as follows:
``` c
void swap (Type& x, Type& y) noexcept(noexcept(x.swap(y)))
{
x.swap(y);
}
```
Here, inside noexcept(...), you can specify a Boolean condition under which no exception gets thrown: Specifying noexcept without condition is a short form of specifying noexcept(true).

https://stackoverflow.com/questions/8001823/how-to-enforce-move-semantics-when-a-vector-grows

## override ##
告诉编译器，子类要override父类的成员函数，让编译器帮助检查
``` c
struce Base{
  virtual void vfunc(float){}
};

struct Derived2 : Base {
  virtual void vfunc(int) override{} //[Error] marked override,bug does not override 
  virtual void vfunc(float) override{}
};
```
## final ##
``` c
struct Base1 final{};
struct Derived1 : Base1{};//[Error] cannot derive from 'final' base 'Base1' in derived type 'Derived1'

struct Base2{
  virtual void f() final;
};
struct Derived2 : Base2{
  void f();//[Error] overriding final function 'virtual void Base2::f()'
};
```
## decltype ##
By using the new decltype keyword, you can let the compiler find out the type of an expression. This is the realization of the often requested typeof feature. However, the existing typeof implementations were inconsistent and incomplete, so C++11 introduced a new keyword. For example:
GNU C++中的typeof并不是标准库的一部分
``` c 
map<string, float> coll;
decltype(coll)::value_type elem;

这样写（before C++11）
map<string, float>::value_type elem;
```
One application of decltype is to declare return types (see below). Another is to use it in metaprogramming (see Section 5.4.1, page 125) or to pass the type of a lambda (see Section 10.3.4, page 504).

defines a type equivalent to the type of an expression

应用：
1、decltype,used to declare return types
Sometimes, the return type of a function depends on an expression processed with the arguments.
However, something like
``` c
template <typename T1, typename T2>
decltype(x+y) add(T1 x, T2 y);
```
was not possible before C++11, because the return expression uses objects not introduced or in scope yet.
But with C++11, you can alternatively declare the return type of a function behind the parameter list:
``` c
template <typename T1, typename T2>
auto add(T1 x, T2 y) -> decltype(x+y);
```
This uses the same syntax as for lambdas to declare return types
[...] (...) mutable_opt throwSpec_opt ->retType_opt {...}
2、decltype, to used in metaprogramming
3、decltype, pass the type of a lambda
面对lambda，我们手上往往只有object，没有type。要获得其type就得借助于decltype。
``` c
auto cmp = [](const Persion& p1, const Person& p2) {
	return p1.lastname() < p2.lastname() ||
		(p1.lastname() == p2.lastname() &&
		p1.firstname() < p2.firstname());
	}

set<Person, decltype(cmp)>coll(cmp);
```


















笔记来自 http://boolan.com/  侯捷C++新标准C++11/14
内容引自《The C++ standard Library A Tutorial and reference》