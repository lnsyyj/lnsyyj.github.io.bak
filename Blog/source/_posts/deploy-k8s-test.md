---
title: deploy k8s test
date: 2018-11-24 17:09:30
tags: k8s
---



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

















