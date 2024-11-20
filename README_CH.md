
# cowsql 

[![CI Tests](https://github.com/cowsql/cowsql/actions/workflows/build-and-test.yml/badge.svg)](https://github.com/cowsql/cowsql/actions/workflows/build-and-test.yml) [![codecov](https://codecov.io/gh/cowsql/cowsql/branch/main/graph/badge.svg)](https://codecov.io/gh/cowsql/cowsql)

**注意**：中文文档有可能未及时更新，请以最新的英文[readme](./README.md)为准。

[cowsql](https://cowsql.dev) (/ˈkaʊ,siːkwəl/
[listen](http://ipa-reader.xyz/?text=%CB%88ka%CA%8A%2Csi%CB%90kw%C9%99l)) 是一个用C语言开发的可嵌入的，支持流复制的数据库引擎，具备高可用性和自动故障转移功能。

cowsql通过网络协议扩展[SQLite](https://sqlite.org/)，将应用程序的各个实例连接在一起，让它们作为一个高可用的集群，而不依赖外部数据库。

"cowsql"名字大致取意["宠物与牲口应用思辨"](https://iamondemand.com/blog/devops-concepts-pets-vs-cattle/)，删除或重建一个存储使用cowsql应用的特定节点，应用基本使用不受影响。

## Canonical dqlite 分支

cowsql 是Canonical [dqlite](https://github.com/canonical/dqlite)项目一个分支，dqlite原先也是cowsql作者[自己](https://github.com/canonical/dqlite/commits?author=freeekanayaka)当时在Canonical工作时候开发的。

## 设计亮点

- 使用[libuv](https://libuv.org/)实现异步单线程的事件循环机制

- 针对SQLite 原始数据类型优化的自定义网络协议

- 基于[Raft](https://raft.github.io/)算法的数据复制及其高效[C-raft](https://github.com/cowsql/raft)实现 

## license

cowsql库是在略微修改的 LGPLv3 版本下发布的，其中包括一个版权例外，允许用户在他们的项目中静态链接这个库的代码并按照自己的条款发布最终作品。如有需要，请查看完整[license](./LICENSE)文件。

## 兼容性

cowsql 在 Linux 上运行，由于C-raft 的 libuv 后端的实现，需要一个支持 [native async
I/O](https://man7.org/linux/man-pages/man2/io_setup.2.html) 的内核(注意不要和[POSIX AIO](https://man7.org/linux/man-pages/man7/aio.7.html)混淆)。

## 尝试使用

查看和了解cowsql的最简单方式是使用绑定了Go cowsql的demo样例程序，Go cowsql的使用可以参考它的项目文档[relevant
documentation](https://github.com/cowsql/go-cowsql#demo)。

## 视频

在 FOSDEM 2020 上有一个关于cowsql的演讲视频，您可以在[此处](https://fosdem.org/2020/schedule/event/cowsql/)观看。

## 网络协议

如果您想编写客户端，请参阅[网络协议](https://cowsql.io/docs/protocol)文档。

## 下载

如果您使用的是基于 Debian 的系统，您可以从 cowsql 的[dev PPA](https://launchpad.net/~cowsql/+archive/ubuntu/dev) 获得最新的开发版本：

```bash
sudo add-apt-repository ppa:cowsql/dev
sudo apt-get update
sudo apt-get install libcowsql-dev
```

## 源码构建

为了编译构建libcowsql，您需要准备：

- 较新版本的libuv（v1.18.0或之后的版本）

- 较新版本的sqlite3-dev

- 构建好的[C-raft](https://github.com/cowsql/raft)库

您的linux发行版应该已经为您提供了预构建的 libuv 共享库和 libsqlite3-dev,就不需要在下载了，否则还需要下载这两个依赖。

编译raft库运行如下命令：

```bash
git clone https://github.com/cowsql/raft.git
cd raft
autoreconf -i
./configure
make
sudo make install
cd ..
```

所有依赖的库都下载好后，运行如下命令手动编译cowsql库：

```bash
autoreconf -i
./configure
make
sudo make install
```

## 注意事项

当环境变量LIBRAFT_TRACE在启动时被设置，将启用详细跟踪。
