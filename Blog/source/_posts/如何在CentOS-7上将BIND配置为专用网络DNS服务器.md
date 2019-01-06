---
title: 如何在CentOS 7上将BIND配置为专用网络DNS服务器
date: 2019-01-06 00:00:30
tags: DNS
---

Ceph的对象存储服务中有网站托管服务，需要结合DNS来做配合。所以想借此机会了解一下DNS服务，DNS开源软件中最有名的当属BIND。那么如果搭建该服务？如何与Ceph的对象存储服务radosgateway对接？这两个课题是需要来实验的。这篇文章不包括DNS理论，因为在网络上有很多，可以自己google一下。本篇文章主要是：如果搭建BIND服务，以实践为主，我在网络上找到一篇英文文章，是关于BIND在centos7上搭建的，如果英文可以请看[原文](https://www.digitalocean.com/community/tutorials/how-to-configure-bind-as-a-private-network-dns-server-on-centos-7)。

下面我们就来验证一下BIND的搭建：

# Introduction 介绍

通过设置适当的域名系统（DNS, Domain Name System），可以按名称查找network interfaces和IP地址，这使得管理server configuration和infrastructure变得更容易。在本教程中，我们将使用CentOS 7上的BIND name servers软件（BIND9）来讨论如何设置内部DNS服务器，Virtual Private Servers (VPS) 可以使用它来解析private host names和private IP地址。这是管理内部多主机hostnames和私有IP地址时必不可少的。

可以在此处找到本教程的[Ubuntu版本](https://www.digitalocean.com/community/tutorials/how-to-configure-bind-as-a-private-network-dns-server-on-ubuntu-14-04)。

# Prerequisites 先决条件

要完成本教程，您需要以下内容：

- 同一datacenter，并运行在同一private networking下的多台server（可以是虚拟机）
- Primary DNS server，ns1
- Secondary DNS server，ns2
- Root访问以上所有内容（[steps 1-4 here](https://www.digitalocean.com/community/tutorials/initial-server-setup-with-centos-7)）

如果您不熟悉DNS概念，建议您阅读[Introduction to Managing DNS](https://www.digitalocean.com/community/tutorial_series/an-introduction-to-managing-dns)。

# Example Hosts 示例主机

出于示例目的，我们将假设以下内容：

- 我们有两个现有的VPS叫做"host1"和"host2"
- 两个VPS都存在于nyc3 datacenter
- 两个VPS都启用了private networking（并且位于10.128.0.0/16子网上）
- 两个VPS都与我们在"example.com"上运行的Web应用程序有某种关联

有了这些假设，我们使用"nyc3.example.com"来参考我们的private subnet或zone的命名方案。因此，host1的private Fully-Qualified Domain Name (FQDN) 将为"host1.nyc3.example.com"。

请参阅下表中的相关详细信息：

| Host  | Role           | Private FQDN           | Private IP Address |
| ----- | -------------- | ---------------------- | ------------------ |
| host1 | Generic Host 1 | host1.nyc3.example.com | 10.128.100.101     |
| host2 | Generic Host 2 | host2.nyc3.example.com | 10.128.200.102     |

注意：自己的实验环境将有所不同，示例中的名称和IP地址只是用于演示如何配置DNS server以提供内部DNS服务。您应该设置适合自己环境的host names和private IP addresses。没有必要在命名方案中使用datacenter的region名称，但我们在此处使用它来表示这些主机属于特定datacenter的private network。如果您使用多个datacenters，则可以在每个相应的datacenter内设置内部DNS。

# Our Goal 我们的目标

在本教程结束时，我们将有一个primary DNS server（ns1），以及可选的secondary DNS server（ns2，它将用作备份）。

这是一个包含示例名称和IP地址的表：

| Host | Role                 | Private FQDN         | Private IP Address |
| ---- | -------------------- | -------------------- | ------------------ |
| ns1  | Primary DNS Server   | ns1.nyc3.example.com | 10.128.10.11       |
| ns2  | Secondary DNS Server | ns2.nyc3.example.com | 10.128.20.12       |

让我们开始安装我们的Primary DNS server，ns1。

# Install BIND on DNS Servers 在DNS服务器上安装BIND

注意：以红色突出显示的文本很重要！ 它通常用于表示需要用您自己的设置替换的内容，或者应该修改或添加到配置文件中的内容。（红色字体我会标注red，全文搜索red就可以）例如，如果您看到类似host1.nyc3.example.com（red）的内容，请将其替换为您自己服务器的FQDN。同样，如果您看到host1_private_IP（red），请将其替换为您自己服务器的private IP地址。

在两个DNS服务器ns1和ns2上，使用yum安装BIND：

```
sudo yum install -y bind bind-utils
```

现在已经安装了BIND，让我们配置primary DNS server。

# Configure Primary DNS Server 配置主DNS服务器

BIND的配置由多个文件组成，这些文件包含在主配置文件named. conf中。 这些文件名以“named”开头，因为这是BIND运行时进程的名称。 我们将从配置options文件开始。

## Configure Bind 配置Bind

BIND的进程名为named，因此，许多文件以"named"命名，而不是"BIND"。

在ns1上，打开named.conf文件进行编辑：

```
sudo vi /etc/named.conf
```

在现有options block上方，创建一个名为"trusted"的新ACL block。在这里我们将定义允许递归查询DNS的客户端列表（即与ns1位于同一datacenter的服务器）。使用我们的示例private IP地址，我们将ns1，ns2，host1和host2的IP添加到可信客户端列表中：

```
/etc/named.conf — 1 of 4

acl "trusted" {
        10.128.10.11;    # ns1 - can be set to localhost
        10.128.20.12;    # ns2
        10.128.100.101;  # host1
        10.128.200.102;  # host2
};
```

我们有了可信DNS客户端列表，我们将要编辑options block。将ns1的private IP添加到`listen-on port 53`指令中，并注释掉`listen-on-v6`行：

```
/etc/named.conf — 2 of 4

options {
        listen-on port 53 { 127.0.0.1; 10.128.10.11; };
#        listen-on-v6 port 53 { ::1; };
...
```

在这些条目下面，将allow-transfer指令从"none"更改为ns2的private IP地址。另外，将allow-query指令从"localhost"更改为"trusted"：

```
/etc/named.conf — 3 of 4

...
options {
...
        allow-transfer { 10.128.20.12; };      # disable zone transfers by default
...
        allow-query { trusted; };  # allows queries from "trusted" clients
...
```

在文件末尾，添加以下行：

```
/etc/named.conf — 4 of 4

include "/etc/named/named.conf.local";
```

现在保存并退出named.conf。上述配置指定只有您自己的服务器（"trusted"服务器）才能查询DNS服务器。

接下来，我们将配置local文件，以指定我们的DNS zones。

## Configure Local File 配置Local文件

在ns1上，打开named.conf.local文件以进行编辑：

```
sudo vi /etc/named/named.conf.local
```

该文件应为空。 在这里，我们将指定forward和reverse zones。

使用以下行添加forward zone（替换您自己的zone名称）：

```
/etc/named/named.conf.local — 1 of 2

zone "nyc3.example.com" {
    type master;
    file "/etc/named/zones/db.nyc3.example.com"; # zone file path
};
```

假设我们的private subnet是10.128.0.0/16，请使用以下行添加reverse zone（请注意，我们的reverse zone名称以"128.10"开头，这是"10.128"的八位字节反转，octet reversal of "10.128"）

```
/etc/named/named.conf.local — 2 of 2

zone "128.10.in-addr.arpa" {
    type master;
    file "/etc/named/zones/db.10.128";  # 10.128.0.0/16 subnet
    };
```

如果您的服务器跨多个private subnets但位于同一datacenter，请确保为每个不同的subnet指定一个额外的zone和zone文件。添加完所有所需zone后，保存并退出named.conf.local文件。

既然我们的zone是在BIND中指定的，我们需要创建相应的forward和reverse zone文件。

## Create Forward Zone File 创建Forward Zone文件

forward zone文件中，我们为正向DNS查找定义了DNS records（记录）。也就是说，当DNS收到名称查询（例如"host1.nyc3.example.com"）时，它将查找forward zone文件以解析host1的相应private IP地址。

创建我们的zone文件。根据我们的named.conf.local配置，该位置应为/etc/named/zones：

```
sudo chmod 755 /etc/named
sudo mkdir /etc/named/zones
```

现在让我们编辑我们的forward zone文件：

```
sudo vi /etc/named/zones/db.nyc3.example.com
```

首先，您需要添加SOA record（记录）。将突出显示的ns1 FQDN替换为您自己的FQDN，然后将第二个"nyc3.example.com"替换为您自己的domain。 每次编辑zone文件时，都应该在重新启动named进程之前增加serial值 —— 我们将它增加到"3"。它应该看起来像这样：

```
/etc/named/zones/db.nyc3.example.com — 1 of 3

@       IN      SOA     ns1.nyc3.example.com. admin.nyc3.example.com. (
                              3         ; Serial
             604800     ; Refresh
              86400     ; Retry
            2419200     ; Expire
             604800 )   ; Negative Cache TTL
```

之后，使用以下行添加您的nameserver records（替换自己的名称`nyc3.example.com.`）。请注意，第二列指定这些是"NS" records（记录）：

```
/etc/named/zones/db.nyc3.example.com — 2 of 3

; name servers - NS records
    IN      NS      ns1.nyc3.example.com.
    IN      NS      ns2.nyc3.example.com.
```

然后为属于此zone的主机添加A记录。 这包括我们希望以".nyc3.example.com" 结尾的名称的任何服务器（替换名称和private IP地址）。使用我们的示例名称和private IP地址，我们将为ns1，ns2，host1和host2添加A记录，如下所示：

```
/etc/named/zones/db.nyc3.example.com — 3 of 3

; name servers - A records
ns1.nyc3.example.com.          IN      A       10.128.10.11
ns2.nyc3.example.com.          IN      A       10.128.20.12

; 10.128.0.0/16 - A records
host1.nyc3.example.com.        IN      A      10.128.100.101
host2.nyc3.example.com.        IN      A      10.128.200.102
```

保存并退出db.nyc3.example.com文件。

我们的最终示例forward zone文件如下所示：

```
/etc/named/zones/db.nyc3.example.com — complete

$TTL    604800
@       IN      SOA     ns1.nyc3.example.com. admin.nyc3.example.com. (
                  3       ; Serial
             604800     ; Refresh
              86400     ; Retry
            2419200     ; Expire
             604800 )   ; Negative Cache TTL
;
; name servers - NS records
     IN      NS      ns1.nyc3.example.com.
     IN      NS      ns2.nyc3.example.com.

; name servers - A records
ns1.nyc3.example.com.          IN      A       10.128.10.11
ns2.nyc3.example.com.          IN      A       10.128.20.12

; 10.128.0.0/16 - A records
host1.nyc3.example.com.        IN      A      10.128.100.101
host2.nyc3.example.com.        IN      A      10.128.200.102
```

现在让我们转到reverse zone文件。

## Create Reverse Zone File(s) 创建Reverse Zone文件

Reverse zone文件是我们为反向DNS查找定义DNS PTR记录的地方。也就是说，当DNS通过IP地址（例如"10.128.100.101"）接收查询时，它将查看reverse zone文件以解析相应的FQDN，在这种情况下查询结果为"host1.nyc3.example.com"。

在ns1上，对于named.conf.local文件中指定的每个reverse zone，都创建一个reverse zone文件。

编辑与named.conf.local中定义的reverse zone对应的reverse zone文件：

```
sudo vi /etc/named/zones/db.10.128
```

与forward zone文件相同的方式，将突出显示的ns1 FQDN替换为您自己的FQDN，然后将第二个"nyc3.example.com"替换为您自己的domain。每次编辑zone文件时，都应该在重新启动named进程之前递增serial值 —— 我们将其增加到"3"。 它应该看起来像这样：

```
/etc/named/zones/db.10.128 — 1 of 3

@       IN      SOA     ns1.nyc3.example.com. admin.nyc3.example.com. (
                              3         ; Serial
                         604800         ; Refresh
                          86400         ; Retry
                        2419200         ; Expire
                         604800 )       ; Negative Cache TTL
```

之后，使用以下行添加您的nameserver records（用您自己的名称替换）。 请注意，第二列指定这些是"NS"记录：

```
/etc/named/zones/db.10.128 — 2 of 3

; name servers - NS records
      IN      NS      ns1.nyc3.example.com.
      IN      NS      ns2.nyc3.example.com.
```

然后为所有服务器添加PTR记录，这些服务器的IP地址位于您正在编辑的zone文件的subnet上。在我们的示例中，这包括我们所有的主机，因为它们都在10.128.0.0/16 subnet上。请注意，第一列包含服务器private IP地址的最后两个八位字节，顺序相反。请务必替换名称和private IP地址以匹配您的服务器：

```
/etc/named/zones/db.10.128 — 3 of 3

; PTR Records
11.10   IN      PTR     ns1.nyc3.example.com.    ; 10.128.10.11
12.20   IN      PTR     ns2.nyc3.example.com.    ; 10.128.20.12
101.100 IN      PTR     host1.nyc3.example.com.  ; 10.128.100.101
102.200 IN      PTR     host2.nyc3.example.com.  ; 10.128.200.102
```

保存并退出reverse zone文件（如果需要添加更多reverse zone文件，请重复此部分）。

我们的最终示例反向区域文件如下所示：

```
/etc/named/zones/db.10.128 — complete

$TTL    604800
@       IN      SOA     nyc3.example.com. admin.nyc3.example.com. (
                              3         ; Serial
                         604800         ; Refresh
                          86400         ; Retry
                        2419200         ; Expire
                         604800 )       ; Negative Cache TTL
; name servers
      IN      NS      ns1.nyc3.example.com.
      IN      NS      ns2.nyc3.example.com.

; PTR Records
11.10   IN      PTR     ns1.nyc3.example.com.    ; 10.128.10.11
12.20   IN      PTR     ns2.nyc3.example.com.    ; 10.128.20.12
101.100 IN      PTR     host1.nyc3.example.com.  ; 10.128.100.101
102.200 IN      PTR     host2.nyc3.example.com.  ; 10.128.200.102
```

## Check BIND Configuration Syntax 检查BIND配置语法

运行以下命令以检查named.conf *文件的语法：

```
sudo named-checkconf
```

如果您的named配置文件没有语法错误，您将返回shell提示符并且看不到任何错误消息。 如果配置文件有问题，请检查error message和Configure Primary DNS Server section，然后再次尝试named-checkconf。

named-checkzone命令可用于检查zone文件的正确性。 其第一个参数指定zone名称，第二个参数指定相应的zone文件，这两个文件都在named.conf.local中定义。

例如，要检查"nyc3.example.com"的forward zone配置，请运行以下命令（更改名称以匹配你自己的forward zone和文件）：

```
sudo named-checkzone nyc3.example.com /etc/named/zones/db.nyc3.example.com
```

要检查"128.10.in-addr.arpa"的reverse zone配置，请运行以下命令（更改数字以匹配你自己的reverse zone和文件）：

```
sudo named-checkzone 128.10.in-addr.arpa /etc/named/zones/db.10.128
```

当所有配置和zone文件都没有错误时，您应该重新启动BIND服务。

## Start BIND 启动BIND

启动BIND：

```
sudo systemctl start named
```

如果你想在OS启动时自动启动BIND服务，请执行：

```
sudo systemctl enable named
```

您的primary DNS服务器现已设置好并可以响应DNS查询了。 让我们继续创建secondary DNS服务器。

# Configure Secondary DNS Server 配置备用DNS服务器

在大多数环境中，最好设置secondary DNS服务器，以便在primary服务器不可用时响应请求。幸运的是，secondary DNS服务器更容易配置。

在ns2上，编辑named.conf文件：

```
sudo vi /etc/named.conf
```

```
注意：如果您希望跳过这些说明，可以复制ns1的named.conf文件并对其进行修改以侦听ns2的private IP地址，and not allow transfers。
```

在现有options block上方，创建一个名为"trusted"的新ACL block。在这里我们将定义允许递归DNS查询的客户端列表（即与ns1位于同一datacenter的服务器）。使用我们的示例private IP地址，我们将ns1，ns2，host1和host2添加到trusted（可信）客户端列表中：

```
/etc/named.conf — 1 of 4

acl "trusted" {
        10.128.10.11;    # ns1 - can be set to localhost
        10.128.20.12;    # ns2
        10.128.100.101;  # host1
        10.128.200.102;  # host2
};
```

现在我们有了trusted（可信）DNS客户端列表，我们将要编辑options block。将ns1的private IP地址添加到listen-on port 53指令，并注释掉listen-on-v6行：

```
/etc/named.conf — 2 of 4

options {
        listen-on port 53 { 127.0.0.1; 10.128.20.12; };
#        listen-on-v6 port 53 { ::1; };
...
```

将allow-query指令从"localhost"更改为"trusted"：

```
/etc/named.conf — 3 of 4

...
options {
...
        allow-query { trusted; }; # allows queries from "trusted" clients
...
```

在文件末尾，添加以下行：

```
/etc/named.conf — 4 of 4

include "/etc/named/named.conf.local";
```

现在保存并退出named.conf。 上述配置指定只有您自己的服务器（"trusted"服务器）才能查询您的DNS服务。

接下来，我们将配置local文件，以指定我们的DNS zones。

保存并退出named.conf。

现在编辑named.conf.local文件：

```
sudo chmod 755 /etc/named
sudo vi /etc/named/named.conf.local
```

定义与primary DNS服务器上的master zones对应的slave zones。请注意，类型是"slave"，文件不包含路径，并且有一个masters指令应该设置为primary DNS服务器的private IP。如果您在primary DNS服务器中定义了多个reverse zones，请确保在此处全部添加它们：

```
/etc/named/named.conf.local

zone "nyc3.example.com" {
    type slave;
    file "slaves/db.nyc3.example.com";
    masters { 10.128.10.11; };  # ns1 private IP
};

zone "128.10.in-addr.arpa" {
    type slave;
    file "slaves/db.10.128";
    masters { 10.128.10.11; };  # ns1 private IP
};
```

现在保存并退出named.conf.local。

运行以下命令以检查配置文件的有效性：

```
sudo named-checkconf
```

检查后，启动BIND：

```
sudo systemctl start named
```

如果你想在OS启动时自动启动BIND服务，请执行：

```
sudo systemctl enable named
```

现在，您拥有用于private network名称和IP地址解析的primary DNS服务器和secondary DNS服务器。 现在，您必须配置客户端服务器以使用您的private DNS服务器。

# Configure DNS Clients 配置DNS客户端

在"trusted" ACL中的所有服务器都可以查询DNS服务器之前，必须将客户端服务器配置为使用ns1和ns2作为nameservers。 此过程因操作系统而异，但对于大多数Linux发行版，它涉及将name servers添加到/etc/resolv.conf文件中。

## CentOS Clients

在CentOS，RedHat和Fedora Linux VPS上，只需编辑resolv.conf文件：

```
sudo vi /etc/resolv.conf
```

然后将以下行添加到文件的顶部（替换您的private domain，以及ns1和ns2 private IP地址）：

```
/etc/resolv.conf

search nyc3.example.com  # your private domain
nameserver 10.128.10.11  # ns1 private IP address
nameserver 10.128.20.12  # ns2 private IP address
```

现在保存并退出。 客户端现在已经配置为使用您自己的DNS服务器。

## Ubuntu Clients

在Ubuntu和Debian Linux VPS上，您可以编辑head文件，该文件在启动时附加到resolv.conf：

```
sudo vi /etc/resolvconf/resolv.conf.d/head
```

将以下行添加到文件中（替换您的private domain，以及ns1和ns2 private IP地址）：

```
/etc/resolvconf/resolv.conf.d/head

search nyc3.example.com  # your private domain
nameserver 10.128.10.11  # ns1 private IP address
nameserver 10.128.20.12  # ns2 private IP address
```

现在运行resolvconf以生成新的resolv.conf文件：

```
sudo resolvconf -u
```

客户端现在已经配置为使用您自己的DNS服务器。

## Test Clients

使用nslookup —— 包含在"bind-utils"包中 —— 测试您的客户端是否可以查询您的name servers。您应该能够在已配置在"trusted" ACL中的所有客户端上执行此操作。

## Forward Lookup

例如，我们可以通过运行以下命令来执行forward（正向）查找以检索host1.nyc3.example.com的IP地址：

```
nslookup host1
```

查询"host1"会扩展为"host1.nyc3.example.com"，因为搜索选项设置为您的private subdomain，在查找其他DNS server之前，DNS查询将尝试查看该subdomain。上面命令的输出如下所示：

```
Output:
Server:     10.128.10.11
Address:    10.128.10.11#53

Name:   host1.nyc3.example.com
Address: 10.128.100.101
```

## Reverse Lookup

要测试reverse（反向）查找，请使用host1的private IP地址查询DNS服务器：

```
nslookup 10.128.100.101
```

您应该看到如下所示的输出：

```
Output:
Server:     10.128.10.11
Address:    10.128.10.11#53

11.10.128.10.in-addr.arpa   name = host1.nyc3.example.com.
```

如果所有名称和IP地址都解析为正确的值，则表示您的zone文件已正确配置。如果收到异常值，请务必查看primary DNS服务器上的zone文件（例如db.nyc3.example.com和db.10.128）。

恭喜！ 您的内部DNS服务器现已正确设置！ 现在我们将介绍如何维护您的zone records。

# Maintaining DNS Records 维护DNS记录

现在您有了一个正常工作的内部DNS，您需要维护DNS records，以便它们准确地响应您的server environment。

## Adding Host to DNS 将主机添加到DNS

每当您向environment添加主机（在同一datacenter中）时，都需要将其添加到DNS中。以下是您需要执行的步骤：

Primary Nameserver（主Nameserver）

- Forward zone文件：为新主机添加"A"记录，增加"Serial"的值
- Reverse zone文件：为新主机添加"PTR"记录，增加"Serial"的值
- 将新主机的private IP地址添加到"trusted" ACL（named.conf.options）

然后重新加载BIND：

```
sudo systemctl reload named
```

Secondary Nameserver（辅助Nameserver）

- 将新主机的private IP地址添加到"trusted" ACL（named.conf.options）


然后重新加载BIND：

```
sudo systemctl reload named
```

配置新主机以使用您自己的DNS

- 配置resolv.conf以使用您自己的DNS服务器
- 使用nslookup进行测试

## Removing Host from DNS 从DNS中删除主机

如果您从environment中删除主机或想要将其从DNS中删除，只需删除将服务器添加到DNS时添加的所有内容（即上述步骤的相反步骤）。

# Conclusion 结论

现在，您可以按名称而不是IP地址来访问服务器的private network interfaces。这使得服务和应用程序的配置更加容易，因为您不再需要记住private IP地址，并且文件将更易于阅读和理解。此外，现在您可以更改配置，以便在单个位置指向新服务器，即primary DNS服务器，而不必编辑各种分布式配置文件，从而简化了维护。

设置内部DNS并且配置文件使用private FQDN指定网络连接后，维护DNS服务器至关重要。如果它们都不可用，那么依赖它们的服务和应用程序将无法正常运行。这就是为什么建议使用至少一个secondary服务器设置DNS，并维护所有这些服务器的工作备份原因。

