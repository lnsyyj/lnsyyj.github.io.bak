---
title: Avocado Test Framework
date: 2018-05-24 09:51:17
tags: automated testing
---

Avocado是一组工具和库, 用于自动化测试。

以下是基于tag61版本的源码部署方式：

```
编译安装方式：
sudo yum  install -y python2 git gcc python-devel python-pip libvirt-devel libffi-devel openssl-devel libyaml-devel redhat-rpm-config xz-devel libvirt-python

pip install --upgrade setuptools

git clone https://github.com/avocado-framework/avocado.git
cd avocado/
git checkout -b my61 61.0

make develop
sudo make requirements
sudo python setup.py install


遇到问题：
[root@cephL avocado]# avocado -v
Avocado 61.0
[root@cephL avocado]# avocado -h
usage: avocado [-h] [-v] [--config [CONFIG_FILE]] [--show [STREAM[:LVL]]] [-s]
               
               {config,diff,distro,exec-path,list,multiplex,plugins,run,sysinfo,variants}
               ...

Avocado Test Runner

optional arguments:
  -h, --help            show this help message and exit
  -v, --version         show program's version number and exit
  --config [CONFIG_FILE]
                        Use custom configuration from a file
  --show [STREAM[:LVL]]
                        List of comma separated builtin logs, or logging
                        streams optionally followed by LEVEL (DEBUG,INFO,...).
                        Builtin streams are: "test": test output; "debug":
                        tracebacks and other debugging info; "app":
                        application output; "early": early logging of other
                        streams, including test (very verbose); "remote":
                        fabric/paramiko debug; "all": all builtin streams;
                        "none": disables regular output (leaving only errors
                        enabled). By default: 'app'
  -s, --silent          disables regular output (leaving only errors enabled)

subcommands:
  valid subcommands

  {config,diff,distro,exec-path,list,multiplex,plugins,run,sysinfo,variants}
                        subcommand help
    config              Shows avocado config keys
    diff                Shows the difference between 2 jobs.
    distro              Shows detected Linux distribution
    exec-path           Returns path to avocado bash libraries and exits.
    list                List available tests
    multiplex           Tool to analyze and visualize test variants and params
    plugins             Displays plugin information
    run                 Runs one or more tests (native test, test alias,
                        binary or script)
    sysinfo             Collect system information
    variants            Tool to analyze and visualize test variants and params
Failed to load plugin from module "avocado_runner_remote": ImportError('No module named api',)
Failed to load plugin from module "avocado_runner_docker": ImportError('No module named api',)
Failed to load plugin from module "avocado_runner_vm": ImportError('No module named api',)

fabric库升级导致No module named api，以下为解决办法：
[root@cephL avocado]# pip list | grep fabric
fabric (2.0.1)
[root@cephL avocado]# pip uninstall fabric
Uninstalling fabric-2.0.1:
  /root/.local/lib/python2.7/site-packages/fabric-2.0.1-py2.7.egg
Proceed (y/n)? y
  Successfully uninstalled fabric-2.0.1
[root@cephL avocado]# pip install fabric==1.14.0

测试avocado运行状态
[root@cephL avocado]# avocado run /bin/true 
JOB ID     : 7c46b08f9cc78635ef6d4c0b1109da7c5429812a
JOB LOG    : /root/avocado/job-results/job-2018-05-24T21.58-7c46b08/job.log
 (1/1) /bin/true: PASS (0.02 s)
RESULTS    : PASS 1 | ERROR 0 | FAIL 0 | SKIP 0 | WARN 0 | INTERRUPT 0 | CANCEL 0
JOB TIME   : 0.12 s
JOB HTML   : /root/avocado/job-results/job-2018-05-24T21.58-7c46b08/results.html
```

python依赖，以下版本运行正常

```
[root@cephL scripts]# pip list 
aexpect (1.4.0)
asn1crypto (0.24.0)
avocado-framework (61.0, /usr/lib/python2.7/site-packages/avocado_framework-61.0-py2.7.egg)
avocado-framework-plugin-glib (61.0, /root/github/avocado/optional_plugins/glib)
avocado-framework-plugin-golang (61.0, /root/github/avocado/optional_plugins/golang)
avocado-framework-plugin-loader-yaml (61.0, /root/github/avocado/optional_plugins/loader_yaml)
avocado-framework-plugin-result-html (61.0, /root/github/avocado/optional_plugins/html)
avocado-framework-plugin-result-upload (61.0, /root/github/avocado/optional_plugins/result_upload)
avocado-framework-plugin-resultsdb (61.0, /root/github/avocado/optional_plugins/resultsdb)
avocado-framework-plugin-robot (61.0, /root/github/avocado/optional_plugins/robot)
avocado-framework-plugin-runner-docker (61.0, /root/github/avocado/optional_plugins/runner_docker)
avocado-framework-plugin-runner-remote (61.0, /root/github/avocado/optional_plugins/runner_remote)
avocado-framework-plugin-runner-vm (61.0, /root/github/avocado/optional_plugins/runner_vm)
avocado-framework-plugin-varianter-pict (61.0, /root/github/avocado/optional_plugins/varianter_pict)
avocado-framework-plugin-varianter-yaml-to-mux (61.0, /root/github/avocado/optional_plugins/varianter_yaml_to_mux)
Babel (0.9.6)
backports.lzma (0.0.11)
backports.ssl-match-hostname (3.5.0.1)
bcrypt (3.1.4)
Beaker (1.5.4)
ceph-deploy (2.0.0)
ceph-detect-init (1.0.1)
ceph-disk (1.0.0)
ceph-volume (1.0.0)
cephfs (2.0.0)
cffi (1.11.5)
chardet (2.2.1)
CherryPy (3.2.2)
configobj (4.7.2)
cryptography (2.2.2)
decorator (3.4.0)
Django (1.6.11.7)
django-filter (0.9.2)
djangorestframework (2.4.3)
djangorestframework-bulk (0.2)
enum34 (1.1.6)
Fabric (1.14.0)
Flask (0.10.1, /usr/lib/python2.7/site-packages)
idna (2.6)
iniparse (0.4)
invoke (1.0.0)
ipaddress (1.0.16)
IPy (0.75)
itsdangerous (0.23)
Jinja2 (2.7.2)
kmod (0.1)
libvirt-python (3.9.0)
M2Crypto (0.21.1)
m2ext (0.1)
Mako (0.8.1)
MarkupSafe (0.11)
netaddr (0.7.5)
netifaces (0.10.4)
nose (1.3.7)
numpy (1.7.1)
pam (0.1.4)
paramiko (2.4.1)
Paste (1.7.5.1)
pbr (4.0.3)
pecan (0.4.5)
perf (0.1)
Pillow (2.0.0)
pip (8.1.2)
policycoreutils-default-encoding (0.1)
prettytable (0.7.2)
psycopg2 (2.5.1)
pudb (2017.1.4)
pyasn1 (0.4.3)
pycparser (2.18)
pycurl (7.19.0)
Pygments (2.2.0)
pygobject (3.22.0)
pygpgme (0.3)
pyliblzma (0.5.3)
PyNaCl (1.2.1)
pyOpenSSL (0.13.1)
pyparsing (1.5.6)
pystache (0.5.4)
python-linux-procfs (0.4.9)
python-memcached (1.48)
pyudev (0.15)
pyxattr (0.5.1)
PyYAML (3.12)
rados (2.0.0)
rbd (2.0.0)
requests (2.6.0)
resultsdb-api (2.0.0)
rgw (2.0.0)
robotframework (3.0.4)
rtslib-fb (2.1.63)
schedutils (0.4)
seobject (0.1)
sepolicy (1.1)
setuptools (0.9.8)
simplegeneric (0.8)
simplejson (3.15.0)
singledispatch (3.4.0.2)
six (1.11.0)
slip (0.4.0)
slip.dbus (0.4.0)
stevedore (1.28.0)
Tempita (0.5.1)
urlgrabber (3.10)
urllib3 (1.10.2)
urwid (2.0.1)
virtualenv (16.0.0)
WebOb (1.2.3)
WebTest (1.3.4)
Werkzeug (0.9.1)
wheel (0.31.0)
yum-metadata-parser (1.1.4)
```

debug方式

```
avocado --show test run examples/tests/sleeptest.py
```

添加测试case方法

```
[root@cephL ~]# mkdir testcase && cd testcase
[root@cephL ~]# vi sleeptest.py  (添加如下代码，http://avocado-framework.readthedocs.io/en/61.0/WritingTests.html)

import time
from avocado import Test
class SleepTest(Test):
    def test(self):
        sleep_length = self.params.get('sleep_length', default=1)
        self.log.debug("Sleeping for %.2f seconds", sleep_length)
        time.sleep(sleep_length)
运行case
[root@cephL ~]# avocado run /root/mac/testcase
JOB ID     : b5fcd62a2baf4c3b94adb6cfd14fe6e8e767dc4a
JOB LOG    : /root/avocado/job-results/job-2018-06-06T00.59-b5fcd62/job.log
 (1/1) /root/mac/testcase/sleeptest.py:SleepTest.test: PASS (1.01 s)
RESULTS    : PASS 1 | ERROR 0 | FAIL 0 | SKIP 0 | WARN 0 | INTERRUPT 0 | CANCEL 0
JOB TIME   : 1.13 s
JOB HTML   : /root/avocado/job-results/job-2018-06-06T00.59-b5fcd62/results.html
```

## Accessing test parameters

每个test都有一组参数，可以通过`self.params.get($name, $path=None, $default=None)`进行访问，其中：

```
name - name of the parameter (key) --- 参数的名称(键)
path - where to look for this parameter (when not specified uses mux-path) --- 在何处查找此参数(未指定时使用mux-path)
default - what to return when param not found --- 未找到参数时返回的内容
```

path有点棘手。Avocado使用tree来表示参数。在简单的场景中，您不必担心，您将在默认路径中找到所有值（values），但最终您可能需要check-out [Test parameters](http://avocado-framework.readthedocs.io/en/61.0/TestParameters.html)以了解详细信息。

假设您的test收到以下参数 (在下一节中您将学习如何执行这些参数):

```
$ avocado variants -m examples/tests/sleeptenmin.py.data/sleeptenmin.yaml --variants 2
...
Variant 1:    /run/sleeptenmin/builtin, /run/variants/one_cycle
    /run/sleeptenmin/builtin:sleep_method => builtin
    /run/variants/one_cycle:sleep_cycles  => 1
    /run/variants/one_cycle:sleep_length  => 600
...
```

在test中, 您可以通过以下方法访问这些参数:

```
self.params.get("sleep_method")    # returns "builtin"
self.params.get("sleep_cycles", '*', 10)    # returns 1
self.params.get("sleep_length", "/*/variants/*"  # returns 600
```

注意：在可能发生冲突的复杂情况下，path非常重要，因为当有多个具有相同key的values匹配查询时，avocado会引发异常。如前所述，您可以通过使用特定path或定义custom mux-path来避免这些情况，它允许指定解析层次结构。更多细节可以在[Test parameters](http://avocado-framework.readthedocs.io/en/61.0/TestParameters.html)中找到。

## Running multiple variants of tests

在上一节中, 我们描述了如何处理参数。现在, 让我们来看看如何生成它们, 并使用不同的参数执行测试。

variants subsystem允许创建参数的多个variants（变体）, 以及使用这些参数variants（变体）执行测试。此subsystem是可插拔的, 因此您可以使用自定义插件来生成variants（变体）。为了简单起见，我们使用Avocado的主要实现，名为"yaml_to_mux"。

该"yaml_to_mux"插件接受YAML文件。这将创建一个树状结构，将变量存储为参数，并使用自定义标记（tags）将locations标记为"multiplex" domains。

```
The “yaml_to_mux” plugin accepts YAML files. Those will create a tree-like structure, store the variables as parameters and use custom tags to mark locations as “multiplex” domains.
```

让我们使用`examples/tests/sleeptenmin.py.data/sleeptenmin.yaml`文件作为示例:

```
sleeptenmin: !mux
    builtin:
        sleep_method: builtin
    shell:
        sleep_method: shell
variants: !mux
    one_cycle:
        sleep_cycles: 1
        sleep_length: 600
    six_cycles:
        sleep_cycles: 6
        sleep_length: 100
    one_hundred_cycles:
        sleep_cycles: 100
        sleep_length: 6
    six_hundred_cycles:
        sleep_cycles: 600
        sleep_length: 1
```

其产生以下结构和参数：

```
$ avocado variants -m examples/tests/sleeptenmin.py.data/sleeptenmin.yaml --summary 2 --variants 2
Multiplex tree representation:
 ┗━━ run
      ┣━━ sleeptenmin
      ┃    ╠══ builtin
      ┃    ║     → sleep_method: builtin
      ┃    ╚══ shell
      ┃          → sleep_method: shell
      ┗━━ variants
           ╠══ one_cycle
           ║     → sleep_length: 600
           ║     → sleep_cycles: 1
           ╠══ six_cycles
           ║     → sleep_length: 100
           ║     → sleep_cycles: 6
           ╠══ one_hundred_cycles
           ║     → sleep_length: 6
           ║     → sleep_cycles: 100
           ╚══ six_hundred_cycles
                 → sleep_length: 1
                 → sleep_cycles: 600

Multiplex variants (8):

Variant builtin-one_cycle-f659:    /run/sleeptenmin/builtin, /run/variants/one_cycle
    /run/sleeptenmin/builtin:sleep_method => builtin
    /run/variants/one_cycle:sleep_cycles  => 1
    /run/variants/one_cycle:sleep_length  => 600

Variant builtin-six_cycles-723b:    /run/sleeptenmin/builtin, /run/variants/six_cycles
    /run/sleeptenmin/builtin:sleep_method => builtin
    /run/variants/six_cycles:sleep_cycles => 6
    /run/variants/six_cycles:sleep_length => 100

Variant builtin-one_hundred_cycles-633a:    /run/sleeptenmin/builtin, /run/variants/one_hundred_cycles
    /run/sleeptenmin/builtin:sleep_method         => builtin
    /run/variants/one_hundred_cycles:sleep_cycles => 100
    /run/variants/one_hundred_cycles:sleep_length => 6

Variant builtin-six_hundred_cycles-a570:    /run/sleeptenmin/builtin, /run/variants/six_hundred_cycles
    /run/sleeptenmin/builtin:sleep_method         => builtin
    /run/variants/six_hundred_cycles:sleep_cycles => 600
    /run/variants/six_hundred_cycles:sleep_length => 1

Variant shell-one_cycle-55f5:    /run/sleeptenmin/shell, /run/variants/one_cycle
    /run/sleeptenmin/shell:sleep_method  => shell
    /run/variants/one_cycle:sleep_cycles => 1
    /run/variants/one_cycle:sleep_length => 600

Variant shell-six_cycles-9e23:    /run/sleeptenmin/shell, /run/variants/six_cycles
    /run/sleeptenmin/shell:sleep_method   => shell
    /run/variants/six_cycles:sleep_cycles => 6
    /run/variants/six_cycles:sleep_length => 100

Variant shell-one_hundred_cycles-586f:    /run/sleeptenmin/shell, /run/variants/one_hundred_cycles
    /run/sleeptenmin/shell:sleep_method           => shell
    /run/variants/one_hundred_cycles:sleep_cycles => 100
    /run/variants/one_hundred_cycles:sleep_length => 6

Variant shell-six_hundred_cycles-1e84:    /run/sleeptenmin/shell, /run/variants/six_hundred_cycles
    /run/sleeptenmin/shell:sleep_method           => shell
    /run/variants/six_hundred_cycles:sleep_cycles => 600
    /run/variants/six_hundred_cycles:sleep_length => 1
```

您可以看到它为每个multiplex domain创建了所有可能的variants（变体）。它们由YAML文件中的!mux 标签（tag）定义，并在树视图中以单行显示（与具有values的单个nodes的双线进行比较）。总共它会为每个test产生8个variants（变体）：

```
$ avocado run --mux-yaml examples/tests/sleeptenmin.py.data/sleeptenmin.yaml -- passtest.py
JOB ID     : cc7ef22654c683b73174af6f97bc385da5a0f02f
JOB LOG    : /home/medic/avocado/job-results/job-2017-01-22T11.26-cc7ef22/job.log
 (1/8) passtest.py:PassTest.test;builtin-one_cycle-f659: PASS (0.01 s)
 (2/8) passtest.py:PassTest.test;builtin-six_cycles-723b: PASS (0.01 s)
 (3/8) passtest.py:PassTest.test;builtin-one_hundred_cycles-633a: PASS (0.01 s)
 (4/8) passtest.py:PassTest.test;builtin-six_hundred_cycles-a570: PASS (0.01 s)
 (5/8) passtest.py:PassTest.test;shell-one_cycle-55f5: PASS (0.01 s)
 (6/8) passtest.py:PassTest.test;shell-six_cycles-9e23: PASS (0.01 s)
 (7/8) passtest.py:PassTest.test;shell-one_hundred_cycles-586f: PASS (0.01 s)
 (8/8) passtest.py:PassTest.test;shell-six_hundred_cycles-1e84: PASS (0.01 s)
RESULTS    : PASS 8 | ERROR 0 | FAIL 0 | SKIP 0 | WARN 0 | INTERRUPT 0
JOB TIME   : 0.16 s
```

还有其他选项可以影响参数（params），请使用命令`avocado run -h`查看详细信息，或查看使用文档[Test parameters](http://avocado-framework.readthedocs.io/en/61.0/TestParameters.html)。

### 执行case

```
[root@cephL]# vi examples/tests/sleeptenmin.py.data/sleeptenmin.yaml
sleeptenmin: !mux
    builtin:
        sleep_method: builtin
    shell:
        sleep_method: shell
variants: !mux
    one_cycle:
        sleep_cycles: 1
        sleep_length: 600
    six_cycles:
        sleep_cycles: 6
        sleep_length: 100
    one_hundred_cycles:
        sleep_cycles: 100
        sleep_length: 6
    six_hundred_cycles:
        sleep_cycles: 600
        sleep_length: 1

[root@cephL]# vi /root/mac/examples/tests/passtest.py
#!/usr/bin/env python
from avocado import main
from avocado import Test
class PassTest(Test):
    """
    Example test that passes.
    :avocado: tags=fast
    """
    def test(self):
        """
        A test simply doesn't have to fail in order to pass
        """
        pass
if __name__ == "__main__":
    main()

[root@cephL]# avocado run --mux-yaml /root/mac/examples/tests/sleeptenmin.py.data/sleeptenmin.yaml -- /root/mac/examples/tests/passtest.py
JOB ID     : 2ae9467667984d8c686087859e953a20bfd294a2
JOB LOG    : /root/avocado/job-results/job-2018-06-06T02.37-2ae9467/job.log
 (1/8) /root/mac/examples/tests/passtest.py:PassTest.test;builtin-one_cycle-f659: PASS (0.01 s)
 (2/8) /root/mac/examples/tests/passtest.py:PassTest.test;builtin-six_cycles-723b: PASS (0.01 s)
 (3/8) /root/mac/examples/tests/passtest.py:PassTest.test;builtin-one_hundred_cycles-633a: PASS (0.02 s)
 (4/8) /root/mac/examples/tests/passtest.py:PassTest.test;builtin-six_hundred_cycles-a570: PASS (0.01 s)
 (5/8) /root/mac/examples/tests/passtest.py:PassTest.test;shell-one_cycle-55f5: PASS (0.01 s)
 (6/8) /root/mac/examples/tests/passtest.py:PassTest.test;shell-six_cycles-9e23: PASS (0.01 s)
 (7/8) /root/mac/examples/tests/passtest.py:PassTest.test;shell-one_hundred_cycles-586f: PASS (0.01 s)
 (8/8) /root/mac/examples/tests/passtest.py:PassTest.test;shell-six_hundred_cycles-1e84: PASS (0.02 s)
RESULTS    : PASS 8 | ERROR 0 | FAIL 0 | SKIP 0 | WARN 0 | INTERRUPT 0 | CANCEL 0
JOB TIME   : 0.50 s
JOB HTML   : /root/avocado/job-results/job-2018-06-06T02.37-2ae9467/results.html
```



