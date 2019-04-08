---
title: LTP使用
date: 2019-04-01 22:13:25
tags: LTP
---





```
[root@centos7 ltp-install]# ./runltp -h

    usage: runltp [ -a EMAIL_TO ] [ -c NUM_PROCS ] [ -C FAILCMDFILE ] [ -T TCONFCMDFILE ]
    [ -d TMPDIR ] [ -D NUM_PROCS,NUM_FILES,NUM_BYTES,CLEAN_FLAG ] -e [ -f CMDFILES(,...) ]
    [ -g HTMLFILE] [ -i NUM_PROCS ] [ -l LOGFILE ] [ -m NUM_PROCS,CHUNKS,BYTES,HANGUP_FLAG ]
    -N -n [ -o OUTPUTFILE ] -p -q -Q [ -r LTPROOT ] [ -s PATTERN ] [ -t DURATION ]
    -v [ -w CMDFILEADDR ] [ -x INSTANCES ] [ -b DEVICE ] [-B LTP_DEV_FS_TYPE]
[ -F LOOPS,PERCENTAGE ] [ -z BIG_DEVICE ] [-Z  LTP_BIG_DEV_FS_TYPE]
					
					# 将所有报告通过EMAIL方式发送到指定E-mail Address
    -a EMAIL_TO     EMAIL all your Reports to this E-mail Address
    				# 在后台额外增加CPU负载的情况下运行LTP
    -c NUM_PROCS    Run LTP under additional background CPU load
                    [NUM_PROCS = no. of processes creating the CPU Load by spinning over sqrt()
                                 (Defaults to 1 when value)]
    -C FAILCMDFILE  Command file with all failed test cases.
    -T TCONFCMDFILE Command file with all test cases that are not fully tested.
    -d TMPDIR       Directory where temporary files will be created.
    -D NUM_PROCS,NUM_FILES,NUM_BYTES,CLEAN_FLAG
                    Run LTP under additional background Load on Secondary Storage (Seperate by comma)
                    [NUM_PROCS   = no. of processes creating Storage Load by spinning over write()]
                    [NUM_FILES   = Write() to these many files (Defaults to 1 when value 0 or undefined)]
                    [NUM_BYTES   = write these many bytes (defaults to 1GB, when value 0 or undefined)]
                    [CLEAN_FLAG  = unlink file to which random data written, when value 1]
    -e              Prints the date of the current LTP release
    -f CMDFILES     Execute user defined list of testcases (separate with ',')
-F LOOPS,PERCENTAGE Induce PERCENTAGE Fault in the Kernel Subsystems, and, run each test for LOOPS loop
    -g HTMLFILE     Create an additional HTML output format
    -h              Help. Prints all available options.
    -i NUM_PROCS    Run LTP under additional background Load on IO Bus
                    [NUM_PROCS   = no. of processes creating IO Bus Load by spinning over sync()]
    -K DMESG_LOG_DIR
Log Kernel messages generated for each test cases inside this directory
    -l LOGFILE      Log results of test in a logfile.
    -m NUM_PROCS,CHUNKS,BYTES,HANGUP_FLAG
                    Run LTP under additional background Load on Main memory (Seperate by comma)
                    [NUM_PROCS   = no. of processes creating main Memory Load by spinning over malloc()]
                    [CHUNKS      = malloc these many chunks (default is 1 when value 0 or undefined)]
                    [BYTES       = malloc CHUNKS of BYTES bytes (default is 256MB when value 0 or undefined) ]
                    [HANGUP_FLAG = hang in a sleep loop after memory allocated, when value 1]
-M CHECK_TYPE
[CHECK_TYPE=1 => Full Memory Leak Check tracing children as well]
[CHECK_TYPE=2 => Thread Concurrency Check tracing children as well]
[CHECK_TYPE=3 => Full Memory Leak & Thread Concurrency Check tracing children as well]
					# 运行所有网络测试
    -N              Run all the networking tests.
    				# 在后台额外增加网络流量的方式运行LTP
    -n              Run LTP with network traffic in background.
    				# 将测试输出重定向到文件
    -o OUTPUTFILE   Redirect test output to a file.
    				# 友好可读格式日志文件。
    -p              Human readable format logfiles.
    				# 打印较少的详细输出到屏幕。 这意味着不在kernel log中记录测试的开始信息
    -q              Print less verbose output to screen. This implies not logging start of the test in kernel log.
                    # 不在kernel log中记录测试开始信息
    -Q              Don't log start of test in kernel log.
    				# 安装testsuite的绝对路径
    -r LTPROOT      Fully qualified path where testsuite is installed.
    				# 随机化测试顺序
    -R              Randomize test order.
    				# 只运行与模式匹配的测试用例
    -s PATTERN      Only run test cases which match PATTERN.
    				# 跳过SKIPFILE中指定的测试
    -S SKIPFILE     Skip tests specified in SKIPFILE
    				# 在给定的时间内持续执行testsuite。例如：
    -t DURATION     Execute the testsuite for given duration. Examples:
                      -t 60s = 60 seconds
                      -t 45m = 45 minutes
                      -t 24h = 24 hours
                      -t 2d  = 2 days
                    # 执行testsuite迭代次数
    -I ITERATIONS   Execute the testsuite ITERATIONS times.
    				# 使用wget获取用户的测试用例列表
    -w CMDFILEADDR  Uses wget to get the user's list of testcases.
    				# 运行此testsuite的多个实例
    -x INSTANCES    Run multiple instances of this testsuite.
    				# 有些测试需要一个unmounted的块设备才能正常运行
    -b DEVICE       Some tests require an unmounted block device to run correctly.
                       # 测试块设备的文件系统
    -B LTP_DEV_FS_TYPE The file system of test block devices.
    				# 某些测试需要大型未安装的块设备才能正常运行
    -z BIG_DEVICE   Some tests require a big unmounted block device to run correctly.
                    		# 大设备的文件系统
    -Z  LTP_BIG_DEV_FS_TYPE The file system of the big device



    example: runltp -c 2 -i 2 -m 2,4,10240,1 -D 2,10,10240,1 -p -q  -l /tmp/result-log.3140 -o /tmp/result-output.3140 -C /tmp/result-failed.3140 -d /root/ltp-install



```

