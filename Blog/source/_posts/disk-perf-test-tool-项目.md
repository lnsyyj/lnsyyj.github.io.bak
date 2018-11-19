---
title: disk_perf_test_tool 项目
date: 2018-10-13 16:54:54
tags: performance
---

## 相关链接

Ukraine/Kharkiv（乌克兰/哈尔科夫）

https://github.com/Mirantis/disk_perf_test_tool

https://github.com/Mirantis/disk_perf_test_tool/wiki

http://koder-ua.github.io/

## Overview（概览）

Wally是一种以分布式方式测量不同类型block storages性能的工具，并提供全面的报告。它在分布式和云环境中运行, 但也可以测量单个磁盘。

Wally在控制的方式中投入了大量精力, 从统计的角度正确的处理结果, 提供你可以依靠、争论和理解的数字。

Wally不是负载生成工具。它使用众所周知的负载发生器 - [fio]来测试系统并为其提供包装, 这有助于discovering cluster features and settings，install sensors，为测试preparing system，并行运行多个测试节点的复杂test suites并可视化结果。

主要特点：

- Cluster and storage preparation，获得尽可能多的可重复结果
- 与[openstack]，[ceph]和[fuel]集成
- 分布式执行测试
- 与[fio]紧密结合
- VM产卵在OS测试
- Sensors subsystem，用于在测试期间收集群集设备上的负载
- 简单而灵活的配置文件，允许指定集群结构并选择在加载期间收集信息
- 综合可视化报告
- 资源消耗报告，允许查看用于向客户端提供服务的群集资源量
- 瓶颈分析器，有助于找到影响结果的部件
- yaml/cvs基于所有结果的存储，因此您可以轻松地将它们插入到结果处理管道中
- wally可以在故障阶段失败时重新启动测试

wally不能做什么：

- Deploy/configure storage - 测试系统必须准备好进行测试
- 在测试执行期间更新报告。沃利是完全 cli 工具, 没有 UI, 报告在测试完成后生成
- 提供交互式报告。所有图/表都是用 matplotlib 并静态生成的。

## Basic architecture overview（基本架构概述）

Wally代码主要由3部分组成 - [agent library]，[cephlib] 和 [wally] 本身。Agent library负责提供与群集和测试节点的 [RPC] 连接。Cephlib 包含大部分storage、discovery、sensors、数据处理和可视化代码。Wally本身提供 cli, 负载工具集成, 报告生成和其他部分。

fio是一个主要的负载工具，很好的集成在wally内。 wally有自己的fio版本，为一些Linux发行版构建。wally也可以使用系统fio，但需要安装最新版本之一。fio配置文件位于wally/suits/io目录中，具有cfg扩展名。default_qd.cfg是具有默认设置的文件，它主要包含所有其他配置。ceph.cfg，hdd.cfg，cinder_iscsi.cfg是一个主要的测试suites。cfg文件是一个fio配置文件，由wally提供了一些额外的功能。测试之前，wally在所选的cfg文件中插入提供的设置，展开cycles，将其拆分为jobs并从测试节点逐个同步执行jobs。

虽然fio提供了一些这样的功能，但是wally不会使用它们来更精确地控制结果。

要运行测试需要一个配置文件，其中包含cluster信息，sensors设置，test config和一些其他变量来控制测试执行和处理结果。配置文件的示例位于configs-examples目录中。该目录中的所有配置文件都includes default.yaml，反过来default.yaml中includes logging.yaml。在大多数情况下，您无需更改default.yaml/logging.yaml文件中的任何内容。配置文件详细描述如下。

wally执行由各个阶段组成, 大多数阶段都映射配置文件模块。主要阶段有:

- 群集发现
- 通过ssh连接节点
- 使用rpc服务器检测节点
- 安装sensors和相应的配置文件
- 运行测试
- 生成报告
- 清理

## Wally motivation（Wally动机）

主要测试问题和wally如何为您修复

## Howto install wally

### Container

```
git clone https://github.com/Mirantis/disk_perf_test_tool.git
docker build -t <username>/wally .
```

### Local installation

```
apt install g++ ....
pip install XXX python -m wally prepare << download fio, compile
可根据Dockerfile查看安装过程
```

## Howto run a test

要运行测试，您需要准备集群和配置文件。
如何运行wally：直接使用容器

## Configuration

- `SSHURI` - 格式为 `[user[:passwd]@]host[:port][:key_file]`的字符串，其中：

```
user - str，用户名，默认为当前用户
passwd - str，ssh密码，如果提供了key_file_rr或使用了默认密钥，则可以省略ssh密码
host - str，唯一必填字段。主机名或IP地址
port - int，要连接的ssh服务器端口，默认为22
key_file - str, ssh私有密钥文件（private）的路径。默认~/.ssh/id_rsa by default. 如果端口被省略, 但提供了key_file - 它必须与host分开两列。

不能同时使用passwd和key_file。
例如：
11.12.23.10:37 - 使用ip 和 ssh 端口, 当前用户和 ~/.ssh/id_rsa key
ceph-1 - 仅使用主机名，默认端口，当前用户和 ~/.ssh/id_rsa key
ceph-12::~/.ssh/keyfile - 使用当前用户和 ~/.ssh/keyfile key
root@master - 以root身份登录并使用 ~/.ssh/id_rsa key
root@127.0.0.1:44 - 以root身份登录，使用44端口 和 ~/.ssh/id_rsa key
user@10.20.20.30::/tmp/keyfile - 以root身份登录 和 /tmp/keyfile key
root:rootpassword@10.20.30.40 - 以root身份登录并使用rootpassword作为ssh密码
```

- `[XXX]` - XXX类型列表

- `{XXX: YYY}` - 从类型XXX（键key）映射到类型YYY（值value）

- `SIZE` - 带有K/M/G/T/P或不带的整数。请注意，使用1024 base，10M实际上意味着10MiB == 10485760 Bytes，依此类推。

## Default settings（默认设置）

许多配置设置在config-examples/default.yaml文件中已经有可用的默认值，在大多数情况下，用户可以重复使用它们。在你自己的配置文件中，include它：

`include: default.yaml`

您可以在配置文件中覆盖选定的设置。

## Plain settings（普通设置）

- `discover`: [str]

列表中可能的值：`ceph`，`openstack`，`fuel`，`ignore_errors`。例：`discover: openstack,ceph`

为wally提供要发现的集群列表。群集发现用于查找群集节点及其角色，以简化设置配置和其他一些步骤。您始终可以在显式部分中定义或重新定义节点角色。每个群集都需要其他配置部分。ignore_errors意味着忽略丢失的集群。

- `run_sensors`: bool

设置为true，允许wally在测试期间收集负载信息。 这大大增加了结果大小，但允许wally提供更复杂的报告。

- `results_storage`: str

要放置结果的默认目录。 对于每个测试，wally将在此目录中生成唯一名称并创建子目录，所有结果和设置将存储其中。 wally必须有rwx权限来访问此目录。

例如: `results_storage: /var/wally`

- `sleep`: int，默认为零

告诉wally在X秒内什么都不做。 如果您只需要收集sensors，则非常有用。

例如：`sleep: 60`

## Section ceph

提供发现ceph集群节点的设置

- `root_node`: str

必须。 根节点的ssh url。 这可以是任何具有ceph client key的节点（任何节点，您可以在其中运行ceph cli命令）。

- `cluster`: str

Ceph集群名称。 默认情况下是ceph。

- `conf`: str

群集配置文件的路径。默认情况下是/etc/ceph/{cluster_name}.conf。

- `key`: str

client.admin密钥文件的路径。默认情况下是/etc/ceph/{cluster_name}.client.admin.keyring。

- `ip_remap`: {IP: IP}

如果OSD和Monitor节点在 ceph 中通过内部 ip 地址注册，这在您运行wally的节点是不可见。这允许将non-routable的IP地址映射到可routable的IP地址。例如：

```
    ip_remap:
        10.8.0.4: 172.16.164.71
        10.8.0.3: 172.16.164.72
        10.8.0.2: 172.16.164.73
```

例如：

```
ceph:
    root_node: ceph-client
    cluster: ceph    # << optional
    ip_remap:        # << optional
        10.8.0.4: 172.16.164.71
        10.8.0.3: 172.16.164.72
        10.8.0.2: 172.16.164.73
```

## Section openstack

提供openstack设置，用于发现OS群集和spawn/find测试vm。

- `skip_preparation`: bool

默认值：true，wally需要准备openstack来生成虚拟机。 如果先前已准备好OS群集，则可以将此设置设置为false以节省一些检查时间。

- `openrc`: 或 str ir {str: str}

指定源[openstack connection settings]。

`openrc: ENV` - 从环境变量中获取OS credentials。你需要在wally开始之前导出openrc设置，就像这样

```
$ source openrc
$ RUN_WALLY
```

或

```
$ env OS_USER=.. OS_PROJECT=..  RUN_WALLY
```

`openrc: str` - 使用openrc文件，位于提供的路径以获取OS connection settings。例如：openrc: /tmp/my_cluster_openrc

`openrc: {str: str}` - 直接在配置文件中提供connection settings。例如：

```
        openrc:
            OS_USERNAME: USER
            OS_PASSWORD: PASSWD
            OS_TENANT_NAME: KEY_FILE
            OS_AUTH_URL: URL
```

- `insecure`: bool - 在openrc section中提供，覆盖OS_INSECURE设置。

- `vms`: [SSHURI] vm sshuri的列表，除了使用hostname/ip vm 名称前缀之外。wally将找到所有具有此前缀名称的vm，并将其用作测试节点。例如：

```
        vms:
            - wally@wally_vm
            - root:rootpasswd@test_vm
```

这将找到所有名为wally_vm*和test_vm的vm，并尝试使用提供的credentials重用它们进行测试。请注意，默认情况下，vm wally使用openstack ssh key，而不是~/.ssh/id_rsa。有关详细信息，请参阅Openstack vm config部分。

- VM spawning选项。此选项控制要为测试生成的新vm的数量以及要使用的配置文件。所有衍生的vm将自动获得testnode角色并将用于测试。wally尝试使用anti-affinity组在所有计算节点上均匀地生成vm。

`count`: str 或 int. 控制生成多少个vm，可能的值：

```
=X，其中X是int - 根据需要产生尽可能多的vm，以使总测试节点计数不小于X。例如 - 如果你已经有1个明确的测试节点，通过节点提供，并且在之前的测试运行中找到了2个vm并且你设置了count: =4，那么wally将产生一个额外的vm。

X，其中X是integer整数。 正好生成X新vm。

xX，其中X是integer整数。 每个compute产生X个vm。例如：copunt: x3 - 每个compute产生3个vm。
```

`cfg_name`: str，vm config。默认情况下，只有wally_1024配置可用。此配置使用来自`https://cloud-images.ubuntu.com/trusty/current/trusty-server-cloudimg-amd64-disk1.img`的image作为vm镜像，1GiB的RAM，2个vCPU和100GiB卷。 有关详细信息，请参阅Openstack vm config。

`network_zone_name`: str。 内部ip v4的Network pool。 通常是net04

`flt_ip_pool`: str。 用于浮动ip v4的Network pool。 通常是net04_ext

`skip_preparation`: bool，默认为false。默认情况下，在spawn vm之前，wally检查所有必需的先决条件，如vm flavor，image，aa-groups，ssh rules是否准备好并创建遗漏的东西。这告诉wally跳过这个阶段。如果你确定openstack准备好了，你可以设置它在这个阶段节省一些时间，但最好还是保留它以防止问题。

## Section nodes

此section定义要执行的测试suites列表。 每个section都是从suite类型到suite配置的映射。 查看下面不同suites的详细信息。

### fio suite config

- `load`: str - 必须的选项，负载profile的名称。

默认下一个profiles可用：

`ceph` - 适用于各种ceph支持的块设备
`hdd` - 本地hdd驱动器
`cinder_iscsi` - cinder lvm-over-iscsi卷
`check_distribution` - 检查IOPS/latency的分布

有关详细信息，请参阅fio task files section。

- `params`: {str: Any} - 负载profile的参数列表。子参数：

`FILENAME`: str，所有profiles都需要。它将用作fio的测试文件。如果测试文件名在不同的测试节点上不同，则需要在开始测试之前在所有测试节点上创建具有相同名称的（sym）链接，并在此处使用链接名称。

`FILESIZE`: SIZE，文件大小参数。虽然在大多数情况下wally可以正确检测device/file大小，但您不需要测试整个文件。此外，如果文件尚不存在，则需要此参数。

Non-standard负载可能需要一些其他参数，有关详细信息，请参阅fio task files section。

- `use_system_fio`: bool，默认为false。告诉wally使用测试节点本地fio二进制文件，而不是wally附带的。如果wally没有为你发行指定版本的fio，你可能需要这个。默认情况下，最好使用wally的fio。有关详细信息，请参阅HOWTO/Supply fio for you distribution。

- `use_sudo`: bool，默认为false。Wally将使用sudo在测试节点上运行fio。 通常，如果本地测试节点用户不是root用户，则需要。

- `force_prefill`: bool，默认为false。在测试之前，告诉wally无条件地用伪随机数据填充测试file/device。默认情况下，首先检查目标是否已包含随机数据并跳过填充步骤。在这一步中，wally填充整个device，因此可能需要很长时间。

- `skip_prefill`: bool，默认为false。强制wally不用伪随机数据填充目标。如果您正在测试本地hdd/ssd/cinder iscsi，请使用此选项。但是，如果测试ceph backed device或任何具有延迟空间分配的系统设备则不要使用。

例如：

```
  - fio:
      load: ceph
      params:
          FILENAME: /dev/vdb
          FILESIZE: 100G
```

### Key test_profile: str

此section允许使用一些预定义的设置集来生成VM和运行测试。config-examples/default.yaml文件中列出了可用的profiles及其设置。下一个profiles默认可用：

- `openstack_ceph` - 每个compute产生1个VM，并针对/dev/vdb运行ceph fio profile 
- `openstack_cinder` - 每个compute产生1个VM，并针对/dev/vdb运行ceph_iscsi_vdb fio profile
- `openstack_nova` - 每个compute产生1个VM，并针对/opt/test.bin运行hdd fio profile

例如：

```
include: default.yaml
discover: openstack,ceph
run_sensors: true
results_storage: /var/wally_results

ceph:
    root_node: localhost

openstack:
    openrc: ENV  # take creds from environment variable

test_profile: openstack_ceph
```

## Howto test

###  Local block device

使用config-examples/local_block_device.yml作为模板。将{STORAGE_FOLDER}替换为存放结果的文件夹的路径。确保wally具有对此文件夹的read/write访问权限，或者可以创建它。您可以直接测试device，也可以测试已mount device上的文件。将{STORAGE_DEV_OR_FILE_NAME}替换为正确的路径。在大多数情况下，wally可以正确检测file或block device大小，但通常最好直接设置{STORAGE_OR_FILE_SIZE}。使用的文件越大，对结果的影响越小，将导致不同的缓存，但也会填充更长的时间。

测试sdb device的示例：

```
include: default.yaml
run_sensors: false
results_storage: /var/wally

nodes:
    localhost: testnode

tests:
  - fio:
      load: hdd
      params:
          FILENAME: /dev/sdb
          FILESIZE: 100G
```

mount到/opt文件夹的device测试示例：

```
include: default.yaml
run_sensors: false
results_storage: /var/wally

nodes:
    localhost: testnode

tests:
  - fio:
      load: hdd
      params:
          FILENAME: /opt/some_test_file.bin
          FILESIZE: 100G
```

请注意，测试完成后，wally不会删除文件。

### Ceph without openstack, or other NAS/SAN

wally仅支持rbd/cephfs测试，不支持object协议，例如rados和RGW。Cephfs测试不需要任何特殊准备，除了将其mounte在测试节点上，详情请参阅ceph fs quick start。

Ceph线性read/write通常受网络限制。例如，如果您在群集中使用10个SATA drives作为storage drives，则聚合线性读取速度可达到~1Gibps或8Gibps，这接近10Gib网络限制。因此，除非你有一个足够带宽的网络测试节点，否则通常最好在多个测试节点并行测试ceph集群。

Ceph在低QD时通常性能较低，因为在这种模式下，您一次只能使用一个OSD。与此同时，ceph可以扩展到比hdd/ssd drives大得多的QD值，因为在这种情况下，您可以在所有OSD daemons中扩展IO请求。您需要最多(16 * OSD_count) QD用于4k随机读取和大约(12 * OSD_COUNT / REPLICATION_FACTOR)QD用于4k随机写入以摸清群集限制。对于其他blocks块和modes模式，您可能需要不同的设置。如果您使用默认的ceph配置文件，则无需关心此操作。

有三种测试RBD的方法 - direct，通过使用[krbd]和虚拟机将其mount到节点，由rbd driver提供的volume，内置到qemu。对于最后一个，参考Ceph with openstack section或文档。

### Using testnode mounted rbd device

首先您需要一个pool作为rbd的target。您可以使用默认rbd pool，也可以创建自己的pool。pool需要有很多PG才能有很好的表现。保守估计是(100 * OSD_COUNT / REPLICATION_FACTOR)。创建后ceph可能会警告“too many PG”，这个消息可以安全地被忽略。ceph文档：PLACEMENT GROUPS。

- 创建一个pool（有关详细信息，请参阅ceph pools documentation）

```
$ ceph osd pool create {pool-name} {pg-num} 
```

- 等到创建完成，所有PG变为active+clean。

- 在此pool中创建rbd volume，需要选择足够大的volume size以缓解不可避免的OSD节点FS caches。通常(SUM_RAM_SIZE_ON_ALL_OSD * 3)运行良好，并且在读取时仅产生约20％的缓存命中：

```
$ rbd create {vol-name} --size {size} --pool {pool-name}
```

- 通过kernel rbd device挂载rbd。这是一个棘手的部分。Kernels通常具有旧版本的rbd driver，并且不支持最新的rbd features。这将导致在mount rbd期间出错。首先尝试挂载rbd device：

```
$ rbd map {vol-name} --pool {pool-name}
```

如果失败 - 您需要运行rbd info --pool {pool-name} {vol-name}，并通过rbd feature disable --pool {pool-name} {vol-name} {feature name}禁用features。然后尝试再次mount。

- wally需要对rbd device进行read/write访问。

### Direct rbd testing

Direct rbd测试通过rbd driver运行，内置在fio中。使用此driver，fio可以直接生成RBD请求，无需外部rbd driver。这是测试RBD的最快和最可靠的方法，但是使用内部rbd driver您可以绕过一些可以在生产环境中使用的代码层。wally附带的fio版本没有rbd support，因为它不能被静态地构建。要使用它，您需要使用rbd support构建fio，请参阅Use you fio binary部分的学习指南。

### Ceph with openstack

最简单的方法是使用预定义的openstack_ceph profile。它为每个计算节点生成一个VM，并在所有计算节点上运行ceph测试suite。

例如：

```
include: default.yaml
discover: openstack,ceph
run_sensors: true
results_storage: /var/wally_results

ceph:
    root_node: localhost

openstack:
    openrc: ENV  # take creds from environment variable

test_profile: openstack_ceph
```

### Cinder lvm volumes

none

## Howto

- Use you fio binary

您需要下载fio源代码，在测试节点上编译它以进行linux distribution，使用bz2进行压缩，命名为`fio_{DISTRNAME}_{ARCH}.bz2`并放入`fio_binaries`目录。ARCH是目标系统上`arch`命令的输出。`DISTRNAME`应与`lsb_release -c -s`输出相同。

以下是从master编译最新文件的典型步骤：

```
$ git clone 
$ cd fio
$ ./configure --build-static 
$ make -jXXX  # Replace XXX with you CPU core count to decrease compilation time
$ bzip2 fio
$ mv fio.bz2 WALLY_FOLDER/fio_binaries/fio_DISTRO_ARCH.bz2
```

## Storage structure

wally保存所有输入configurations，所有收集的数据和测试结果保存到results_storage设置目录的单个子文件夹下。所有文件都是csv(results/sensor files)，yaml/js用于配置non-numeric信息，png/svg用于images和联结原始文本文件，如日志和一些输出。

以下是每个文件包含的内容：

- cli - txt，wally cli in semi-raw formal
- config.yaml - yaml，完整的最终config，从原始的wally config构建，处理所有替换和cli参数传递。
- log - txt，wally执行日志。包含某个测试的所有wally运行log，包括restarts和报告生成。
- result_code - yaml，此文件夹上带有'test'子命令的最后一次执行的退出代码。
- run_interval - yaml，此文件夹上带有'test'子命令的最后一次执行的[begin_time, end_time]列表。
- meta - folder，用于统计计算的cached值。
- nodes - folder，有关测试集群的信息。

       all.yml - yaml，除节点参数外的所有节点的信息

       nodes/parameters.js - js，节点参数。参数是单独存储的，因为它们可能是非常多的ceph节点，并且在python中解析的js文件比yaml快得多。

- report - folder，html/css文件报告和所有图表。可以复制到其他地方。

       index.html - 报告开始页面。

       main.css - css文件报告。

       XXX/YYY.png or .svg - 图表文件报告。

- results - 所有fio结果的文件夹

       fio_{SUITE_NAME}_{IDX1}.yml - yaml，每个已执行suite的完整配置。

       fio_{SUITE_NAME}_{IDX1}.{JOB_SHORT_DESCR}_{IDX2} - 包含suite中每个job所有数据的文件夹

              {NODE_IP}:{NODE_SSH_PORT}.fio.{TS}.(csv|json) - fio输出文件。TS是解析时间序列（timeseries）名称 - bw或lat或stdout用于输出。

​              info.yml - 社区的原作者没有写完

















