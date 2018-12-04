---
title: gitlab jenkins ci
date: 2018-12-03 00:13:42
tags: jenkins
---

# 搭建jenkins（centos 7）

```
安装jenkins
sudo yum install wget -y
sudo wget -O /etc/yum.repos.d/jenkins.repo http://pkg.jenkins-ci.org/redhat/jenkins.repo
sudo rpm --import https://jenkins-ci.org/redhat/jenkins-ci.org.key
sudo yum install jenkins java -y

启动jenkins
sudo systemctl start jenkins
sudo systemctl enable jenkins
sudo systemctl status jenkins

设置防火墙
sudo firewall-cmd --permanent --new-service=jenkins
sudo firewall-cmd --permanent --service=jenkins --set-short="Jenkins Service Ports"
sudo firewall-cmd --permanent --service=jenkins --set-description="Jenkins service firewalld port exceptions"
sudo firewall-cmd --permanent --service=jenkins --add-port=8080/tcp
sudo firewall-cmd --permanent --add-service=jenkins
sudo firewall-cmd --zone=public --add-service=http --permanent
sudo firewall-cmd --reload
```



# 搭建gitlab

```
sudo yum install -y curl policycoreutils-python openssh-server
sudo systemctl enable sshd
sudo systemctl start sshd
sudo firewall-cmd --permanent --add-service=http
sudo firewall-cmd --permanent --add-service=httpsudo systemctl reload firewalld
sudo systemctl reload firewalld

安装Postfix以发送通知电子邮件
sudo yum install postfix
sudo systemctl enable postfix
sudo systemctl start postfix

安装gitbla
wget https://mirror.tuna.tsinghua.edu.cn/gitlab-ce/yum/el7/gitlab-ce-11.5.1-ce.0.el7.x86_64.rpm
rpm -i gitlab-ce-11.5.1-ce.0.el7.x86_64.rpm


gitlab-ctl reconfigure

vi /etc/gitlab/gitlab.rb
unicorn['port'] = 80

gitlab-ctl reconfigure
gitlab-ctl restart

```







# 配置jenkins gitlab ci













