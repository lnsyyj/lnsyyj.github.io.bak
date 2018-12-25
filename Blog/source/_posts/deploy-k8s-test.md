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

## mac

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

查看当前config
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

查看当前context
yujiangdeMBP-13:~ yujiang$ kubectl config get-contexts
CURRENT   NAME       CLUSTER    AUTHINFO   NAMESPACE
*         minikube   minikube   minikube   

查看当前k8s集群情况
yujiangdeMBP-13:~ yujiang$ kubectl cluster-info
Kubernetes master is running at https://192.168.99.100:8443

进入minikube虚拟机
yujiangdeMBP-13:~ yujiang$ minikube ssh

设置代理（ss-ng），否则无法pull docker镜像(https://blog.zhesih.com/2018/06/24/k8s-minikube-setup/)
$ sudo vi /etc/systemd/system/docker.service.d/http-proxy.conf
$ sudo systemctl daemon-reload
$ sudo systemctl restart docker
$ systemctl show --property=Environment docker
Environment=DOCKER_RAMDISK=yes HTTP_PROXY=http://192.168.199.165:1087 HTTPS_PROXY=https://192.168.199.165:1087
```

### 实验一（kind: Pod）

```
Pod是k8s调度的最小单位，一个Pod中可以有多个Container
1、创建一个Pod
yujiangdeMBP-13:k8s_yaml yujiang$ cat pod_nginx.yml 
apiVersion: v1
kind: Pod
metadata:
        name: nginx
        labels:
                app: nginx
spec:
        containers:
        - name: nginx
          image: nginx
          ports:
          - containerPort: 80

yujiangdeMBP-13:k8s_yaml yujiang$ kubectl create -f pod_nginx.yml 
pod/nginx created

2、查看Pod
yujiangdeMBP-13:k8s_yaml yujiang$ kubectl get pod --all-namespaces
NAMESPACE     NAME                                    READY   STATUS    RESTARTS   AGE
default       nginx                                   1/1     Running   0          20m
kube-system   kube-addon-manager-minikube             1/1     Running   8          60m
kube-system   kube-dns-54cccfbdf8-kr847               3/3     Running   12         57m
kube-system   kubernetes-dashboard-77d8b98585-vq77b   1/1     Running   4          57m
kube-system   storage-provisioner                     1/1     Running   5          57m

3、查看Pod的详细信息，可以看到Pod运行在哪台机器上
yujiangdeMBP-13:k8s_yaml yujiang$ kubectl get pods -o wide
NAME    READY   STATUS    RESTARTS   AGE   IP           NODE       NOMINATED NODE   READINESS GATES
nginx   1/1     Running   0          31m   172.17.0.2   minikube   <none>           <none>

4、打开dashboard
yujiangdeMBP-13:k8s_yaml yujiang$ minikube dashboard

5、查询nginx IP
$ docker network ls
NETWORK ID          NAME                DRIVER              SCOPE
fb08befd952b        bridge              bridge              local
2de0a434731f        host                host                local
4cd7f4c7083c        none                null                local

$ docker network inspect bridge
[
    {
        "Name": "bridge",
        "Id": "fb08befd952b4448e86e491f271a1e6a50056b5a18bdfb8a3e930edde2ac44da",
        "Created": "2018-12-24T17:23:11.618994916Z",
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
            "25e63a3ff8a091319fdec698f554f1b47c3e019fbdda9b06709d15630003ee6e": {
                "Name": "k8s_POD_kube-dns-54cccfbdf8-kr847_kube-system_69d32f1a-0799-11e9-be08-080027bea66e_8",
                "EndpointID": "004b9a81ab36cd4e0aba21572b4736b8d85d5541c3533571a67383e5d07e34b6",
                "MacAddress": "02:42:ac:11:00:03",
                "IPv4Address": "172.17.0.3/16",
                "IPv6Address": ""
            },
            "4138eeb8348781495987d47f6683138515111d2b6afa6fc7352d5f98d4a0858b": {
                "Name": "k8s_POD_kubernetes-dashboard-77d8b98585-vq77b_kube-system_69b46ed4-0799-11e9-be08-080027bea66e_8",
                "EndpointID": "bb31db755c5a5cd8f36bec35d89c73267d3d283ba8caa691b58ba504b0d6fabc",
                "MacAddress": "02:42:ac:11:00:04",
                "IPv4Address": "172.17.0.4/16",
                "IPv6Address": ""
            },
            "6874ee85f9257b5b0dfdf808ebf92df9c3c677079a778980956e4655abf23507": {
                "Name": "k8s_POD_nginx_default_9e0d9ffc-079e-11e9-be08-080027bea66e_0",
                "EndpointID": "040a5616a76665ff9e35bb6cef2388f8db443b2db94b67282a7496b9803acc20",
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

$ curl 172.17.0.2
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

6、指定Pod名，进入Container(默认进入第1个Container)。如果Pod中有多个Container，可以加"-c"参数指定进入哪个Container。
yujiangdeMBP-13:k8s_yaml yujiang$ kubectl exec -it nginx sh

7、打印nginx Pod的详细信息
yujiangdeMBP-13:k8s_yaml yujiang$ kubectl describe pod nginx

8、怎样把nginx的端口映射出来？
有两种方法：
	(1)port-forward
		yujiangdeMBP-13:k8s_yaml yujiang$ kubectl port-forward nginx 8080:80
		打开浏览器：http://127.0.0.1:8080/
```

### 实验二（kind: ReplicationController）

```
yujiangdeMBP-13:k8s_yaml yujiang$ cat rc_nginx.yml 
apiVersion: v1
kind: ReplicationController
metadata:
        name: nginx
spec:
        replicas: 3
        selector:
                app: nginx
        template:
                metadata:
                        name: nginx
                        labels:
                                app: nginx
                spec:
                        containers:
                        - name: nginx
                          image: nginx
                          ports:
                          - containerPort: 80
yujiangdeMBP-13:k8s_yaml yujiang$ kubectl create -f rc_nginx.yml 
replicationcontroller/nginx created

yujiangdeMBP-13:k8s_yaml yujiang$ kubectl get pod 
NAME          READY   STATUS    RESTARTS   AGE
nginx-prlfw   1/1     Running   0          19s
nginx-szrh8   1/1     Running   0          19s
nginx-z59kd   1/1     Running   0          19s

# 修改横向扩展数量
yujiangdeMBP-13:k8s_yaml yujiang$ kubectl scale rc nginx --replicas=2
replicationcontroller/nginx scaled
yujiangdeMBP-13:k8s_yaml yujiang$ kubectl get pods
NAME          READY   STATUS    RESTARTS   AGE
nginx-szrh8   1/1     Running   1          21h
nginx-z59kd   1/1     Running   1          21h
yujiangdeMBP-13:k8s_yaml yujiang$ kubectl get rc
NAME    DESIRED   CURRENT   READY   AGE
nginx   2         2         2       21h
yujiangdeMBP-13:k8s_yaml yujiang$ kubectl get pods -o wide
NAME          READY   STATUS    RESTARTS   AGE   IP           NODE       NOMINATED NODE   READINESS GATES
nginx-szrh8   1/1     Running   1          21h   172.17.0.4   minikube   <none>           <none>
nginx-z59kd   1/1     Running   1          21h   172.17.0.6   minikube   <none>           <none>
```

### 实验三（kind: ReplicaSet）

```
apiVersion: apps/v1
kind: ReplicaSet
metadata:
        name: nginx
        labels:
                tier: frontend
spec:
        replicas: 3
        selector:
                matchLabels:
                        tier: frontend
        template:
                metadata:
                        name: nginx
                        labels:
                                tier: frontend
                spec:
                        containers:
                        - name: nginx
                          image: nginx
                          ports:
                          - containerPort: 80

yujiangdeMBP-13:k8s_yaml yujiang$ kubectl create -f rs_nginx.yml 
replicaset.apps/nginx created
yujiangdeMBP-13:k8s_yaml yujiang$ kubectl get rs
NAME    DESIRED   CURRENT   READY   AGE
nginx   3         3         3       52s
yujiangdeMBP-13:k8s_yaml yujiang$ kubectl get pods
NAME          READY   STATUS    RESTARTS   AGE
nginx-brtzk   1/1     Running   0          81s
nginx-m5xgq   1/1     Running   0          81s
nginx-qvdq6   1/1     Running   0          81s
yujiangdeMBP-13:k8s_yaml yujiang$ kubectl scale rs nginx --replicas=2
replicaset.extensions/nginx scaled
yujiangdeMBP-13:k8s_yaml yujiang$ kubectl get rs
NAME    DESIRED   CURRENT   READY   AGE
nginx   2         2         2       2m32s

```

### 实验四（kind: Deployment）

```
yujiangdeMBP-13:k8s_yaml yujiang$ cat deployment_nginx.yml 
apiVersion: apps/v1
kind: Deployment
metadata:
        name: nginx-deployment
        labels:
                app: nginx
spec:
        replicas: 3
        selector:
                matchLabels:
                        app: nginx
        template:
                metadata:
                        labels:
                                app: nginx
                spec:
                        containers:
                                - name: nginx
                                  image: nginx:1.12.2
                                  ports:
                                  - containerPort: 80
yujiangdeMBP-13:k8s_yaml yujiang$ kubectl create -f deployment_nginx.yml
deployment.apps/nginx-deployment created
yujiangdeMBP-13:k8s_yaml yujiang$ kubectl get deployment
NAME               READY   UP-TO-DATE   AVAILABLE   AGE
nginx-deployment   3/3     3            3           5m31s
yujiangdeMBP-13:k8s_yaml yujiang$ kubectl get rs
NAME                         DESIRED   CURRENT   READY   AGE
nginx-deployment-9898d9674   3         3         3       6m6s
yujiangdeMBP-13:k8s_yaml yujiang$ kubectl get pods
NAME                               READY   STATUS    RESTARTS   AGE
nginx-deployment-9898d9674-2mrdl   1/1     Running   0          6m31s
nginx-deployment-9898d9674-ndblx   1/1     Running   0          6m31s
nginx-deployment-9898d9674-t8ngw   1/1     Running   0          6m31s
yujiangdeMBP-13:k8s_yaml yujiang$ kubectl get deployment -o wide
NAME               READY   UP-TO-DATE   AVAILABLE   AGE     CONTAINERS   IMAGES         SELECTOR
nginx-deployment   3/3     3            3           7m31s   nginx        nginx:1.12.2   app=nginx

# 更新nginx
yujiangdeMBP-13:k8s_yaml yujiang$ kubectl set image deployment nginx-deployment nginx=nginx:1.13
deployment.extensions/nginx-deployment image updated
yujiangdeMBP-13:k8s_yaml yujiang$ kubectl get deployment
NAME               READY   UP-TO-DATE   AVAILABLE   AGE
nginx-deployment   3/3     3            3           12m
yujiangdeMBP-13:k8s_yaml yujiang$ kubectl get deployment -o wide
NAME               READY   UP-TO-DATE   AVAILABLE   AGE   CONTAINERS   IMAGES       SELECTOR
nginx-deployment   3/3     3            3           12m   nginx        nginx:1.13   app=nginx
yujiangdeMBP-13:k8s_yaml yujiang$ kubectl get rs
NAME                         DESIRED   CURRENT   READY   AGE
nginx-deployment-9898d9674   0         0         0       13m
nginx-deployment-d8d99448f   3         3         3       3m42s
yujiangdeMBP-13:k8s_yaml yujiang$ kubectl get pods
NAME                               READY   STATUS    RESTARTS   AGE
nginx-deployment-d8d99448f-fzfdw   1/1     Running   0          3m59s
nginx-deployment-d8d99448f-l8m8w   1/1     Running   0          72s
nginx-deployment-d8d99448f-pbnml   1/1     Running   0          73s

# 回滚到上一次更新前
yujiangdeMBP-13:k8s_yaml yujiang$ kubectl rollout history deployment nginx-deployment
deployment.extensions/nginx-deployment 
REVISION  CHANGE-CAUSE
1         <none>
2         <none>
yujiangdeMBP-13:k8s_yaml yujiang$ kubectl rollout undo deployment nginx-deployment
deployment.extensions/nginx-deployment rolled back
yujiangdeMBP-13:k8s_yaml yujiang$ kubectl get deployment -o wide
NAME               READY   UP-TO-DATE   AVAILABLE   AGE   CONTAINERS   IMAGES         SELECTOR
nginx-deployment   3/3     3            3           15m   nginx        nginx:1.12.2   app=nginx
yujiangdeMBP-13:k8s_yaml yujiang$ kubectl rollout history deployment nginx-deployment
deployment.extensions/nginx-deployment 
REVISION  CHANGE-CAUSE
2         <none>
3         <none>

# 暴露nginx Container端口到宿主机(node节点)
yujiangdeMBP-13:k8s_yaml yujiang$ kubectl get node -o wide
NAME       STATUS   ROLES    AGE   VERSION   INTERNAL-IP      EXTERNAL-IP   OS-IMAGE            KERNEL-VERSION   CONTAINER-RUNTIME
minikube   Ready    <none>   23h   v1.9.4    192.168.99.100   <none>        Buildroot 2017.11   4.9.64           docker://17.9.0
yujiangdeMBP-13:k8s_yaml yujiang$ kubectl expose deployment nginx-deployment --type=NodePort 
service/nginx-deployment exposed
yujiangdeMBP-13:k8s_yaml yujiang$ kubectl get svc
NAME               TYPE        CLUSTER-IP    EXTERNAL-IP   PORT(S)        AGE
kubernetes         ClusterIP   10.96.0.1     <none>        443/TCP        23h
nginx-deployment   NodePort    10.98.51.33   <none>        80:30233/TCP   68s
# 打开浏览器http://192.168.99.100:30233/
```

### 实验五（kubectl自动补全）

```
https://kubernetes.io/docs/tasks/tools/install-kubectl/
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

















