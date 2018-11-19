---
title: Lambdas
date: 2017-09-11 23:26:57
tags: Cpp2.0
---
C++11 introduced lambdas, allowing the definition of inline functionality, which can be used as a parameter or a local object.Lambdas change the way the C++ standard library is used.

Syntax of Lambdas
A lambda is a definition of functionality that can be defined inside statements and expressions. Thus, you can use a lambda as an inline function.
The minimal lambda function has no parameters and simply does something. For example:
``` c
[] {
std::cout << "hello lambda" << std::endl;
}
```
You can call it directly: [...](...)mutable_opt throwSpec_opt->retType_opt{...}
``` c
[] {
std::cout << "hello lambda" << std::endl;
} (); // prints ‘‘hello lambda’’
```
or pass it to objects to get called:
``` c
auto l = [] {
std::cout << "hello lambda" << std::endl;
};
...
l(); // prints ‘‘hello lambda’’
```
  
lambda introducer，capture to access nonstatic outside objects inside the lambda.static objects such as std::cout can be used
  |
[...](...)mutable_opt throwSpec_opt->retType_opt
       |
     all of them are optional,bug if one of them occurs,the parentheses for the parameters are mandatory
mutable关系到[...]中的数据是否可以被改写（objects are passed by value,but inside the function object defined by the lambda, you have write access to the passed value.）,可以取用外部变量
(...)函数的参数
retType,without any specific definition of the return type, it is deduced from the return value.
you can specify a capture to access data of outer scope that is not passed as an argument:
[=]means that the outer scope is passed to the lambda by value.
[&]means that the outer scope is passed to the lambda by reference.
``` c
Ex:
int x=0;
int y=42;
auto qqq=[x,&y]{...}	//[=,&y]to pass y by reference and all other objects by value.y是by reference并且允许使用所有objects by value
```

``` c
//The type of a lambda is an anouymous function object (or functor)
int id = 0;
//objects are passed by value, but inside the functionnn object defined by the lambda, you have write access to the passed value.
auto f = [id]()mutable {
    cout << "id: " << id << endl;
	++id; //如果没写mutable,难道不能++id;吗?
};
id = 42;
f();    //0
f();    //1
f();    //2
cout << id << endl;    //42

//等同于
class Functor {
private:
	int id;   //copy of outside id
public:
	void operator()(){
		cout << "id: " << id << endl;
		++id;    //OK
	}
};
Functor f;

//[&id]
int id = 0;
auto f = [&id](int param) {
	cout << "id: " << id << endl;
	cout << "param: " << param << endl;
	++id;
	++param;
};
id = 42;
f(7);	//id:42,param:7
f(7);	//id:43,param:7
f(7);	//id:44,param:7
cout << id << endl;	//id:45

//无mutable,[Error] increment of read-only variable 'id'
int id = 0;
auto f = [id]() {
	cout << "id: " << id << endl;
	++id;
};
id = 42;
f();
f();
f();
cout << id << endl;

//
int id = 0;
auto f = [id]()mutable {
	cout << "id: " << id << endl;
	++id;
	static int x = 5;	//可以声明变量
	int y = 6;
	return id;			//可以返回数值
};
f();
```
Here is what compiler generates for lambda's:
``` c
1、
int tobefound = 5;
auto lambda1 = [tobefound](int val){return val == tobefound;};
2、
class UnNamedLocalFunction{
private:
	int localVar;
public:
	UnNamedLocalFunction(int var):localVar(var){}
	bool operator()(int val){
		return val == localVar;
	}
};
UnNamedLocalFunction lambda2(tobefound);
bool b1 = lambda1(5);
bool b2 = lambda2(5);
```






