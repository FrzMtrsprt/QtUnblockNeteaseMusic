# QtUnblockNeteaseMusic

[![CodeFactor](https://www.codefactor.io/repository/github/frzmtrsprt/qtunblockneteasemusic/badge)](https://www.codefactor.io/repository/github/frzmtrsprt/qtunblockneteasemusic)
![GitHub commit activity](https://img.shields.io/github/commit-activity/m/FrzMtrsprt/QtUnblockNeteaseMusic)
![GitHub all releases](https://img.shields.io/github/downloads/FrzMtrsprt/QtUnblockNeteaseMusic/total)

适用于 UnblockNeteaseMusic 的桌面应用，使用 Qt 制作。

English version: [README_en.md](README_en.md)

## 特性
- 指定 UnblockNeteaseMusic 服务器的启动参数
- 保存上次运行选项
- 显示服务器的实时日志输出
- 支持暗色主题

## 支持
原始版本：[nondanee/UnblockNeteaseMusic](https://github.com/nondanee/UnblockNeteaseMusic)

增强版本：[UnblockNeteaseMusic/server](https://github.com/UnblockNeteaseMusic/server)

## 截图
![截图](https://github.com/FrzMtrsprt/QtUnblockNeteaseMusic/raw/main/screenshot.png)

## 使用方法
1. 从 [releases](https://github.com/FrzMtrsprt/QtUnblockNeteaseMusic/releases) 下载最新版本
2. 如果你想使用脚本服务器而不是打包的可执行文件，请先安装 [Node.js Runtime](https://nodejs.org/zh-cn/download)
3. 从 [nondanee/UnblockNeteaseMusic](https://github.com/nondanee/UnblockNeteaseMusic) 或 [UnblockNeteaseMusic/server](https://github.com/UnblockNeteaseMusic/server) 下载 UnblockNeteaseMusic 服务器，并放置于 `QtUnblockNeteaseMusic.exe` 所在目录
```
QtUnblockNeteaseMusic  
├─ ...  
├─ QtUnblockNeteaseMusic.exe  
└─ server  
   ├─ ...  
   └─ app.js
```
或
```
QtUnblockNeteaseMusic  
├─ ...  
├─ QtUnblockNeteaseMusic.exe  
└─ unblockneteasemusic.exe
```
4. 运行 `QtUnblockNeteaseMusic.exe` 并设置启动参数
5. 在网易云客户端中设置相应的代理，享受音乐吧！
- 在个人设备上使用时，尽量避免使用默认的 `8080` 端口。为避免来自其他设备的访问，可以将主机地址设为 `127.0.0.1`。
- 新版网易云客户端有更严格的 SSL 验证，如果碰到了连接失败的问题，可以启用 HTTPS 端口并安装 UnblockNeteaseMusic CA 证书。
- 要安装 UnblockNeteaseMusic CA 证书，右键点击 `QtUnblockNeteaseMusic.exe`，选择 `以管理员身份运行`，然后点击`高级`菜单中的`安装证书`。

## 构建
1. 安装 Qt 开发环境（Qt 6.5.0）
2. 克隆仓库
3. 在 Qt Creator 或 VS Code（需要额外插件）中打开项目
4. 编译并运行
- 如果要生成带有依赖库的完整应用，请将 `QtUnblockNeteaseMusic.exe` 复制到新目录下并运行 `windeployqt.exe .\QtUnblockNeteaseMusic.exe`

## 感谢
这个小工具的诞生离不开以下项目：
- [Qt](https://github.com/qt): 强大而易用的开发框架
- [nondanee/UnblockNeteaseMusic](https://github.com/nondanee/UnblockNeteaseMusic)：一切始于这个项目
- [UnblockNeteaseMusic/server](https://github.com/UnblockNeteaseMusic/server)：维护并增强 UNM 服务器

## 待解决
- ~~增强版服务器的日志输出乱码（[#791](https://github.com/UnblockNeteaseMusic/server/issues/791)）~~已在上游修复（[#798](https://github.com/UnblockNeteaseMusic/server/pull/798)）
- 使用 CMake 完成所有构建 ~~已完成~~  
  Qt 6 中的 lupdate 无法与 CMake 配合使用，因此仍然需要手动运行 lupdate
- 未测试 / 未支持 macOS、Linux 、 Windows 8 及以下等平台
- 我是菜鸡