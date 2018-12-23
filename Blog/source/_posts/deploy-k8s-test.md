---
title: deploy k8s test
date: 2018-11-24 17:09:30
tags: k8s
---

部署kubernetes有多种方式：

```
1、不借助任何工具，从基本的CLI操作开始部署k8s集群，可以学习k8s各个组件
	https://github.com/kelseyhightower/kubernetes-the-hard-way
2、部署单节点k8s集群，适合快速学习
	https://github.com/kubernetes/minikube
3、部署多节点k8s集群
	https://github.com/kubernetes/kubeadm
4、在cloud上部署k8s集群
	https://github.com/kubernetes/kops
5、coreos的工具，大于十个节点收费，小于等于十个节点免费
	https://coreos.com/tectonic/
6、实验环境
	https://labs.play-with-k8s.com/
```

# 安装minikube

mac

```
1、安装kubectl，文档https://kubernetes.io/docs/tasks/tools/install-kubectl/
yujiangdeMBP-13:~ yujiang$ brew install kubernetes-cli

2、安装minikube，文档https://github.com/kubernetes/minikube/releases
0.32.0版本在minikube start时遇到了Starting cluster components失败，在网上查找资料需要降级0.25.2。如果想使用新版本，还是找台虚拟机试一下。
yujiangdeMBP-13:~ yujiang$ curl -Lo minikube https://storage.googleapis.com/minikube/releases/v0.25.2/minikube-darwin-amd64 && chmod +x minikube && sudo cp minikube /usr/local/bin/ && rm minikube

如果已经安装了0.32.0版本的minikube，降级方法请看链接：https://coding.m.imooc.com/questiondetail.html?qid=57784

3、安装virtualbox

4、启动minikube单节点k8s(https://www.jianshu.com/p/a7620f73c7f3)
yujiangdeMBP-13:~ yujiang$ minikube start 
Starting local Kubernetes v1.9.4 cluster...
Starting VM...
Getting VM IP address...
Moving files into cluster...
Setting up certs...
Connecting to cluster...
Setting up kubeconfig...
Starting cluster components...
Kubectl is now configured to use the cluster.
Loading cached images from config file.

=====================================以下为测试，有坑，不需要执行。
yujiangdeMBP-13:~ yujiang$ rm -rf ~/.minikube/machines/minikube/
yujiangdeMBP-13:~ yujiang$ minikube start
yujiangdeMBP-13:~ yujiang$ minikube start --kubernetes-version v1.8.0 --bootstrapper kubeadm
=====================================以上为测试，有坑，不需要执行。

minikube start命令创建一个名为“minikube”的“kubectl context”。这个context包含与Minikube群集通信的配置。Minikube会自动将此context设置为默认值，但如果您以后需要切换回它，请运行：kubectl config use-context minikube,或者传递每个命令的context，如下所示：kubectl get pods --context=minikube。可以使用不同的context连接不同的k8s集群。

查看当前config清空
yujiangdeMBP-13:~ yujiang$ kubectl config view
apiVersion: v1
clusters:
- cluster:
    certificate-authority: /Users/yujiang/.minikube/ca.crt
    server: https://192.168.99.101:8443
  name: minikube
contexts:
- context:
    cluster: minikube
    user: minikube
  name: minikube
current-context: minikube
kind: Config
preferences: {}
users:
- name: minikube
  user:
    client-certificate: /Users/yujiang/.minikube/client.crt
    client-key: /Users/yujiang/.minikube/client.key

查看当前contexts
yujiangdeMBP-13:~ yujiang$ kubectl config get-contexts
CURRENT   NAME       CLUSTER    AUTHINFO   NAMESPACE
*         minikube   minikube   minikube   

查看当前k8s集群
yujiangdeMBP-13:~ yujiang$ kubectl cluster-info

进入minikube虚拟机
yujiangdeMBP-13:~ yujiang$ minikube ssh
```





# kubeadm

```

  kubeadm join 192.168.56.201:6443 --token mzbbw0.5z6zpgauylva58na --discovery-token-ca-cert-hash sha256:932a24f271bd2c14cb41d6698e9781c1fcede848b7b9fe4a0731b9c54f275df2


systemctl disable firewalld.service

systemctl stop firewalld.service

setenforce 0


cat <<EOF >  /etc/sysctl.d/k8s.conf

net.bridge.bridge-nf-call-ip6tables = 1

net.bridge.bridge-nf-call-iptables = 1

EOF

sysctl --system







yum install -y yum-utils device-mapper-persistent-data lvm2

yum-config-manager --add-repo http://mirrors.aliyun.com/docker-ce/linux/centos/docker-ce.repo

yum makecache fast



yum install -y https://download.docker.com/linux/centos/7/x86_64/stable/Packages/docker-ce-selinux-17.03.1.ce-1.el7.centos.noarch.rpm

yum install -y docker-ce-<17.03.1.ce-1.el7.centos> 



systemctl enable docker.service

systemctl start docker.service





cat <<EOF > /etc/yum.repos.d/kubernetes.repo

[kubernetes]

name=Kubernetes

baseurl=http://mirrors.aliyun.com/kubernetes/yum/repos/kubernetes-el7-x86_64

enabled=1

gpgcheck=0

repo_gpgcheck=0

gpgkey=http://mirrors.aliyun.com/kubernetes/yum/doc/yum-key.gpg

        http://mirrors.aliyun.com/kubernetes/yum/doc/rpm-package-key.gpg

EOF



yum install -y kubelet-1.11.0-0 kubeadm-1.11.0-0 kubectl-1.11.0-0

systemctl enable kubelet.service

systemctl start kubelet.service





vi /etc/sysconfig/kubelet

KUBELET_EXTRA_ARGS="--cgroup-driver=cgroupfs --pod-infra-container-image=registry.cn-hangzhou.aliyuncs.com/google_containers/pause-amd64:3.1"



systemctl daemon-reload

systemctl restart kubelet.service



vi kubeadm-master.config

apiVersion: kubeadm.k8s.io/v1alpha2

kind: MasterConfiguration

kubernetesVersion: v1.11.0

imageRepository: registry.cn-hangzhou.aliyuncs.com/google_containers

api:

  advertiseAddress: <10.0.90.217>



controllerManagerExtraArgs:

  node-monitor-grace-period: 10s

  pod-eviction-timeout: 10s



networking:

  podSubnet: <10.211.0.0>/16

  serviceSubnet: <10.96.0.0>/16



kubeProxy:

  config:

    mode: iptables





kubeadm config images pull --config kubeadm-master.config



docker pull quay.io/calico/typha:v0.7.4

docker pull quay.io/calico/node:v3.1.3

docker pull quay.io/calico/cni:v3.1.3



kubeadm init --config kubeadm-master.config

kubeadm reset

swapoff -a

kubectl get pod --all-namespaces -o wide

```

















