---
title: docker-3 docker network
date: 2018-12-16 01:05:49
tags: docker
---

学习视频地址：https://coding.imooc.com/class/189.html

```
单机
	Bridge Network
	Host Network
	None 
多机
	Overlay Network
```

抓包工具

```
https://www.wireshark.org
```

# Vagrant创建两台主机

```
yujiangdeMBP-13:docker-network yujiang$ cat Vagrantfile 
# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.require_version ">= 1.6.0"
boxes = [
    {
        :name => "docker-node1",
        :eth1 => "192.168.56.61",
        :mem => "1024",
        :cpu => "1"
    },
    {
        :name => "docker-node2",
        :eth1 => "192.168.56.62",
        :mem => "1024",
        :cpu => "1"
    },
]

Vagrant.configure(2) do |config|
    config.vm.box = "centos/centos7"
    boxes.each do |opts|
        config.vm.define opts[:name] do |config|
            config.vm.hostname = opts[:name]
            config.vm.provider "vmware_fusion" do |v|
                v.vmx["memsize"] = opts[:mem]
                v.vmx["numvcpus"] = opts[:cpu]
            end
    
            config.vm.provider "virtualbox" do |v|
                v.customize ["modifyvm", :id, "--memory", opts[:mem]]
                v.customize ["modifyvm", :id, "--cpus", opts[:cpu]]
            end
            config.vm.network :private_network, ip:opts[:eth1]
        end
    end
    config.vm.provision "shell", inline: <<-SHELL
        sudo yum remove docker docker-client docker-client-latest docker-common docker-latest docker-latest-logrotate docker-logrotate docker-selinux docker-engine-selinux docker-engine
        sudo yum install -y yum-utils device-mapper-persistent-data lvm2
        sudo yum-config-manager --add-repo https://download.docker.com/linux/centos/docker-ce.repo
        sudo yum install docker-ce -y
        sudo systemctl start docker
        sudo systemctl enable docker
        sudo groupadd docker
        sudo gpasswd -a vagrant docker
    SHELL

	# 问题 mount: unnown filesystem type 'vboxsf'
    #config.vm.synced_folder "./share", "/Users/yujiang/Vagrant/docker-network"
    #config.vm.provision "shell", privileged: true, path: "./setup.sh"
end
```

# linux network namespace

http://cizixs.com/2017/02/10/network-virtualization-network-namespace/

https://tonybai.com/2017/01/11/understanding-linux-network-namespace-for-docker-network/

```
[vagrant@docker-node1 ~]$ docker run -d --name test1 busybox /bin/sh -c "while true;do sleep 3600; done"
[vagrant@docker-node1 ~]$ docker ps
CONTAINER ID        IMAGE               COMMAND                  CREATED             STATUS              PORTS               NAMES
f9a344634aab        busybox             "/bin/sh -c 'while t…"   11 seconds ago      Up 10 seconds                           test1
```

查看宿主机的network namespace

```
[vagrant@docker-node1 ~]$ sudo ip netns list
[vagrant@docker-node1 ~]$ sudo ip netns add test1
[vagrant@docker-node1 ~]$ sudo ip netns add test2
[vagrant@docker-node1 ~]$ sudo ip netns list
test2
test1
[vagrant@docker-node1 ~]$ sudo ip netns exec test1 ip a
1: lo: <LOOPBACK> mtu 65536 qdisc noop state DOWN group default qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
[vagrant@docker-node1 ~]$ sudo ip netns exec test1 ip link set dev lo up
[vagrant@docker-node1 ~]$ sudo ip netns exec test1 ip a
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN group default qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
    inet 127.0.0.1/8 scope host lo
       valid_lft forever preferred_lft forever
    inet6 ::1/128 scope host 
       valid_lft forever preferred_lft forever

[vagrant@docker-node1 ~]$ sudo ip link add veth-test1 type veth peer name veth-test2
[vagrant@docker-node1 ~]$ ip link
5: veth-test2@veth-test1: <BROADCAST,MULTICAST,M-DOWN> mtu 1500 qdisc noop state DOWN mode DEFAULT group default qlen 1000
    link/ether 96:92:53:64:58:17 brd ff:ff:ff:ff:ff:ff
6: veth-test1@veth-test2: <BROADCAST,MULTICAST,M-DOWN> mtu 1500 qdisc noop state DOWN mode DEFAULT group default qlen 1000
    link/ether 3e:20:9c:b8:9c:2b brd ff:ff:ff:ff:ff:ff

[vagrant@docker-node1 ~]$ sudo ip link set veth-test1 netns test1
[vagrant@docker-node1 ~]$ sudo ip netns exec test1 ip link
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN mode DEFAULT group default qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
6: veth-test1@if5: <BROADCAST,MULTICAST> mtu 1500 qdisc noop state DOWN mode DEFAULT group default qlen 1000
    link/ether 3e:20:9c:b8:9c:2b brd ff:ff:ff:ff:ff:ff link-netnsid 0
[vagrant@docker-node1 ~]$ ip link
5: veth-test2@if6: <BROADCAST,MULTICAST> mtu 1500 qdisc noop state DOWN mode DEFAULT group default qlen 1000
    link/ether 96:92:53:64:58:17 brd ff:ff:ff:ff:ff:ff link-netnsid 0

[vagrant@docker-node1 ~]$ sudo ip link set veth-test2 netns test2
[vagrant@docker-node1 ~]$ ip link
5: veth-test2@if6也不见了
[vagrant@docker-node1 ~]$ sudo ip netns exec test2 ip link
1: lo: <LOOPBACK> mtu 65536 qdisc noop state DOWN mode DEFAULT group default qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
5: veth-test2@if6: <BROADCAST,MULTICAST> mtu 1500 qdisc noop state DOWN mode DEFAULT group default qlen 1000
    link/ether 96:92:53:64:58:17 brd ff:ff:ff:ff:ff:ff link-netnsid 0

分配IP地址
[vagrant@docker-node1 ~]$ sudo ip netns exec test1 ip addr add 192.168.1.2/24 dev veth-test1
[vagrant@docker-node1 ~]$ sudo ip netns exec test2 ip addr add 192.168.1.3/24 dev veth-test2
[vagrant@docker-node1 ~]$ sudo ip netns exec test1 ip link set dev veth-test1 up
[vagrant@docker-node1 ~]$ sudo ip netns exec test2 ip link set dev veth-test2 up
[vagrant@docker-node1 ~]$ sudo ip netns exec test1 ip a
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN group default qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
    inet 127.0.0.1/8 scope host lo
       valid_lft forever preferred_lft forever
    inet6 ::1/128 scope host 
       valid_lft forever preferred_lft forever
6: veth-test1@if5: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc noqueue state UP group default qlen 1000
    link/ether 3e:20:9c:b8:9c:2b brd ff:ff:ff:ff:ff:ff link-netnsid 1
    inet 192.168.1.2/24 scope global veth-test1
       valid_lft forever preferred_lft forever
    inet6 fe80::3c20:9cff:feb8:9c2b/64 scope link 
       valid_lft forever preferred_lft forever
[vagrant@docker-node1 ~]$ sudo ip netns exec test2 ip a
1: lo: <LOOPBACK> mtu 65536 qdisc noop state DOWN group default qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
5: veth-test2@if6: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc noqueue state UP group default qlen 1000
    link/ether 96:92:53:64:58:17 brd ff:ff:ff:ff:ff:ff link-netnsid 0
    inet 192.168.1.3/24 scope global veth-test2
       valid_lft forever preferred_lft forever
    inet6 fe80::9492:53ff:fe64:5817/64 scope link 
       valid_lft forever preferred_lft forever

互ping
[vagrant@docker-node1 ~]$ sudo ip netns exec test1 ping 192.168.1.3
PING 192.168.1.3 (192.168.1.3) 56(84) bytes of data.
64 bytes from 192.168.1.3: icmp_seq=1 ttl=64 time=0.051 ms
[vagrant@docker-node1 ~]$ sudo ip netns exec test2 ping 192.168.1.2
PING 192.168.1.2 (192.168.1.2) 56(84) bytes of data.
64 bytes from 192.168.1.2: icmp_seq=1 ttl=64 time=0.035 ms

```

# Bridge0

```
[vagrant@docker-node1 ~]$ docker network ls
NETWORK ID          NAME                DRIVER              SCOPE
36c9fe545daf        bridge              bridge              local
39385556d8cd        host                host                local
7828c2433efd        none                null                local
[vagrant@docker-node1 ~]$ docker network inspect 36c9fe545daf
[
    {
        "Name": "bridge",
        "Id": "36c9fe545daf2ab0917fce9b1a8edee5ebdd4cb375b1083439212506f3fe179c",
        "Created": "2018-12-15T19:24:36.985544361Z",
        "Scope": "local",
        "Driver": "bridge",
        "EnableIPv6": false,
        "IPAM": {
            "Driver": "default",
            "Options": null,
            "Config": [
                {
                    "Subnet": "172.17.0.0/16",
                    "Gateway": "172.17.0.1"
                }
            ]
        },
        "Internal": false,
        "Attachable": false,
        "Ingress": false,
        "ConfigFrom": {
            "Network": ""
        },
        "ConfigOnly": false,
        "Containers": { # test1这个container连到了bridge这个网络
            "f6c6d9b55defc79cf5a39feff1dde1da336f3aa29e68a7577428aefab0196f6b": {
                "Name": "test1",
                "EndpointID": "12a9fa5786c42c486e46bb058e44eb811f8d293ca4a89e1203df134c393d0254",
                "MacAddress": "02:42:ac:11:00:02",
                "IPv4Address": "172.17.0.2/16",
                "IPv6Address": ""
            }
        },
        "Options": {
            "com.docker.network.bridge.default_bridge": "true",
            "com.docker.network.bridge.enable_icc": "true",
            "com.docker.network.bridge.enable_ip_masquerade": "true",
            "com.docker.network.bridge.host_binding_ipv4": "0.0.0.0",
            "com.docker.network.bridge.name": "docker0",
            "com.docker.network.driver.mtu": "1500"
        },
        "Labels": {}
    }
]
[vagrant@docker-node1 ~]$ ip a
3: docker0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc noqueue state UP group default 
    link/ether 02:42:42:8b:4b:10 brd ff:ff:ff:ff:ff:ff
    inet 172.17.0.1/16 brd 172.17.255.255 scope global docker0
       valid_lft forever preferred_lft forever
    inet6 fe80::42:42ff:fe8b:4b10/64 scope link 
       valid_lft forever preferred_lft forever
8: veth2657408@if7: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc noqueue master docker0 state UP group default 
    link/ether a2:0b:d6:4a:77:e5 brd ff:ff:ff:ff:ff:ff link-netnsid 2
    inet6 fe80::a00b:d6ff:fe4a:77e5/64 scope link 
       valid_lft forever preferred_lft forever
[vagrant@docker-node1 ~]$ docker exec test1 ip a
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
    inet 127.0.0.1/8 scope host lo
       valid_lft forever preferred_lft forever
7: eth0@if8: <BROADCAST,MULTICAST,UP,LOWER_UP,M-DOWN> mtu 1500 qdisc noqueue 
    link/ether 02:42:ac:11:00:02 brd ff:ff:ff:ff:ff:ff
    inet 172.17.0.2/16 brd 172.17.255.255 scope global eth0
       valid_lft forever preferred_lft forever
veth2657408负责连到docker0，test1 container中有一个接口，test1 container中的eth0与外面的veth2657408是一对。
[vagrant@docker-node1 ~]$ sudo yum install bridge-utils -y
[vagrant@docker-node1 ~]$ brctl show
bridge name	bridge id		STP enabled	interfaces
docker0		8000.0242428b4b10	no		veth2657408
同一台宿主机上的container是通过docker0相互通信的。docker0通过NAT来使container访问Internet。
```

## 自建Bridge

```
[vagrant@docker-node1 ~]$ docker network create -d bridge my-bridge
7cbd844d6e4cf262eba12277a3e8b4c475a1fb6aa73b9ea312d113d6c519284a
[vagrant@docker-node1 ~]$ docker network ls
NETWORK ID          NAME                DRIVER              SCOPE
36c9fe545daf        bridge              bridge              local
39385556d8cd        host                host                local
7cbd844d6e4c        my-bridge           bridge              local
7828c2433efd        none                null                local
[vagrant@docker-node1 ~]$ brctl show
bridge name			bridge id			STP enabled		interfaces
br-7cbd844d6e4c		8000.02425dd71cb3	no				
docker0				8000.0242428b4b10	no				veth2657408
														veth7687c87
[vagrant@docker-node1 ~]$ docker run -d --name test3 --network my-bridge busybox /bin/sh -c "while true; do sleep 3600; done"
[vagrant@docker-node1 ~]$ brctl show
bridge name			bridge id			STP enabled		interfaces
br-7cbd844d6e4c		8000.02425dd71cb3	no				vetha530f6b
docker0				8000.0242428b4b10	no				veth2657408
														veth7687c87
[vagrant@docker-node1 ~]$ docker network ls
NETWORK ID          NAME                DRIVER              SCOPE
36c9fe545daf        bridge              bridge              local
39385556d8cd        host                host                local
7cbd844d6e4c        my-bridge           bridge              local
7828c2433efd        none                null                local
[vagrant@docker-node1 ~]$ docker network inspect 7cbd844d6e4c
[
    {
        "Name": "my-bridge",
        "Id": "7cbd844d6e4cf262eba12277a3e8b4c475a1fb6aa73b9ea312d113d6c519284a",
        "Created": "2018-12-16T13:17:25.765613424Z",
        "Scope": "local",
        "Driver": "bridge",
        "EnableIPv6": false,
        "IPAM": {
            "Driver": "default",
            "Options": {},
            "Config": [
                {
                    "Subnet": "172.18.0.0/16",
                    "Gateway": "172.18.0.1"
                }
            ]
        },
        "Internal": false,
        "Attachable": false,
        "Ingress": false,
        "ConfigFrom": {
            "Network": ""
        },
        "ConfigOnly": false,
        "Containers": {
            "5adef860247e056b8f8d2fab89892e10573daff5ea598482f53cefe3152700e0": {
                "Name": "test3",
                "EndpointID": "19f5d575ad3ff8602c7342f036f024e88c4b97c33a68efa206bd810128b82ddc",
                "MacAddress": "02:42:ac:12:00:02",
                "IPv4Address": "172.18.0.2/16",
                "IPv6Address": ""
            }
        },
        "Options": {},
        "Labels": {}
    }
]
连接到test2上
[vagrant@docker-node1 ~]$ docker network connect my-bridge test2
[vagrant@docker-node1 ~]$ docker network inspect 7cbd844d6e4c
[
    {
        "Name": "my-bridge",
        "Id": "7cbd844d6e4cf262eba12277a3e8b4c475a1fb6aa73b9ea312d113d6c519284a",
        "Created": "2018-12-16T13:17:25.765613424Z",
        "Scope": "local",
        "Driver": "bridge",
        "EnableIPv6": false,
        "IPAM": {
            "Driver": "default",
            "Options": {},
            "Config": [
                {
                    "Subnet": "172.18.0.0/16",
                    "Gateway": "172.18.0.1"
                }
            ]
        },
        "Internal": false,
        "Attachable": false,
        "Ingress": false,
        "ConfigFrom": {
            "Network": ""
        },
        "ConfigOnly": false,
        "Containers": {
            "4b7db28e2a457ea6102fd8f06b512041e502dcf54c6d7c362fc5e39f7a0cdf23": {
                "Name": "test2",
                "EndpointID": "cf4a07fff8a8684cd179ae747630ceeef32ebd38b2fb5eb9db4d2d35759970fb",
                "MacAddress": "02:42:ac:12:00:03",
                "IPv4Address": "172.18.0.3/16",
                "IPv6Address": ""
            },
            "5adef860247e056b8f8d2fab89892e10573daff5ea598482f53cefe3152700e0": {
                "Name": "test3",
                "EndpointID": "19f5d575ad3ff8602c7342f036f024e88c4b97c33a68efa206bd810128b82ddc",
                "MacAddress": "02:42:ac:12:00:02",
                "IPv4Address": "172.18.0.2/16",
                "IPv6Address": ""
            }
        },
        "Options": {},
        "Labels": {}
    }
]
查看默认的bridge，都有test2，说明test2 Container及连到了默认的bridge又连到了my-bridge。
[vagrant@docker-node1 ~]$ docker network ls
NETWORK ID          NAME                DRIVER              SCOPE
36c9fe545daf        bridge              bridge              local
39385556d8cd        host                host                local
7cbd844d6e4c        my-bridge           bridge              local
7828c2433efd        none                null                local
[vagrant@docker-node1 ~]$ docker network inspect 36c9fe545daf
[
    {
        "Name": "bridge",
        "Id": "36c9fe545daf2ab0917fce9b1a8edee5ebdd4cb375b1083439212506f3fe179c",
        "Created": "2018-12-15T19:24:36.985544361Z",
        "Scope": "local",
        "Driver": "bridge",
        "EnableIPv6": false,
        "IPAM": {
            "Driver": "default",
            "Options": null,
            "Config": [
                {
                    "Subnet": "172.17.0.0/16",
                    "Gateway": "172.17.0.1"
                }
            ]
        },
        "Internal": false,
        "Attachable": false,
        "Ingress": false,
        "ConfigFrom": {
            "Network": ""
        },
        "ConfigOnly": false,
        "Containers": {
            "4b7db28e2a457ea6102fd8f06b512041e502dcf54c6d7c362fc5e39f7a0cdf23": {
                "Name": "test2",
                "EndpointID": "4ba2bbe1ee63a797b528e25533fd09dda8921cf5f688d4a5a24db4efed7410e2",
                "MacAddress": "02:42:ac:11:00:03",
                "IPv4Address": "172.17.0.3/16",
                "IPv6Address": ""
            },
            "f6c6d9b55defc79cf5a39feff1dde1da336f3aa29e68a7577428aefab0196f6b": {
                "Name": "test1",
                "EndpointID": "12a9fa5786c42c486e46bb058e44eb811f8d293ca4a89e1203df134c393d0254",
                "MacAddress": "02:42:ac:11:00:02",
                "IPv4Address": "172.17.0.2/16",
                "IPv6Address": ""
            }
        },
        "Options": {
            "com.docker.network.bridge.default_bridge": "true",
            "com.docker.network.bridge.enable_icc": "true",
            "com.docker.network.bridge.enable_ip_masquerade": "true",
            "com.docker.network.bridge.host_binding_ipv4": "0.0.0.0",
            "com.docker.network.bridge.name": "docker0",
            "com.docker.network.driver.mtu": "1500"
        },
        "Labels": {}
    }
]
[vagrant@docker-node1 ~]$ docker exec test2 ip a
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
    inet 127.0.0.1/8 scope host lo
       valid_lft forever preferred_lft forever
9: eth0@if10: <BROADCAST,MULTICAST,UP,LOWER_UP,M-DOWN> mtu 1500 qdisc noqueue 
    link/ether 02:42:ac:11:00:03 brd ff:ff:ff:ff:ff:ff
    inet 172.17.0.3/16 brd 172.17.255.255 scope global eth0
       valid_lft forever preferred_lft forever
14: eth1@if15: <BROADCAST,MULTICAST,UP,LOWER_UP,M-DOWN> mtu 1500 qdisc noqueue 
    link/ether 02:42:ac:12:00:03 brd ff:ff:ff:ff:ff:ff
    inet 172.18.0.3/16 brd 172.18.255.255 scope global eth1
       valid_lft forever preferred_lft forever

[vagrant@docker-node1 ~]$ docker exec -it test3 /bin/sh
/ # ping 172.18.0.3
PING 172.18.0.3 (172.18.0.3): 56 data bytes
64 bytes from 172.18.0.3: seq=0 ttl=64 time=0.727 ms
/ # ping test2
PING test2 (172.18.0.3): 56 data bytes
64 bytes from 172.18.0.3: seq=0 ttl=64 time=0.058 ms
Container连接到用户自己创建的bridge上，默认是link好的。所以使用Container name也能通。如果连接到docker0则不是。
```

# Container之间link

```
[vagrant@docker-node1 ~]$ docker run -d --name test1 busybox /bin/sh -c "while true;do sleep 3600; done"
[vagrant@docker-node1 ~]$ sudo docker run -d --name test2 --link test1 busybox /bin/sh -c "while true; do sleep 3600; done"

[vagrant@docker-node1 ~]$ docker exec test1 ip a
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
    inet 127.0.0.1/8 scope host lo
       valid_lft forever preferred_lft forever
7: eth0@if8: <BROADCAST,MULTICAST,UP,LOWER_UP,M-DOWN> mtu 1500 qdisc noqueue 
    link/ether 02:42:ac:11:00:02 brd ff:ff:ff:ff:ff:ff
    inet 172.17.0.2/16 brd 172.17.255.255 scope global eth0
       valid_lft forever preferred_lft forever
[vagrant@docker-node1 ~]$ docker exec test2 ip a
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
    inet 127.0.0.1/8 scope host lo
       valid_lft forever preferred_lft forever
9: eth0@if10: <BROADCAST,MULTICAST,UP,LOWER_UP,M-DOWN> mtu 1500 qdisc noqueue 
    link/ether 02:42:ac:11:00:03 brd ff:ff:ff:ff:ff:ff
    inet 172.17.0.3/16 brd 172.17.255.255 scope global eth0
       valid_lft forever preferred_lft forever

[vagrant@docker-node1 ~]$ docker exec -it test2 /bin/sh
/ # ping 172.17.0.2
PING 172.17.0.2 (172.17.0.2): 56 data bytes
64 bytes from 172.17.0.2: seq=0 ttl=64 time=0.419 ms
/ # ping test1     (ping Container名字也是通的)
PING test1 (172.17.0.2): 56 data bytes
64 bytes from 172.17.0.2: seq=0 ttl=64 time=0.082 ms
```

# docker 端口映射

```
docker run --name nginx -d -p <容器中端口>:<宿主机端口> nginx
[vagrant@docker-node1 ~]$ docker run --name nginx -d -p 80:80 nginx
[vagrant@docker-node1 ~]$ docker ps
CONTAINER ID        IMAGE               COMMAND                  CREATED             STATUS              PORTS                NAMES
82d01fa9a547        nginx               "nginx -g 'daemon of…"   14 seconds ago      Up 13 seconds       0.0.0.0:80->80/tcp   nginx
[vagrant@docker-node1 ~]$ curl 127.0.0.1
<!DOCTYPE html>
<html>
<head>
<title>Welcome to nginx!</title>
<style>
    body {
        width: 35em;
        margin: 0 auto;
        font-family: Tahoma, Verdana, Arial, sans-serif;
    }
</style>
</head>
<body>
<h1>Welcome to nginx!</h1>
<p>If you see this page, the nginx web server is successfully installed and
working. Further configuration is required.</p>

<p>For online documentation and support please refer to
<a href="http://nginx.org/">nginx.org</a>.<br/>
Commercial support is available at
<a href="http://nginx.com/">nginx.com</a>.</p>

<p><em>Thank you for using nginx.</em></p>
</body>
</html>
```

# HOST与NONE网络

NONE，只能使用docker exec来访问Container。

```
[vagrant@docker-node1 ~]$ docker run -d --name test1 --network none busybox /bin/sh -c "while true;do sleep 3600;done"
005cd306dc9e51a50a9bff5a2f2a9c54297c2dfe786443787d54df1d0a10e8bb
[vagrant@docker-node1 ~]$ docker network inspect none
[
    {
        "Name": "none",
        "Id": "7828c2433efde412a4e8d4ffa9a92e121fb6391a37687fda9e2b09f8304ff12b",
        "Created": "2018-12-15T18:17:04.614441655Z",
        "Scope": "local",
        "Driver": "null",
        "EnableIPv6": false,
        "IPAM": {
            "Driver": "default",
            "Options": null,
            "Config": []
        },
        "Internal": false,
        "Attachable": false,
        "Ingress": false,
        "ConfigFrom": {
            "Network": ""
        },
        "ConfigOnly": false,
        "Containers": {
            "005cd306dc9e51a50a9bff5a2f2a9c54297c2dfe786443787d54df1d0a10e8bb": {
                "Name": "test1",
                "EndpointID": "46a077f2a809d0073a6c9251897e4dd1e47dd785c6f13f32056dfb13e32f0ded",
                "MacAddress": "",
                "IPv4Address": "",
                "IPv6Address": ""
            }
        },
        "Options": {},
        "Labels": {}
    }
]
[vagrant@docker-node1 ~]$ docker exec test1 ip a
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
    inet 127.0.0.1/8 scope host lo
       valid_lft forever preferred_lft forever
```

HOST，与宿主机IP相同。

```
[vagrant@docker-node1 ~]$ docker run -d --name test1 --network host busybox /bin/sh -c "while true;do sleep 3600;done"
0bf2514b4ce329a56491409d6470b0bc8374824be805bec6c5e387ecbb10713f
[vagrant@docker-node1 ~]$ docker network inspect host
[
    {
        "Name": "host",
        "Id": "39385556d8cda0864b8911f06c7c0b79d1858f2d16b6b31fd34e5e87cb8ac08b",
        "Created": "2018-12-15T18:17:04.625179187Z",
        "Scope": "local",
        "Driver": "host",
        "EnableIPv6": false,
        "IPAM": {
            "Driver": "default",
            "Options": null,
            "Config": []
        },
        "Internal": false,
        "Attachable": false,
        "Ingress": false,
        "ConfigFrom": {
            "Network": ""
        },
        "ConfigOnly": false,
        "Containers": {
            "0bf2514b4ce329a56491409d6470b0bc8374824be805bec6c5e387ecbb10713f": {
                "Name": "test1",
                "EndpointID": "adfa701db6a153c3fafd4b9fb4d10b35b47872afd9f7595f0b5ee7b9089d59df",
                "MacAddress": "",
                "IPv4Address": "",
                "IPv6Address": ""
            }
        },
        "Options": {},
        "Labels": {}
    }
]
[vagrant@docker-node1 ~]$ docker  exec test1 ip a
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
    inet 127.0.0.1/8 scope host lo
       valid_lft forever preferred_lft forever
    inet6 ::1/128 scope host 
       valid_lft forever preferred_lft forever
2: eth0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast qlen 1000
    link/ether 52:54:00:47:2c:0d brd ff:ff:ff:ff:ff:ff
    inet 10.0.2.15/24 brd 10.0.2.255 scope global dynamic eth0
       valid_lft 84405sec preferred_lft 84405sec
    inet6 fe80::5054:ff:fe47:2c0d/64 scope link 
       valid_lft forever preferred_lft forever
3: docker0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc noqueue 
    link/ether 02:42:20:4d:38:8a brd ff:ff:ff:ff:ff:ff
    inet 172.17.0.1/16 brd 172.17.255.255 scope global docker0
       valid_lft forever preferred_lft forever
    inet6 fe80::42:20ff:fe4d:388a/64 scope link 
       valid_lft forever preferred_lft forever
4: br-7cbd844d6e4c: <NO-CARRIER,BROADCAST,MULTICAST,UP> mtu 1500 qdisc noqueue 
    link/ether 02:42:be:6a:02:b8 brd ff:ff:ff:ff:ff:ff
    inet 172.18.0.1/16 brd 172.18.255.255 scope global br-7cbd844d6e4c
       valid_lft forever preferred_lft forever
5: eth1: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast qlen 1000
    link/ether 08:00:27:2a:89:41 brd ff:ff:ff:ff:ff:ff
    inet 192.168.56.61/24 brd 192.168.56.255 scope global eth1
       valid_lft forever preferred_lft forever
    inet6 fe80::a00:27ff:fe2a:8941/64 scope link 
       valid_lft forever preferred_lft forever
7: vethd963769@if6: <BROADCAST,MULTICAST,UP,LOWER_UP,M-DOWN> mtu 1500 qdisc noqueue master docker0 
    link/ether ee:f6:51:e6:91:e3 brd ff:ff:ff:ff:ff:ff
    inet6 fe80::ecf6:51ff:fee6:91e3/64 scope link 
       valid_lft forever preferred_lft forever
```

# 多机通信 Overlay

参考资料：https://github.com/docker/labs/blob/master/networking/concepts/06-overlay-networks.md

```
Overlay
 +
VXLAN(VXLAN，https://cizixs.com/2017/09/25/vxlan-protocol-introduction/)
 +
隧道(Ethernet、IPV4、UDP)
```

需要借用etcd，记录多机上的Container没有重复IP。

```
搭建etcd集群
[vagrant@docker-node1 ~]$ sudo yum install wget -y && wget https://github.com/etcd-io/etcd/releases/download/v3.3.10/etcd-v3.3.10-linux-amd64.tar.gz && tar zxvf etcd-v3.3.10-linux-amd64.tar.gz && cd etcd-v3.3.10-linux-amd64/
[vagrant@docker-node2 ~]$ sudo yum install wget -y && wget https://github.com/etcd-io/etcd/releases/download/v3.3.10/etcd-v3.3.10-linux-amd64.tar.gz && tar zxvf etcd-v3.3.10-linux-amd64.tar.gz && cd etcd-v3.3.10-linux-amd64/

[vagrant@docker-node1 etcd-v3.3.10-linux-amd64]$ nohup ./etcd --name docker-node1 \
--initial-advertise-peer-urls http://192.168.56.61:2380 \
--listen-peer-urls http://192.168.56.61:2380 \
--listen-client-urls http://192.168.56.61:2379,http://127.0.0.1:2379 \
--advertise-client-urls http://192.168.56.61:2379 \
--initial-cluster-token etcd-cluster \
--initial-cluster docker-node1=http://192.168.56.61:2380,docker-node2=http://192.168.56.62:2380 \
--initial-cluster-state new&
[vagrant@docker-node2 etcd-v3.3.10-linux-amd64]$ nohup ./etcd --name docker-node2 \
--initial-advertise-peer-urls http://192.168.56.62:2380 \
--listen-peer-urls http://192.168.56.62:2380 \
--listen-client-urls http://192.168.56.62:2379,http://127.0.0.1:2379 \
--advertise-client-urls http://192.168.56.62:2379 \
--initial-cluster-token etcd-cluster \
--initial-cluster docker-node1=http://192.168.56.61:2380,docker-node2=http://192.168.56.62:2380 \
--initial-cluster-state new&

[vagrant@docker-node1 etcd-v3.3.10-linux-amd64]$ ./etcdctl cluster-health
member 14192bed1b668a6 is healthy: got healthy result from http://192.168.56.61:2379
member 80c395b734da48f6 is healthy: got healthy result from http://192.168.56.62:2379
cluster is healthy
[vagrant@docker-node2 etcd-v3.3.10-linux-amd64]$ ./etcdctl cluster-health
member 14192bed1b668a6 is healthy: got healthy result from http://192.168.56.61:2379
member 80c395b734da48f6 is healthy: got healthy result from http://192.168.56.62:2379
cluster is healthy

[vagrant@docker-node1 etcd-v3.3.10-linux-amd64]$ sudo service docker stop
[vagrant@docker-node1 etcd-v3.3.10-linux-amd64]$ sudo /usr/bin/dockerd -H tcp://0.0.0.0:2375 -H unix:///var/run/docker.sock --cluster-store=etcd://192.168.56.61:2379 --cluster-advertise=192.168.56.61:2375&
[vagrant@docker-node1 etcd-v3.3.10-linux-amd64]$ sudo docker version

[vagrant@docker-node2 etcd-v3.3.10-linux-amd64]$ sudo service docker stop
[vagrant@docker-node2 etcd-v3.3.10-linux-amd64]$ sudo /usr/bin/dockerd -H tcp://0.0.0.0:2375 -H unix:///var/run/docker.sock --cluster-store=etcd://192.168.56.62:2379 --cluster-advertise=192.168.56.62:2375&
[vagrant@docker-node2 etcd-v3.3.10-linux-amd64]$ sudo docker version

[vagrant@docker-node1 ~]$ sudo docker network ls
NETWORK ID          NAME                DRIVER              SCOPE
6a9dcf7a7d0a        bridge              bridge              local
39385556d8cd        host                host                local
7828c2433efd        none                null                local
[vagrant@docker-node2 ~]$ sudo docker network ls
NETWORK ID          NAME                DRIVER              SCOPE
e93dec4d4089        bridge              bridge              local
199f9936292d        host                host                local
4a55973792c6        none                null                local
在docker-node1机器上创建overlay网络，名字为demo。会自动同步到docker-node2机器上，这个就是etcd做的。
[vagrant@docker-node1 ~]$ sudo docker network create -d overlay demo
d0cf99b69499e384b52854e852b0c734ebae3f039fdacd762e5347de18454fda
[vagrant@docker-node1 ~]$ sudo docker network ls
NETWORK ID          NAME                DRIVER              SCOPE
6a9dcf7a7d0a        bridge              bridge              local
d0cf99b69499        demo                overlay             global
39385556d8cd        host                host                local
7828c2433efd        none                null                local
[vagrant@docker-node2 ~]$ sudo docker network ls
NETWORK ID          NAME                DRIVER              SCOPE
e93dec4d4089        bridge              bridge              local
d0cf99b69499        demo                overlay             global
199f9936292d        host                host                local
4a55973792c6        none                null                local
[vagrant@docker-node1 etcd-v3.3.10-linux-amd64]$ ./etcdctl ls /docker/network/v1.0/network
/docker/network/v1.0/network/d0cf99b69499e384b52854e852b0c734ebae3f039fdacd762e5347de18454fda
[vagrant@docker-node1 ~]$ docker network inspect demo
[
    {
        "Name": "demo",
        "Id": "d0cf99b69499e384b52854e852b0c734ebae3f039fdacd762e5347de18454fda",
        "Created": "2018-12-17T15:00:39.903457425Z",
        "Scope": "global",
        "Driver": "overlay",
        "EnableIPv6": false,
        "IPAM": {
            "Driver": "default",
            "Options": {},
            "Config": [
                {
                    "Subnet": "10.0.0.0/24",
                    "Gateway": "10.0.0.1"
                }
            ]
        },
        "Internal": false,
        "Attachable": false,
        "Ingress": false,
        "ConfigFrom": {
            "Network": ""
        },
        "ConfigOnly": false,
        "Containers": {},
        "Options": {},
        "Labels": {}
    }
]

创建Container测试，在不同的机器上创建相同名字的Container会有报错，当在第二个机器上创建test1 Container时，会去etcd中查找。
[vagrant@docker-node1 ~]$ sudo docker run -d --name test1 --net demo busybox sh -c "while true; do sleep 3600; done"
fabfd6aab82b692d0d59ba5ca61d0315fced0fa459510ebe1d17fe8eacd71882
[vagrant@docker-node2 ~]$ sudo docker run -d --name test1 --net demo busybox sh -c "while true; do sleep 3600; done"
docker: Error response from daemon: Conflict. The container name "/test1" is already in use by container "74d40e80850ca8b95f6c43b7c78f755811e71892116428b1108a1f62553dbf7f". You have to remove (or rename) that container to be able to reuse that name.
See 'docker run --help'.
[vagrant@docker-node2 ~]$ sudo docker run -d --name test2 --net demo busybox sh -c "while true; do sleep 3600; done"
09ab0bd6131b7bebbadf7183a4ab6179fd4fea2fe1322c38437e2043bf7b0b43
再次查看网络信息
[vagrant@docker-node1 ~]$ docker network inspect demo
[
    {
        "Name": "demo",
        "Id": "d0cf99b69499e384b52854e852b0c734ebae3f039fdacd762e5347de18454fda",
        "Created": "2018-12-17T15:00:39.903457425Z",
        "Scope": "global",
        "Driver": "overlay",
        "EnableIPv6": false,
        "IPAM": {
            "Driver": "default",
            "Options": {},
            "Config": [
                {
                    "Subnet": "10.0.0.0/24",
                    "Gateway": "10.0.0.1"
                }
            ]
        },
        "Internal": false,
        "Attachable": false,
        "Ingress": false,
        "ConfigFrom": {
            "Network": ""
        },
        "ConfigOnly": false,
        "Containers": {
            "ep-596086a2ecffec95d85529df2e24016ac62c8448466f60454e61567482faf02f": {
                "Name": "test2",
                "EndpointID": "596086a2ecffec95d85529df2e24016ac62c8448466f60454e61567482faf02f",
                "MacAddress": "02:42:0a:00:00:03",
                "IPv4Address": "10.0.0.3/24",
                "IPv6Address": ""
            },
            "fabfd6aab82b692d0d59ba5ca61d0315fced0fa459510ebe1d17fe8eacd71882": {
                "Name": "test1",
                "EndpointID": "36027fb8100351e24db8e5691a70420413a5e02d6f397af3794cb2eabdc71fb8",
                "MacAddress": "02:42:0a:00:00:02",
                "IPv4Address": "10.0.0.2/24",
                "IPv6Address": ""
            }
        },
        "Options": {},
        "Labels": {}
    }
]
[vagrant@docker-node1 ~]$ docker exec test1 ip a
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
    inet 127.0.0.1/8 scope host lo
       valid_lft forever preferred_lft forever
12: eth0@if13: <BROADCAST,MULTICAST,UP,LOWER_UP,M-DOWN> mtu 1450 qdisc noqueue 
    link/ether 02:42:0a:00:00:02 brd ff:ff:ff:ff:ff:ff
    inet 10.0.0.2/24 brd 10.0.0.255 scope global eth0
       valid_lft forever preferred_lft forever
15: eth1@if16: <BROADCAST,MULTICAST,UP,LOWER_UP,M-DOWN> mtu 1500 qdisc noqueue 
    link/ether 02:42:ac:13:00:02 brd ff:ff:ff:ff:ff:ff
    inet 172.19.0.2/16 brd 172.19.255.255 scope global eth1
       valid_lft forever preferred_lft forever
[vagrant@docker-node2 ~]$ docker exec test2 ip a
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
    inet 127.0.0.1/8 scope host lo
       valid_lft forever preferred_lft forever
7: eth0@if8: <BROADCAST,MULTICAST,UP,LOWER_UP,M-DOWN> mtu 1450 qdisc noqueue 
    link/ether 02:42:0a:00:00:03 brd ff:ff:ff:ff:ff:ff
    inet 10.0.0.3/24 brd 10.0.0.255 scope global eth0
       valid_lft forever preferred_lft forever
10: eth1@if11: <BROADCAST,MULTICAST,UP,LOWER_UP,M-DOWN> mtu 1500 qdisc noqueue 
    link/ether 02:42:ac:12:00:02 brd ff:ff:ff:ff:ff:ff
    inet 172.18.0.2/16 brd 172.18.255.255 scope global eth1
       valid_lft forever preferred_lft forever
每个Container中两个接口，这时docker network会多出一个docker_gwbridge，Container内其中一个接口连接的就是docker_gwbridge。
[vagrant@docker-node1 ~]$ docker network ls
NETWORK ID          NAME                DRIVER              SCOPE
d0cf99b69499        demo                overlay             global
c43108631ed7        docker_gwbridge     bridge              local

[vagrant@docker-node1 ~]$ docker exec test1 ping 10.0.0.3
PING 10.0.0.3 (10.0.0.3): 56 data bytes
64 bytes from 10.0.0.3: seq=0 ttl=64 time=10.259 ms
[vagrant@docker-node1 ~]$ docker exec test1 ping test2
PING test2 (10.0.0.3): 56 data bytes
64 bytes from 10.0.0.3: seq=0 ttl=64 time=5.188 ms

[vagrant@docker-node2 ~]$ docker exec test2 ping 10.0.0.2
PING 10.0.0.2 (10.0.0.2): 56 data bytes
64 bytes from 10.0.0.2: seq=0 ttl=64 time=8.995 ms
[vagrant@docker-node2 ~]$ docker exec test2 ping test1
PING test1 (10.0.0.2): 56 data bytes
64 bytes from 10.0.0.2: seq=0 ttl=64 time=5.485 ms
```

