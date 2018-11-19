---
title: Repo for Microsoft Windows and Linux
date: 2018-06-15 22:28:18
tags: git-repo
---

这是E.S.R.Labs Repo —— 就像Google Repo，但它也可以在Microsoft Windows运行。Repo是Google建立在Git之上的repository管理工具。必要时，Repo会统一许多Git repositories上传到版本控制系统，并使部分开发工作流程自动化。Repo并不意味着取代Git，只是为了更容易在多个repositories的环境中使用Git。repo command是一个可执行的Python脚本，您可以将其放在任何路径。在处理源文件时，您将使用Repo进行跨网络操作。例如，使用Repo命令，可以将多个repositories中的文件下载到本地工作目录中。

## Setup steps for Microsoft Windows

修复权限以允许创建符号链接

如果您是Administrators group的member，则必须关闭[User Access Control](https://support.microsoft.com/en-us/products/windows?os=windows-10) (UAC)，然后重新启动计算机。

否则，您必须调整您的用户权限才能获得[SeCreateSymbolicLinkPrivilege](https://stackoverflow.com/questions/6722589/using-windows-mklink-for-linking-2-files)权限。editrights工具是作为Microsoft Windows的git-repo的一部分提供的。

Highly experimental（除开发此功能外，请勿使用！）：如果您不想使用符号链接，而是文件夹和文件的硬链接（hardlinks），则必须在`~/.gitconfig`中设置以下内容:

```
[portable]
      windowsNoSymlinks = true
```

这不需要上述所说的设置权限。

已知问题：硬链接被git销毁，例如，当你删除一个branch时（breaks .git/config）。这会破坏project workspace！

### Download and install Git

下载Git（http://git-scm.com/downloads）

将Git添加到path环境变量中：例如C:\Program Files (x86)\Git\cmd;

将MinGW添加到path环境变量中：例如C:\Program Files (x86)\Git\bin;

### Download and install Python

下载Python 2.7+（https://www.python.org/downloads/）

将Python添加到path环境变量中：例如C:\Python27;

注意：git-repo在实验环境支持Python 3.x

### 使用Windows Command Shell或Git Bash下载并安装Repo

Windows Command Shell

```
md %USERPROFILE%\bin
curl https://raw.githubusercontent.com/esrlabs/git-repo/stable/repo > %USERPROFILE%/bin/repo
curl https://raw.githubusercontent.com/esrlabs/git-repo/stable/repo.cmd > %USERPROFILE%/bin/repo.cmd
```

Windows PowerShell

```
md $env:USERPROFILE\bin
curl https://raw.githubusercontent.com/esrlabs/git-repo/stable/repo > $env:USERPROFILE/bin/repo
curl https://raw.githubusercontent.com/esrlabs/git-repo/stable/repo.cmd > $env:USERPROFILE/bin/repo.cmd
```

Git Bash

```
mkdir ~/bin
curl https://raw.githubusercontent.com/esrlabs/git-repo/stable/repo > ~/bin/repo
curl https://raw.githubusercontent.com/esrlabs/git-repo/stable/repo.cmd > ~/bin/repo.cmd
```

将Repo添加path环境变量：%USERPROFILE%\bin;

创建一个指向％USERPROFILE％的HOME环境变量（OpenSSH需要找到它的.ssh目录）。

创建一个GIT_EDITOR环境变量，其中包含一个editor可执行文件作为值。为此，首先将editor可执行文件的主目录添加到path环境变量中。例如，GIT_EDITOR可以设置为"notepad++.exe"，"gvim.exe"。

### Setup steps for Linux

下载并安装Git和Python

```
sudo apt-get install git-core
sudo apt-get install python
```

下载并安装Repo

```
$ mkdir ~/bin
$ PATH=~/bin:$PATH
$ curl https://raw.githubusercontent.com/esrlabs/git-repo/stable/repo > ~/bin/repo
$ chmod a+x ~/bin/repo
```

### 用法

有关git-repo用法的更多详细说明，请访问[git](http://source.android.com/source/using-repo.html)-repo。

### 常见问题解答

- 我无法看到repo状态的任何颜色

用于repo状态的pager默认设置为less，除非另外配置，否则将忽略转义序列。您可以将环境变量LESS设置为-FRSX以减少处理颜色。 （您可能还想将LESSCHARSET设置为utf-8）

它仍然不起作用！

对pager有更多的选择。它的行为并不像less一样，但它对你来说可能已经足够了。

```
git config --global core.pager more
```

或者，环境变量GIT_PAGER可以设置为more。

- 在使用repo init初始化我的repo时，我在Windows Command Shell中遇到WindowsError

如果在输出开始时出现警告，表示GPG不可用

```
warning: gpg (GnuPG) is not available.
warning: Installing it is strongly encouraged.
```

你必须添加gpg.exe到你的PATH变量。可执行文件可以在Git安装文件夹`$GIT\usr\bin`中找到。当你使用Git Bash时，`$Git\usr\bin`文件夹已经被添加到你的PATH中。

### Developer Information

与official google repo同步

为了与official google repo git同步，这里是与official google repo的tag v1.12.33同步的命令：

```
# add google git-repo remote with tag
git remote add googlesource https://android.googlesource.com/tools/repo/
git checkout v1.12.33 -b google-latest

# checkout basis for resync
git checkout google-git-repo-base -b update
git merge --allow-unrelated-histories -Xtheirs --squash google-latest
git commit -m "Update: google git-repo v1.12.33"
git rebase stable

# solve conflicts; keep portability in mind

git checkout stable
git rebase update

# cleanup
git branch -D update
git branch -D google-latest
```

### 创建一个新的签名版本

用于创建新版本repo的命令：

```
git tag -s -u KEYID v0.4.16 -m "COMMENT"
git push origin stable:stable
git push origin v0.4.16
```

替换KEYID（类似0x ..）

v0.4.16（两次）

用更多的解释替换COMMENT