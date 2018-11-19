---
title: 使用Vundle管理Vim插件
date: 2018-04-17 19:17:49
tags: VIM
---

工欲善其事，必先利其器。这篇文章记录怎样在Linux上使用Vundle管理Vim插件，提高效率。

步骤很简单，大体如下：

### 系统环境

```
[root@centos7 ~]# yum install -y redhat-lsb
[root@centos7 ~]# lsb_release -a
LSB Version:	:core-4.1-amd64:core-4.1-noarch:cxx-4.1-amd64:cxx-4.1-noarch:desktop-4.1-amd64:desktop-4.1-noarch:languages-4.1-amd64:languages-4.1-noarch:printing-4.1-amd64:printing-4.1-noarch
Distributor ID:	CentOS
Description:	CentOS Linux release 7.4.1708 (Core) 
Release:	7.4.1708
Codename:	Core
```

### Install vim and git

```
[root@centos7 ~]# yum install -y git vim
```

### Set up Vundle

```
[root@centos7 ~]# git clone https://github.com/VundleVim/Vundle.vim.git ~/.vim/bundle/Vundle.vim
```

### Configure Plugins 

```
[root@centos7 ~]# vim ~/.vimrc
set nocompatible              " be iMproved, required
filetype off                  " required

" set the runtime path to include Vundle and initialize
set rtp+=~/.vim/bundle/Vundle.vim
call vundle#begin()
" alternatively, pass a path where Vundle should install plugins
"call vundle#begin('~/some/path/here')

" let Vundle manage Vundle, required
Plugin 'VundleVim/Vundle.vim'

" All of your Plugins must be added before the following line
call vundle#end()            " required
filetype plugin indent on    " required
" To ignore plugin indent changes, instead use:
"filetype plugin on
"
" Brief help
" :PluginList       - lists configured plugins
" :PluginInstall    - installs plugins; append `!` to update or just :PluginUpdate
" :PluginSearch foo - searches for foo; append `!` to refresh local cache
" :PluginClean      - confirms removal of unused plugins; append `!` to auto-approve removal
"
" see :h vundle for more details or wiki for FAQ
" Put your non-Plugin stuff after this line
```

打开~/.vimrc，并添加你要安装的Plugins

```
[root@centos7 ~]# vim ~/.vimrc
call vundle#begin()
" 在其之间添加你要安装的Plugins，例如我这里安装一个NERDTree
Plugin 'scrooloose/nerdtree'
call vundle#end()
```

### Install Plugins

启动vim并运行:PluginInstall

```
[root@centos7 ~]# vim ~/.vimrc
:PluginInstall
```

配置已安装NERDTree插件

```
[root@centos7 ~]# vim ~/.vimrc
添加以下，注意<F2>后面有空格，配置好以后，使用vim打开任意文件就可以使用F2来调出NERDTree了
map <F2> :NERDTreeToggle<CR>
```

当然vim不止这一个插件，这里只是介绍一下怎样配置，其他好玩的插件请自己发掘。

###  vim常用选项

http://gohom.win/2015/06/08/Vim-Setup/