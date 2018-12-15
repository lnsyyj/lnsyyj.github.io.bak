---
title: docker-2 从零制作Base Image
date: 2018-12-14 00:20:04
tags: docker
---

学习视频地址：https://coding.imooc.com/class/189.html

# 从零制作Base Image

1、编写一个Demo程序

```
[vagrant@localhost ~]$ sudo yum install glibc-static gcc -y

[vagrant@localhost ~]$ cat helloworld.c 
#include <stdio.h>

int main() {
    printf("hello world!\n");
    return 0;
}

# 不加-static编译，docker run会报错，为什么？后续再查... ...
# standard_init_linux.go:190: exec user process caused "no such file or directory"
[vagrant@localhost ~]$ gcc -static helloworld.c -o helloworld
```

2、编写Dockerfile

```
[vagrant@localhost ~]$ cat Dockerfile 
FROM scratch
ADD helloworld /
CMD ["/helloworld"]
```

3、build镜像

```
[vagrant@localhost ~]$ docker build -t yujiang/helloworld .
Sending build context to Docker daemon  19.97kB
Step 1/3 : FROM scratch
 ---> 
Step 2/3 : ADD helloworld /
 ---> 2949199fbdb8
Step 3/3 : CMD ["/helloworld"]
 ---> Running in 3af2e910629e
Removing intermediate container 3af2e910629e
 ---> b898a6498b21
Successfully built b898a6498b21
Successfully tagged yujiang/helloworld:latest

[vagrant@localhost ~]$ docker image ls
REPOSITORY           TAG                 IMAGE ID            CREATED             SIZE
yujiang/helloworld   latest              b898a6498b21        21 seconds ago      857kB

查看分层
[vagrant@localhost ~]$ docker history yujiang/helloworld
IMAGE               CREATED              CREATED BY                                      SIZE                COMMENT
b898a6498b21        About a minute ago   /bin/sh -c #(nop)  CMD ["/helloworld"]          0B                  
2949199fbdb8        About a minute ago   /bin/sh -c #(nop) ADD file:5e0b91d4866514aa0…   857kB              

[vagrant@localhost ~]$ ll -h
total 848K
-rw-rw-r--. 1 vagrant vagrant   50 Dec 13 16:41 Dockerfile
-rwxrwxr-x. 1 vagrant vagrant 837K Dec 13 16:47 helloworld
-rw-rw-r--. 1 vagrant vagrant   79 Dec 13 16:11 helloworld.c

[vagrant@localhost ~]$ docker run yujiang/helloworld
hello world!
```

# 使用docker commit制作image

当container已经存在时，并对容器中进行了一些变化（如安装了某一个软件）。可以把已经改变的container commit一个新的image，命令如下：

```
docker container commit
```

1、启动一个容器并安装vim

```
[vagrant@localhost ~]$ docker run -it centos
[root@b2985a1be234 /]# yum install vim -y
[root@b2985a1be234 /]# exit
[vagrant@localhost ~]$ docker container ls -a
CONTAINER ID        IMAGE               COMMAND             CREATED             STATUS                          PORTS               NAMES
b2985a1be234        centos              "/bin/bash"         3 minutes ago       Exited (0) About a minute ago                       nervous_haslett
```

2、使用docker commit创建image

```
[vagrant@localhost ~]$ docker commit b2985a1be234 yujiang/centos-vim
sha256:d0a8856e664eb754f9854c0c27a92c7d01623d2087a77269fbd12ba5021e6e13
[vagrant@localhost ~]$ docker image ls
REPOSITORY            TAG                 IMAGE ID            CREATED             SIZE
yujiang/centos-vim    latest              d0a8856e664e        59 seconds ago      327MB
centos                latest              1e1148e4cc2c        7 days ago          202MB

[vagrant@localhost ~]$ docker history 1e1148e4cc2c
IMAGE               CREATED             CREATED BY                                      SIZE                COMMENT
1e1148e4cc2c        7 days ago          /bin/sh -c #(nop)  CMD ["/bin/bash"]            0B                  
<missing>           7 days ago          /bin/sh -c #(nop)  LABEL org.label-schema.sc…   0B                  
<missing>           7 days ago          /bin/sh -c #(nop) ADD file:6f877549795f4798a…   202MB               
[vagrant@localhost ~]$ docker history d0a8856e664e
IMAGE               CREATED              CREATED BY                                      SIZE                COMMENT
d0a8856e664e        About a minute ago   /bin/bash                                       126MB               
1e1148e4cc2c        7 days ago           /bin/sh -c #(nop)  CMD ["/bin/bash"]            0B                  
<missing>           7 days ago           /bin/sh -c #(nop)  LABEL org.label-schema.sc…   0B                  
<missing>           7 days ago           /bin/sh -c #(nop) ADD file:6f877549795f4798a…   202MB 
```

这样发布image是不安全的，因为其他人不知道你对镜像做了哪些修改。不提倡。

# 使用docker build制作image

1、创建centos-vim目录

```
[vagrant@localhost ~]$ mkdir centos-vim && cd centos-vim
```

2、编写Dockerfile

```
[vagrant@localhost centos-vim]$ vim Dockerfile
FROM centos
RUN yum install vim -y	
```

3、使用docker build创建image

```
[vagrant@localhost centos-vim]$ docker build -t yujiang/centos-vim .
Sending build context to Docker daemon  2.048kB
Step 1/2 : FROM centos
 ---> 1e1148e4cc2c
Step 2/2 : RUN yum install vim -y
 ---> Running in 8122262ff02a

...安装过程省略...

Complete!
Removing intermediate container 8122262ff02a
 ---> e00635baf672
Successfully built e00635baf672
Successfully tagged yujiang/centos-vim:latest



Step 1/2 直接引用centos这层image(1e1148e4cc2c)
Step 2/2 生成了一个临时的image(8122262ff02a)，在临时的image里面通过yum安装vim，安装完以后临时的image会被remove掉（Removing intermediate container 8122262ff02a），最后根据刚才临时的image commit成为一个新的image。如果build过程中报错，可以使用这个debug(docker run -it 8122262ff02a /bin/bash)

[vagrant@localhost centos-vim]$ docker image ls
REPOSITORY            TAG                 IMAGE ID            CREATED             SIZE
yujiang/centos-vim    latest              e00635baf672        6 minutes ago       327MB
centos                latest              1e1148e4cc2c        7 days ago          202MB
```

推荐使用docker build来构建image。

# Dockerfile语法

```
官方文档：https://docs.docker.com/engine/reference/builder/
```

FROM

```
FROM scratch		# 制作base image
FROM centos			# 使用base image
FROM ubuntu:14.04	
尽量使用官方的image作为base image
```

LABEL

```
LABEL maintainer="lnsyyj@xxx.com"
LABEL version="1.0"
LABEL description="Tish is description"
定义image的metadata
```

RUN

```
RUN yum update && yum install -y vim \
	python-devel # 反斜线换行
RUN apt-get update && apt-get install -y perl \
	pwgen --no-install-recommends && rm -rf \
	/var/lib/apt/lists/* # 注意清理cache
RUN /bin/bash -c 'source $HOME/.bashrc; echo $HOME'
为了美观，复杂的RUN请用反斜杠换行，避免无用分层，合并多条命令成一行。
```

WORKDIR

```
WORKDIR /root

WORKDIR /test	# 如果没有会自动创建test目录
WORKDIR demo
RUN pwd		# 输出结果应该是/test/demo
用WORKDIR，不要用RUN cd，尽量使用绝对路径！
```

ADD 和 COPY

```
ADD hello /

ADD test.tar.gz /	# 添加到根目录并解压

WORKDIR /root
ADD hello test/		# /root/test/hello

WORKDIR /root
COPY hello test/
大部分情况，COPY优于ADD！ADD除了COPY还有额外功能(解压)！添加远程文件/目录请使用curl或者wget下载到docker中！
```

ENV

```
ENV MYSQL_VERSION 5.6	# 设置常量
RUN apt-get install -y mysql-server="${MYSQL_VERSION}" \
	&& rm -rf /var/lib/apt/lists/*	# 引用常量
尽量使用ENV增加可维护性！
```

VOLUME 和 EXPOSE

```
（存储和网络），后面我们单独讲！
```

CMD 和 ENTRYPOINT

```
后面我们单独讲！
```

# RUN vs CMD vs ENTRYPOINT

```
RUN：执行命令并创建新的Image Layer
CMD：设置容器启动后默认执行的命令和参数
ENTRYPOINT：设置容器启动时运行的命令
```

## Shell和Exec格式

Shell格式

```
RUN apt-get install -y vim
CMD echo "hello docker"
ENTRYPOINT echo "hello docker"
```

Exec格式

```
RUN [ "apt-get", "install", "-y", "vim" ]
CMD [ "/bin/echo", "hello docker" ]
ENTRYPOINT [ "/bin/echo", "hello docker" ]
```

DEMO

```
Dockerfile1：
FROM centos
ENV name Docker
ENTRYPOINT echo "hello $name"

Dockerfile2：
FROM centos
ENV name Docker
ENTRYPOINT [ "/bin/echo", "hello $name" ]

Dockerfile2会原样输出"hello $name"，不会像Dockerfile1那样输出"hello Docker"。如何使Dockerfile2像Dockerfile1那样输出的？请看Dockerfile3

Dockerfile3：
FROM centos
ENV name Docker
ENTRYPOINT [ "/bin/bash", "-c", "echo hello $name" ] # 指定-c参数，后面的命令需要放在一个""中，作为一条命令
```

## CMD

- 容器启动时默认执行的命令

- 如果docker run指定了其他命令，CMD命令被忽略

- 如果定义了多个CMD，只有最后一个会执行

```
FROM centos
ENV name Docker
CMD echo "hello $name"

docker run [image]输出？打印"hello Docker"
Docker run -it [image] /bin/bash输出？不会打印"hello Docker"，/bin/bash覆盖了CMD
```

## ENTRYPOINT

- 让容器以应用程序或者服务的形式运行

- 不会被忽略，一定会执行

- 最佳实践：写一个shell脚本作为entrypoint

```
COPY docker-entrypoint.sh /usr/local/bin/
ENTRYPOINT [ "docker-entrypoint.sh" ]

EXPOSE 27017
CMD [ "mongod" ]
```

# 镜像存放

## 公有镜像

https://hub.docker.com/

```
[vagrant@localhost ~]$ docker build -t lnsyyj/helloworld .
[vagrant@localhost ~]$ docker images
lnsyyj/helloworld     latest              1a8620e6d6de        46 hours ago        857kB

[vagrant@localhost ~]$ docker login
Login with your Docker ID to push and pull images from Docker Hub. If you don't have a Docker ID, head over to https://hub.docker.com to create one.
Username: lnsyyj
Password: 
Login Succeeded

[vagrant@localhost ~]$ docker push lnsyyj/helloworld:latest
The push refers to repository [docker.io/lnsyyj/helloworld]
a9094ec14918: Pushed 
latest: digest: sha256:dd740db962a1e3a8fb74461505f539248b7c88de80b133db612c22e80d7b2d17 size: 527

删除本地镜像，测试下载镜像
[vagrant@localhost ~]$ docker rmi lnsyyj/helloworld
[vagrant@localhost ~]$ docker pull lnsyyj/helloworld
```

- 也可以link到github，github中的Repositories中有Dockerfile，dockehub会自动build镜像

## 私有镜像

搭建docker registry

```
https://hub.docker.com/_/registry

[vagrant@localhost ~]$ docker run -d -p 5000:5000 --restart always --name registry registry:2
Unable to find image 'registry:2' locally
2: Pulling from library/registry
d6a5679aa3cf: Pull complete 
ad0eac849f8f: Pull complete 
2261ba058a15: Pull complete 
f296fda86f10: Pull complete 
bcd4a541795b: Pull complete 
Digest: sha256:5a156ff125e5a12ac7fdec2b90b7e2ae5120fa249cf62248337b6d04abc574c8
Status: Downloaded newer image for registry:2
2707d472d3dba19f366c7ca51e621b83a63975492152880e01268a326d34bf50

[vagrant@localhost ~]$ docker build -t 10.0.2.15:5000/helloworld .
Sending build context to Docker daemon  873.5kB
Step 1/3 : FROM scratch
 ---> 
Step 2/3 : ADD helloworld /
 ---> Using cache
 ---> 1b468168e95e
Step 3/3 : CMD ["/helloworld"]
 ---> Using cache
 ---> 1a8620e6d6de
Successfully built 1a8620e6d6de
Successfully tagged 10.0.2.15:5000/helloworld:latest

[vagrant@localhost ~]$ cat /etc/docker/daemon.json 
{ "insecure-registries": ["10.0.2.15:5000"] }

[vagrant@localhost ~]$ sudo vim /lib/systemd/system/docker.service 
添加
EnvironmentFile=-/etc/docker/daemon.json
[vagrant@localhost ~]$ sudo systemctl daemon-reload
[vagrant@localhost ~]$ service docker restart

push到私有镜像仓库
[vagrant@localhost ~]$ docker images
10.0.2.15:5000/helloworld   latest              1a8620e6d6de        47 hours ago        857kB
[vagrant@localhost ~]$ curl http://10.0.2.15:5000/v2/_catalog
{"repositories":[]}
[vagrant@localhost ~]$ docker push 10.0.2.15:5000/helloworld
The push refers to repository [10.0.2.15:5000/helloworld]
a9094ec14918: Pushed 
latest: digest: sha256:dd740db962a1e3a8fb74461505f539248b7c88de80b133db612c22e80d7b2d17 size: 527
[vagrant@localhost ~]$ curl http://10.0.2.15:5000/v2/_catalog
{"repositories":["helloworld"]}
验证
[vagrant@localhost ~]$ docker rmi 10.0.2.15:5000/helloworld
[vagrant@localhost ~]$ docker pull 10.0.2.15:5000/helloworld
```

# container常用命令

docker exec，进入运行中的容器。

```
docker exec -it <container ID> /bin/bash
```

docker stop，停止运行中的容器。

```
docker stop <container ID>
```

docker inspect，查看容器的详细信息。

```
docker inspect <container ID>
```

docker logs，查看容器的log。

```
docker logs <container ID>
```

# linux压力测试工具stress

docker build命令行程序

```
[vagrant@localhost ~]$ mkdir stress && cd stress/
[vagrant@localhost stress]$ cat Dockerfile 
FROM ubuntu
RUN apt-get update && apt-get install -y stress
ENTRYPOINT ["/usr/bin/stress"]
CMD []
[vagrant@localhost stress]$ docker build -t lnsyyj/ubuntu-stress  .

[vagrant@localhost stress]$ docker run -it lnsyyj/ubuntu-stress --vm 1 --verbose
stress: info: [1] dispatching hogs: 0 cpu, 0 io, 1 vm, 0 hdd
stress: dbug: [1] using backoff sleep of 3000us
stress: dbug: [1] --> hogvm worker 1 [6] forked
stress: dbug: [6] allocating 268435456 bytes ...
stress: dbug: [6] touching bytes in strides of 4096 bytes ...
stress: dbug: [6] freed 268435456 bytes
stress: dbug: [6] allocating 268435456 bytes ...
stress: dbug: [6] touching bytes in strides of 4096 bytes ...
stress: dbug: [6] freed 268435456 bytes

--vm 1 --verbose是通过Dockerfile中的CMD []接收的。ENTRYPOINT+CMD这种方式是非常流行的。
```

# 对一个容器进行资源限制

```
[vagrant@localhost stress]$ docker run --memory=200M  -it lnsyyj/ubuntu-stress --vm 1 --vm-bytes 500M --verbose
stress: info: [1] dispatching hogs: 0 cpu, 0 io, 1 vm, 0 hdd
stress: dbug: [1] using backoff sleep of 3000us
stress: dbug: [1] --> hogvm worker 1 [6] forked
stress: dbug: [6] allocating 524288000 bytes ...
stress: dbug: [6] touching bytes in strides of 4096 bytes ...
stress: FAIL: [1] (415) <-- worker 6 got signal 9
stress: WARN: [1] (417) now reaping child worker processes
stress: FAIL: [1] (421) kill error: No such process
stress: FAIL: [1] (451) failed run completed in 1s
```

# docker 底层技术支持

- namespaces：做隔离pid、net、ipc、mnt、uts

- control groups：做资源限制

- union file systems：container和image的分层

