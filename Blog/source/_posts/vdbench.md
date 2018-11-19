---
title: vdbench
date: 2017-10-31 21:38:14
tags: Tools
---

[root@centos001 vdbench]# lsb_release -a
LSB Version:    :core-4.1-amd64:core-4.1-noarch:cxx-4.1-amd64:cxx-4.1-noarch:desktop-4.1-amd64:desktop-4.1-noarch:languages-4.1-amd64:languages-4.1-noarch:printing-4.1-amd64:printing-4.1-noarch
Distributor ID:    CentOS
Description:    CentOS Linux release 7.3.1611 (Core)
Release:    7.3.1611
Codename:    Core

# vdbench单机测试 #

1、下载vdbench50406.zip

2、解压vdbench50406.zip

3、安装java，jdk
[root@centos001 ~]# yum install java-1.8.0-openjdk unzip net-tools iotop -y
[root@yujiangcontrol ~]# java -version
openjdk version "1.8.0_144"
OpenJDK Runtime Environment (build 1.8.0_144-b01)
OpenJDK 64-Bit Server VM (build 25.144-b01, mixed mode)

4、配置vdbench
[root@centos001 ~]# unzip vdbench50406.zip
[root@centos001 ~]# ./vdbench -t

5、配置文件
[root@centos001 ~]# vi yujiang
sd=sd1,lun=/dev/sdb,openflags=o_direct
wd=wd1,sd=sd1,seekpct=random,rdpct=0,xfersize=8192
rd=rd1,wd=wd*,elapsed=120,interval=10,iorate=max
openflags=o_direct 直接对块设备进行读写

6、执行配置文件
[root@centos001 ~]# sudo ./vdbench -f yujiang 

# vdbench多机测试 #

[root@centos001 vdbench]# cat /etc/hosts
192.168.30.100  centos001       centos001.test.com
192.168.30.103  plana003        plana003.test.com
192.168.30.104  plana004        plana004.test.com
192.168.30.105  plana005        plana005.test.com

1、生成sshkey
[root@centos001 vdbench]# ssh-keygen
与其他slave节点做ssh互信 
[root@centos001 vdbench]# ssh-copy-id plana003
[root@centos001 vdbench]# ssh-copy-id plana004
[root@centos001 vdbench]# ssh-copy-id plana005
[root@centos001 vdbench]# ssh plana003.test.com
[root@centos001 vdbench]# ssh plana004.test.com
[root@centos001 vdbench]# ssh plana005.test.com

2、把vdbench拷贝到每台slave节点，包括master节点，路径相同
/root/vdbench

3、每台slave机监听，ssh不用监听
[root@plana003 vdbench]# ./vdbench rsh
[root@plana004 vdbench]# ./vdbench rsh
[root@plana005 vdbench]# ./vdbench rsh

备注：
Host Definition (HD)
These parameters are ONLY needed when running Vdbench in a multi-host environment or if you want to override the number of JVMs used in a single-host environment.
See also Host Definition parameter detail.

hd=default Sets defaults for all HDs that are entered later
hd=localhost Sets values for the current host
hd=host_label Specify a host label.
system=host_name Host IP address or network name, e.g. xyz.customer.com
vdbench=vdbench_dir_name Where to find Vdbench on a remote host if different from current.
jvms=nnn How many slaves to use. See Multi JVM execution.
shell=rsh | ssh | vdbench How to start a Vdbench slave on a remote system.
user=xxxx Userid on remote system Required.
clients=nn This host will simulate the running of multiple ‘clients’. Very  useful if you want to simulate numerous clients for file servers  without having all the hardware.
mount=”mount xxx …” This mount command is issued on the target host after the  possibly needed mount directories have been created.
rg=name Unique name for this Replay Group (RG).
devices=(xxx,yyy,….) The device numbers from Swat’s flatfile.bin.gz to be replayed.

Storage Definition (SD)
See also Storage Definition Parameter Detail.
This set of parameters identifies each physical or logical volume manager volume or file system  file used in the requested workload. Of course, with a file system file, the file system takes the  responsibility of all I/O: reads and writes can and will be cached (see also openflags=) and  Vdbench will not have control over physical I/O. However, Vdbench can be used to test file  system file performance (See also File system testing).
Example: sd=sd1,lun=/dev/rdsk/cxt0d0s0,threads=8

sd=default Sets defaults for all SDs that are entered later.
sd=name Unique name for this Storage Definition (SD).
host=name Name of host where this SD can be found. Default ‘localhost’
lun=lun_name Name of raw disk or file system file.
align=nnn Generate logical byte address in ‘nnn’ byte boundaries, not using default ‘xfersize’ boundaries.
count=(nn,mm) Creates a sequence of SD parameters.
hitarea=nn See read hit percentage for an explanation. Default 1m.
journal=xxx Directory name for journal file for data validation
offset=nnn At which offset in a lun to start I/O.
openflags=(flag,..) Pass specific flags when opening a lun or file
range=(nn,mm) Use only a subset 'range=nn': Limit Seek Range of this SD.
replay=(group,..) Replay Group(s) using this SD.
replay=(nnn,..) Device number(s) to select for Swat Vdbench replay
resetbus=nnn Issue ioctl (USCSI_RESET_ALL) every nnn seconds. Solaris only
resetlun=nnn Issue ioctl (USCSI_RESET) every nnn seconds. Solaris only
size=nn Size of the raw disk or file to use for workload. Optional unless you  want Vdbench to create a disk file for you.
streams=(nn,mm) Create independent sequential streams on the same device.
threads=nn Maximum number of concurrent outstanding I/O for this SD. Default 8

Workload Definition (WD)
See also Workload Definition Parameter Detail.
The Workload Definition parameters describe what kind of workload must be executed using the storage definitions entered.
Example: wd=wd1,sd=(sd1,sd2),rdpct=100,xfersize=4k

wd=default Sets defaults for all WDs that are entered later.
wd=name Unique name for this Workload Definition (WD)
sd=xx Name(s) of Storage Definition(s) to use
host=host_label Which host to run this workload on. Default localhost.
iorate=nn Requested fixed I/O rate for this workload.
openflags=(flag,..) Pass specific flags when opening a lun or file.
priority=nn I/O priority to be used for this workload.
range=(nn,nn) Limit seek range to a defined range within an SD.
rdpct=nn Read percentage. Default 100.
rhpct=nn Read hit percentage. Default 0.
seekpct=nn Percentage of random seeks. Default seekpct=100 or seekpct=random.
skew=nn Percentage of skew that this workload receives from the total I/O  rate.
stride=(min,max) To allow for skip-sequential I/O.
threads=nn Only available during SD concatenation.
whpct=nn Write hit percentage. Default 0.
xfersize=nn Data transfer size. Default 4k.
xfersize=(n,m,n,m,..) Specify a distribution list with percentages.
xfersize=(min,max,align) Generate xfersize as a random value between min and max.

Run Definition (RD)
See also Run Definition Parameter Detail.
The Run Definition parameters define which of the earlier defined workloads need to be  executed, what I/O rates need to be generated, and how long the workload will run. One Run  Definition can result in multiple actual workloads, depending on the parameters used.
Example: rd=run1,wd=(wd1,wd2),iorate=1000,elapsed=60,interval=5
There is a separate list of RD parameters for file system testing.

rd=default Sets defaults for all RDs that are entered later.
rd=name Unique name for this Run Definition (RD).
wd=xx Workload Definitions to use for this run.
sd=xxx Which SDs to use for this run (Optional).
curve=(nn,nn,..) Data points to generate when creating a performance curve.
distribution=(x[,variable] I/O inter arrival time calculations: exponential, uniform, or  deterministic. Default exponential.
elapsed=nn Elapsed time for this run in seconds. Default 30 seconds.
maxdata=nnn Stop the run after nnn bytes have been read or written, e.g.
maxdata=200g. Vdbench will stop at the lower of elapsed= and maxdata=.
endcmd=cmd Execute command or script at the end of the last run
(for)compratio=nn Multiple runs for each compression percentage.
(for)hitarea=nn Multiple runs for each hit area size.
(for)hpct=nn Multiple runs for each read hit percentage.
(for)rdpct=nn Multiple runs for each read percentage.
(for)seekpct=nn Multiple runs for each seek percentage.
(for)threads=nn Multiple runs for each thread count.
(for)whpct=nn Multiple runs for each write hit percentage.
(for)xfersize=nn Multiple runs for each data transfer size.
Most forxxx parameters may be abbreviated to their regular name, e.g. xfersize=(..,..)
interval=nn Reporting interval in seconds. Default 'min(elapsed/2,60)'
iorate=(nn,nn,nn,…) One or more I/O rates.
iorate=curve Create a performance curve.
iorate=max Run an uncontrolled workload.
iorate=(nn,ss,…) nn,ss: pairs of I/O rates and seconds of duration for this I/O  rate. See also 'distribution=variable'.
openflags=xxxx Pass specific flags when opening a lun or file
pause=nn Sleep 'nn' seconds before starting next run.
replay=(filename,split=split_dir,repeat=nn)
-'filename': Replay file name used for Swat Vdbench replay
- 'split_dir': directory used to do the replay file split.
- 'nn': how often to repeat the replay.
startcmd=cmd Execute command or script at the beginning of the first run
warmup=nn Override warmup period.

 
1、多slave host不同块设备
For instance if a lun is /dev/rdsk/a on hosta but it is named /dev/rdsk/b on hostb then you’ll have to tell Vdbench about it.
sd=sd1,lun=/dev/rdsk/a,host=hosta,lun=/dev/rdsk/b,host=hostb
By default Vdbench assumes that the lun names on each host are identical.


2、
[root@centos001 vdbench]# cat example5
hd=default,vdbench=/root/vdbench,user=root,shell=ssh
hd=plana003,system=plana003.test.com
hd=plana004,system=plana004.test.com
hd=plana005,system=plana005.test.com

sd=sd1,host=*,lun=/dev/sdb,size=10m,openflags=o_direct
wd=wd1,sd=sd*,rdpct=100,xf=4k
rd=rd1,wd=wd1,el=3,in=1,io=10,warmup=30
解释：
hd=default以下hd都使用后面的默认值
vdbench到每台slave机的相同目录寻找vdbench程序
user以root用户登录远程slave机
shell以ssh协议登录
hd=plana003远程slave机标签
system=plana003.test.com ip地址或者配置hosts文件，用来ssh连接slave机








