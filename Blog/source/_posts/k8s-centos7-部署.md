---
title: k8s centos7 部署
date: 2018-08-20 23:19:07
tags: k8s
---

参考文章：针对以下文章的实际操作

【1】https://juejin.im/post/5b45d4185188251ac062f27c

【2】https://blog.qikqiak.com/post/use-kubeadm-install-kubernetes-1.10/

## 虚拟机master节点和slave节点——Docker版本

```
# 所有master节点和slave节点
[root@k8s-master ~]# docker --version
Docker version 1.13.1, build dded712/1.13.1

[root@k8s-master ~]# yum install -y docker && systemctl enable docker && systemctl restart docker
[root@k8s-slave ~]# yum install -y docker && systemctl enable docker && systemctl restart docker
```

## 虚拟机master节点——安装etcd

```
[root@k8s-master ~]# hostnamectl
   Static hostname: rhel7
         Icon name: computer-vm
           Chassis: vm
        Machine ID: 5b097de8abcc4690b4cdd7ec9deefbc4
           Boot ID: 89bd04baef274f4f9e8eceb107547e5a
    Virtualization: kvm
  Operating System: CentOS Linux 7 (Core)
       CPE OS Name: cpe:/o:centos:centos:7
            Kernel: Linux 3.10.0-862.el7.x86_64
      Architecture: x86-64

[root@k8s-master ~]# yum install wget -y && wget https://github.com/coreos/etcd/releases/download/v3.3.9/etcd-v3.3.9-linux-amd64.tar.gz && tar zxvf etcd-v3.3.9-linux-amd64.tar.gz && cd etcd-v3.3.9-linux-amd64

[root@k8s-master etcd-v3.3.9-linux-amd64]# cp etcd /usr/bin/ && cp etcdctl /usr/bin/

[root@k8s-master ~]# vi /usr/lib/systemd/system/etcd.service
[Unit]
Description=Etcd Server
After=network.target
After=network-online.target
Wants=network-online.target
[Service]
Type=notify
WorkingDirectory=/var/lib/etcd/
EnvironmentFile=-/etc/etcd/etcd.conf
User=root
# set GOMAXPROCS to number of processors
ExecStart=/bin/bash -c "GOMAXPROCS=$(nproc) /usr/bin/etcd --name=\"${ETCD_NAME}\" --data-dir=\"${ETCD_DATA_DIR}\" --listen-client-urls=\"${ETCD_LISTEN_CLIENT_URLS}\""
Restart=on-failure
LimitNOFILE=65536
[Install]
WantedBy=multi-user.target

[root@k8s-master ~]# systemctl daemon-reload && systemctl start etcd && systemctl enable etcd && systemctl status etcd

使用kubeadm安装要关闭etcd，
```

## 虚拟机master节点——安装

```
# 所有master和slave节点，配置kubernetes源
cat <<EOF > /etc/yum.repos.d/kubernetes.repo
[kubernetes]
name=Kubernetes
baseurl=https://mirrors.aliyun.com/kubernetes/yum/repos/kubernetes-el7-x86_64
enabled=1
gpgcheck=1
repo_gpgcheck=1
gpgkey=https://mirrors.aliyun.com/kubernetes/yum/doc/yum-key.gpg https://mirrors.aliyun.com/kubernetes/yum/doc/rpm-package-key.gpg
EOF
[root@k8s-master ~]# yum install -y kubelet kubeadm kubectl ipvsadm

# 所有master和slave节点
[root@k8s-master ~]# iptables -P FORWARD ACCEPT

# 所有master和slave节点，配置转发相关参数
cat <<EOF >  /etc/sysctl.d/k8s.conf
net.bridge.bridge-nf-call-ip6tables = 1
net.bridge.bridge-nf-call-iptables = 1
vm.swappiness=0
EOF
[root@k8s-master ~]# sysctl --system

# 所有master和slave节点，加载ipvs相关内核模块
[root@k8s-master ~]# modprobe ip_vs
[root@k8s-master ~]# modprobe ip_vs_rr
[root@k8s-master ~]# modprobe ip_vs_wrr
[root@k8s-master ~]# modprobe ip_vs_sh
[root@k8s-master ~]# modprobe nf_conntrack_ipv4
[root@k8s-master ~]# lsmod | grep ip_vs
ip_vs_sh               12688  0 
ip_vs_wrr              12697  0 
ip_vs_rr               12600  0 
ip_vs                 141432  6 ip_vs_rr,ip_vs_sh,ip_vs_wrr
nf_conntrack          133053  6 ip_vs,nf_nat,nf_nat_ipv4,xt_conntrack,nf_nat_masquerade_ipv4,nf_conntrack_ipv4
libcrc32c              12644  4 xfs,ip_vs,nf_nat,nf_conntrack
# 或持久化
[root@k8s-master modules]# cat /etc/sysconfig/modules/k8s.modules
#！/bin/sh
/sbin/modinfo -F filename ip_vs > /dev/null 2>&1 
if [ $? -eq 0 ]; then 
    /sbin/modprobe ip_vs
fi
/sbin/modinfo -F filename ip_vs_rr > /dev/null 2>&1 
if [ $? -eq 0 ]; then 
    /sbin/modprobe ip_vs_rr
fi
/sbin/modinfo -F filename ip_vs_wrr > /dev/null 2>&1 
if [ $? -eq 0 ]; then
    /sbin/modprobe ip_vs_wrr
fi
/sbin/modinfo -F filename ip_vs_sh > /dev/null 2>&1 
if [ $? -eq 0 ]; then 
    /sbin/modprobe ip_vs_sh
fi
/sbin/modinfo -F filename nf_conntrack_ipv4 > /dev/null 2>&1 
if [ $? -eq 0 ]; then 
    /sbin/modprobe nf_conntrack_ipv4
fi
[root@k8s-master modules]# chmod +x k8s.modules
[root@k8s-master ~]# reboot
[root@k8s-master ~]# lsmod | grep ip_vs

# 所有master和slave节点，修改hosts文件
[root@k8s-master ~]# cat /etc/hosts
127.0.0.1   localhost localhost.localdomain localhost4 localhost4.localdomain4
::1         localhost localhost.localdomain localhost6 localhost6.localdomain6
192.168.56.200	k8s-master	k8s-master.localdomain
192.168.56.201	k8s-slave	k8s-slave.localdomain

# 所有master和slave节点
[root@k8s-master ~]# DOCKER_CGROUPS=$(docker info | grep 'Cgroup' | cut -d' ' -f3)
[root@k8s-master ~]# echo $DOCKER_CGROUPS
# 或持久化
[root@k8s-master ~]# vi /etc/profile
export DOCKER_CGROUPS=$(docker info | grep 'Cgroup' | cut -d' ' -f3)
[root@k8s-master ~]# source /etc/profile

# 所有master和slave节点
cat >/etc/sysconfig/kubelet<<EOF
KUBELET_EXTRA_ARGS="--cgroup-driver=$DOCKER_CGROUPS --pod-infra-container-image=registry.cn-hangzhou.aliyuncs.com/google_containers/pause-amd64:3.1"
EOF
[root@k8s-master ~]# systemctl daemon-reload && systemctl enable kubelet && systemctl start kubelet && systemctl status kubelet

# 永久关闭 注释/etc/fstab文件里swap相关的行
[root@k8s-master ~]# swapoff -a

# 
cat >kubeadm-master.config<<EOF
apiVersion: kubeadm.k8s.io/v1alpha2
kind: MasterConfiguration
kubernetesVersion: v1.11.2
imageRepository: registry.cn-hangzhou.aliyuncs.com/google_containers
api:
  advertiseAddress: 192.168.56.200

controllerManagerExtraArgs:
  node-monitor-grace-period: 10s
  pod-eviction-timeout: 10s

networking:
  podSubnet: 10.244.0.0/16
  
kubeProxy:
  config:
    # mode: ipvs
    mode: iptables
EOF

[root@k8s-master ~]# kubeadm config images pull --config kubeadm-master.config
[config/images] Pulled registry.cn-hangzhou.aliyuncs.com/google_containers/kube-apiserver-amd64:v1.11.2
[config/images] Pulled registry.cn-hangzhou.aliyuncs.com/google_containers/kube-controller-manager-amd64:v1.11.2
[config/images] Pulled registry.cn-hangzhou.aliyuncs.com/google_containers/kube-scheduler-amd64:v1.11.2
[config/images] Pulled registry.cn-hangzhou.aliyuncs.com/google_containers/kube-proxy-amd64:v1.11.2
[config/images] Pulled registry.cn-hangzhou.aliyuncs.com/google_containers/pause:3.1
[config/images] Pulled registry.cn-hangzhou.aliyuncs.com/google_containers/etcd-amd64:3.2.18
[config/images] Pulled registry.cn-hangzhou.aliyuncs.com/google_containers/coredns:1.1.3

[root@k8s-master ~]# kubeadm init --config kubeadm-master.config
[init] using Kubernetes version: v1.11.2
[preflight] running pre-flight checks
I0822 05:07:42.655181    8212 kernel_validator.go:81] Validating kernel version
I0822 05:07:42.655280    8212 kernel_validator.go:96] Validating kernel config
[preflight] Some fatal errors occurred:
	[ERROR DirAvailable--var-lib-etcd]: /var/lib/etcd is not empty
[preflight] If you know what you are doing, you can make a check non-fatal with `--ignore-preflight-errors=...`
[root@k8s-master ~]# rm -rf /var/lib/etcd/*
[root@k8s-master ~]# kubeadm init --config kubeadm-master.config
[init] using Kubernetes version: v1.11.2
[preflight] running pre-flight checks
I0822 05:08:00.364246    8239 kernel_validator.go:81] Validating kernel version
I0822 05:08:00.364505    8239 kernel_validator.go:96] Validating kernel config
[preflight/images] Pulling images required for setting up a Kubernetes cluster
[preflight/images] This might take a minute or two, depending on the speed of your internet connection
[preflight/images] You can also perform this action in beforehand using 'kubeadm config images pull'
[kubelet] Writing kubelet environment file with flags to file "/var/lib/kubelet/kubeadm-flags.env"
[kubelet] Writing kubelet configuration to file "/var/lib/kubelet/config.yaml"
[preflight] Activating the kubelet service
[certificates] Generated ca certificate and key.
[certificates] Generated apiserver certificate and key.
[certificates] apiserver serving cert is signed for DNS names [k8s-master.localdomain kubernetes kubernetes.default kubernetes.default.svc kubernetes.default.svc.cluster.local] and IPs [10.96.0.1 192.168.56.200]
[certificates] Generated apiserver-kubelet-client certificate and key.
[certificates] Generated sa key and public key.
[certificates] Generated front-proxy-ca certificate and key.
[certificates] Generated front-proxy-client certificate and key.
[certificates] Generated etcd/ca certificate and key.
[certificates] Generated etcd/server certificate and key.
[certificates] etcd/server serving cert is signed for DNS names [k8s-master.localdomain localhost] and IPs [127.0.0.1 ::1]
[certificates] Generated etcd/peer certificate and key.
[certificates] etcd/peer serving cert is signed for DNS names [k8s-master.localdomain localhost] and IPs [192.168.56.200 127.0.0.1 ::1]
[certificates] Generated etcd/healthcheck-client certificate and key.
[certificates] Generated apiserver-etcd-client certificate and key.
[certificates] valid certificates and keys now exist in "/etc/kubernetes/pki"
[kubeconfig] Wrote KubeConfig file to disk: "/etc/kubernetes/admin.conf"
[kubeconfig] Wrote KubeConfig file to disk: "/etc/kubernetes/kubelet.conf"
[kubeconfig] Wrote KubeConfig file to disk: "/etc/kubernetes/controller-manager.conf"
[kubeconfig] Wrote KubeConfig file to disk: "/etc/kubernetes/scheduler.conf"
[controlplane] wrote Static Pod manifest for component kube-apiserver to "/etc/kubernetes/manifests/kube-apiserver.yaml"
[controlplane] wrote Static Pod manifest for component kube-controller-manager to "/etc/kubernetes/manifests/kube-controller-manager.yaml"
[controlplane] wrote Static Pod manifest for component kube-scheduler to "/etc/kubernetes/manifests/kube-scheduler.yaml"
[etcd] Wrote Static Pod manifest for a local etcd instance to "/etc/kubernetes/manifests/etcd.yaml"
[init] waiting for the kubelet to boot up the control plane as Static Pods from directory "/etc/kubernetes/manifests" 
[init] this might take a minute or longer if the control plane images have to be pulled
[apiclient] All control plane components are healthy after 39.501843 seconds
[uploadconfig] storing the configuration used in ConfigMap "kubeadm-config" in the "kube-system" Namespace
[kubelet] Creating a ConfigMap "kubelet-config-1.11" in namespace kube-system with the configuration for the kubelets in the cluster
[markmaster] Marking the node k8s-master.localdomain as master by adding the label "node-role.kubernetes.io/master=''"
[markmaster] Marking the node k8s-master.localdomain as master by adding the taints [node-role.kubernetes.io/master:NoSchedule]
[patchnode] Uploading the CRI Socket information "/var/run/dockershim.sock" to the Node API object "k8s-master.localdomain" as an annotation
[bootstraptoken] using token: vyabge.9wi2cqgukrf3c2qh
[bootstraptoken] configured RBAC rules to allow Node Bootstrap tokens to post CSRs in order for nodes to get long term certificate credentials
[bootstraptoken] configured RBAC rules to allow the csrapprover controller automatically approve CSRs from a Node Bootstrap Token
[bootstraptoken] configured RBAC rules to allow certificate rotation for all node client certificates in the cluster
[bootstraptoken] creating the "cluster-info" ConfigMap in the "kube-public" namespace
[addons] Applied essential addon: CoreDNS
[addons] Applied essential addon: kube-proxy

Your Kubernetes master has initialized successfully!

To start using your cluster, you need to run the following as a regular user:

  mkdir -p $HOME/.kube
  sudo cp -i /etc/kubernetes/admin.conf $HOME/.kube/config
  sudo chown $(id -u):$(id -g) $HOME/.kube/config

You should now deploy a pod network to the cluster.
Run "kubectl apply -f [podnetwork].yaml" with one of the options listed at:
  https://kubernetes.io/docs/concepts/cluster-administration/addons/

You can now join any number of machines by running the following on each node
as root:

  kubeadm join 192.168.56.200:6443 --token vyabge.9wi2cqgukrf3c2qh --discovery-token-ca-cert-hash sha256:2224c75b200730114e0153aa7aea9014affd90ffc47cfe08b7f931ef8e58b6ab

[root@k8s-master ~]# rm -rf $HOME/.kube
[root@k8s-master ~]# mkdir -p $HOME/.kube
[root@k8s-master ~]# sudo cp -i /etc/kubernetes/admin.conf $HOME/.kube/config
[root@k8s-master ~]# sudo chown $(id -u):$(id -g) $HOME/.kube/config
[root@k8s-master ~]# kubectl get nodes
NAME                     STATUS     ROLES     AGE       VERSION
k8s-master.localdomain   NotReady   master    2d        v1.11.2
```

## 虚拟机master节点——安装flannel

```
# 下载配置文件
[root@k8s-master ~]# mkdir flannel && cd flannel
[root@k8s-master flannel]# wget https://raw.githubusercontent.com/coreos/flannel/v0.10.0/Documentation/kube-flannel.yml

# 修改配置
# 此处的ip配置要与上面kubeadm的pod-network一致
  net-conf.json: |
    {
      "Network": "10.244.0.0/16",
      "Backend": {
        "Type": "vxlan"
      }
    }
# 修改镜像
        image: registry.cn-shanghai.aliyuncs.com/gcr-k8s/flannel:v0.10.0-amd64

# 如果Node有多个网卡的话，参考flannel issues 39701，
# https://github.com/kubernetes/kubernetes/issues/39701
# 目前需要在kube-flannel.yml中使用--iface参数指定集群主机内网网卡的名称，
# 否则可能会出现dns无法解析。容器无法通信的情况，需要将kube-flannel.yml下载到本地，
# flanneld启动参数加上--iface=<iface-name>
      containers:
      - name: kube-flannel
        image: registry.cn-shanghai.aliyuncs.com/gcr-k8s/flannel:v0.10.0-amd64
        command:
        - /opt/bin/flanneld
        args:
        - --ip-masq
        - --kube-subnet-mgr
        - --iface=enp0s8

# 启动
[root@k8s-master flannel]# kubectl apply -f kube-flannel.yml
clusterrole.rbac.authorization.k8s.io/flannel created
clusterrolebinding.rbac.authorization.k8s.io/flannel created
serviceaccount/flannel created
configmap/kube-flannel-cfg created
daemonset.extensions/kube-flannel-ds created

# 查看
[root@k8s-master flannel]# kubectl get pods --namespace kube-system
NAME                                             READY     STATUS              RESTARTS   AGE
coredns-777d78ff6f-2z75r                         0/1       ContainerCreating   0          2d
coredns-777d78ff6f-rghp9                         0/1       ContainerCreating   0          2d
etcd-k8s-master.localdomain                      1/1       Running             1          2d
kube-apiserver-k8s-master.localdomain            1/1       Running             1          2d
kube-controller-manager-k8s-master.localdomain   1/1       Running             1          2d
kube-flannel-ds-jftcs                            1/1       Running             0          19s
kube-proxy-wvsxb                                 1/1       Running             1          2d
kube-scheduler-k8s-master.localdomain            1/1       Running             1          2d
[root@k8s-master flannel]# kubectl get svc --namespace kube-system
NAME       TYPE        CLUSTER-IP   EXTERNAL-IP   PORT(S)         AGE
kube-dns   ClusterIP   10.96.0.10   <none>        53/UDP,53/TCP   2d
```

## 配置slave节点加入集群

```
[root@k8s-master ~]# kubeadm join 192.168.56.201:6443 --token vliohs.l5lz1treou1srvt1 --discovery-token-ca-cert-hash sha256:0c08eeb897ef635c188770ef42e82580326f5ff534be49cc1a80fd8785173495

```







