---
title: CEPH MANAGER DAEMON - RESTful plugin
date: 2017-11-27 21:56:23
tags: CEPH-MGR
---

CEPH MANAGER DAEMON - RESTful plugin- RESTFUL PLUGIN
RESTful插件通过SSL-secured connection提供REST API访问集群状态。

**ENABLING**

restful模块启用： 

    ceph mgr module enable restful

在API endpoint 可用之前，您还需要配置下面的SSL certificate（证书）。默认情况下，模块将在主机上所有IPv4和IPv6地址的8003端口上接受HTTPS请求。

**SECURING**

所有的restful连接都通过SSL安全保护。您可以使用以下命令生成自签名证书：

    ceph restful create-self-signed-cert

请注意，对于自签名证书，大多数客户端将需要一个标志来允许连接and/or禁止警告消息。例如，如果ceph-mgr daemon在同一主机上，则：

    curl -k https://localhost:8003/

为了更安全的部署，应使用由组织认证机构签发的证书。例如，可以使用类似于以下的命令来生成密钥对：

    openssl req -new -nodes -x509 \
    -subj "/O=IT/CN=ceph-mgr-restful" \
    -days 3650 -keyout restful.key -out restful.crt -extensions v3_ca

然后restful.crt应由您的organization的CA（证书颁发机构）签名。一旦完成，您可以将其设置为：

    ceph config-key set mgr/restful/$name/crt -i restful.crt
    ceph config-key set mgr/restful/$name/key -i restful.key

其中$name是ceph-mgr实例的名称（通常是主机名）。如果所有manager实例要共享相同的证书，则可以忽略$name部分：

    ceph config-key set mgr/restful/crt -i restful.crt
    ceph config-key set mgr/restful/key -i restful.key

**CONFIGURING IP AND PORT**

像任何其他RESTful API endpoint一样，restful绑定IP和port。默认情况下，当前active的ceph-mgr daemon将绑定主机上任何可用的IPv4或IPv6地址的8003端口。
由于每个ceph-mgr都拥有自己的restful实例，因此也可能需要单独配置它们。IP和端口可以通过配置key facility进行更改：

    ceph config-key set mgr/restful/$name/server_addr $IP
    ceph config-key set mgr/restful/$name/server_port $PORT
其中$name是ceph-mgr守护进程的ID（通常是hostname）。
这些设置也可以在集群范围内配置，而不是manager 指定的。例如：

    ceph config-key set mgr/restful/server_addr $IP
    ceph config-key set mgr/restful/server_port $PORT
如果端口没有配置，restful将默认绑定到8003端口。
如果没有配置IP地址，restful将绑定到::，这对应于所有可用的IPv4和IPv6地址。

**LOAD BALANCER**

请注意，restful只会在当前处于active状态的manager启动。查询Ceph集群状态来查看哪个manager处于active状态（例如，ceph mgr dump）。为了使API可通过一致的URL访问，无论哪个管理器daemon当前处于active状态，您可能需要在前端设置一个负载平衡器，以将流量引导至任何可用的manager  endpoint。
