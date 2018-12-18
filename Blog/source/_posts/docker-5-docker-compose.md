---
title: docker-5 docker compose
date: 2018-12-18 23:25:37
tags: docker
---

# 搭建wordpress

```
[vagrant@docker-node1 ~]$ docker run -d --name mysql -v mysql-data:/var/lib/mysql -e MYSQL_ROOT_PASSWORD=root -e MYSQL_DATABASE=workpress mysql:5.7.24
[vagrant@docker-node1 ~]$ docker run -d --name wordpress -e WORDPRESS_DB_HOST=mysql:3306 --link mysql -p 80:80 wordpress

docker-compose.yml示例
version: '3'
services:
    wordpress:
        image: wordpress
        ports: 
            - 80:80
        environment:
            WORDPRESS_DB_HOST: mysql
            WORDPRESS_DB_PASSWORD: root
        networks:
            - my-bridge

    mysql:
        image: mysql:5.7.24
        environment:
            MYSQL_ROOT_PASSWORD: root
            MYSQL_DATABASE: wordpress
        volumes:
            - mysql-data:/var/lib/mysql
        networks:
            - my-bridge
volumes:
    mysql-data:
networks:
    my-bridge:
        driver: bridge
创建docker组合服务
[vagrant@docker-node1 ~]$ docker-compose -f docker-compose.yml up -d
清除环境
[vagrant@docker-node1 ~]$ docker-compose down

Dockerfile示例
[vagrant@docker-node1 ~]$ cat docker-compose-build.yml 
version: "3"
services:
    hello-docker:
        build:
            context: .
            dockerfile: Dockerfile
[vagrant@docker-node1 ~]$ cat Dockerfile 
FROM centos
RUN yum install vim -y
[vagrant@docker-node1 ~]$ docker-compose -f docker-compose-build.yml up 

Load balancing示例
lb:
	image: dockercloud/haproxy
	links:
		- webserver
	ports:
		- 80:80
	volumes:
		- /var/run/docker.sock:/var/run/docker.sock
```

# Docker Compose

如果搭建的APP有多个Container组成，部署APP的时候会非常繁琐。

1、要从Dockerfile build image或者Dockerhub拉取image

2、要创建多个Container

3、要管理这些Container（启动停止删除）

## Docker Compose   -->   批处理

- Docker Compose是一个工具

- 这个工具可以通过一个yml文件定义多容器的docker应用

- 通过一条命令就可以根据yml文件的定义去创建或者管理这多个容器

### docker-compose.yml

```
三大概念：services、networks、volumes
```

demo

```
一个service代表一个Container，这个Container可以从dockerhub的image来创建，或者自己build出来的images来创建

	service的启动类似docker run，我们可以给其指定network和volume，所以可以给service指定network和volume的引用

services:
	db:
		image: postgres:9.4
		volume:
			- "db-data:/var/lib/postgresql/data"
		networks:
			- back-tier
就像：
docker run -d --network back-tier -v db-data:/var/lib/postgresql/data postgres:9.4

service:
	worker:
		build: ./worker  #指定Docker的location
		links:
			- db
			- redis
		networks:
			- back-tier
```

# 安装docker compose

Linux

```
sudo curl -L "https://github.com/docker/compose/releases/download/1.23.1/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose

sudo chmod +x /usr/local/bin/docker-compose

docker-compose --version
```